/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : gamecore.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "boardtheme.h"
#include "chessgame.h"
#include "chessplayer.h"
#include "config.h"
#include "fontloader.h"
#include "gamecore.h"
#include "menu.h"
#include "menuitem.h"
#include "objfile.h"
#include "options.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_thread.h"
#include "texture.h"

#include <cmath>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

// Menu string constants
static const string AI_DIFFICULTY_EASY = "Easy";
static const string AI_DIFFICULTY_MEDIUM = "Medium";
static const string AI_DIFFICULTY_HARD = "Hard";

static const string PLAYER_BRUTAL = "Brutal";
static const string PLAYER_HUMAN = "Human";
static const string PLAYER_RANDOM = "Random";

GameCore * GameCore::m_instance = 0;

GameCore * GameCore::get_Instance()
{
	if (m_instance == 0)
		m_instance = new GameCore;
	return m_instance;
}

void GameCore::destroy()
{
	delete m_theme;
	m_theme = 0;
	delete m_set;
	m_set;
	delete this;
}

void GameCore::init(const ChessGame & cg, BoardTheme * bt, PieceSet * ps)
{
	m_game	= cg;
	m_theme	= bt;
	m_set	= ps;
}

bool GameCore::load()
{
	m_options = Options::getInstance();

	m_set->load();
	m_theme->load();
	m_game.newGame();
	m_game.startGame();
	m_rotatey = 0.0;
	m_isWaitingForPromotion = false;
	m_rotate = false;
	m_loaded = true;
	m_suggestedwhiteplayer = m_options->getPlayer1String();
	m_suggestedblackplayer = m_options->getPlayer2String();
	return true;
}

bool GameCore::loadGL()
{
	if(m_loadthread) {
		SDL_WaitThread(m_loadthread, NULL);
		m_loadthread = 0;
	}

	m_set->loadGL();
	m_theme->loadGL();

	m_defaultcur = SDL_GetCursor();
	char blank[64];
	for(int i = 0; i < 64; i++) {
		blank[i] = 0;
	}

	m_blankcur = SDL_CreateCursor((Uint8*)blank, (Uint8*)blank, 8, 8, 0, 0);
	m_glloaded = true;

	if(!m_thinkthread) {
		spawnThinkThread();
	}

	return true;
}

void GameCore::unloadGL()
{
	if(!m_glloaded) {
		return;
	}

	m_set->unloadGL();
	m_theme->unloadGL();

	SDL_FreeCursor(m_blankcur);

	m_glloaded = false;
}

void GameCore::draw()
{
	if(m_loaded) {
		if(!m_glloaded) {
			loadGL();
		}
		
		toBoardSpace();

		// Fix the lighting so it rotates with the board (same as shadow light)
		GLfloat light_position[4];
		light_position[0] = 10;
		light_position[1] = 12;
		light_position[2] = 6;
		light_position[3] = 1;
		//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        if(m_options->reflections) {
			drawReflections();
		}
		
		// Blend the draw reflections with the board.
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		m_theme->draw(m_game.getState());
		glDisable( GL_BLEND );
		
        if(m_options->shadows) {
			projectShadows();
		}

		m_set->draw(m_game.getState());


		// Draw a spinning pawn (like the loading screen) if we are waiting on the player
		if(!m_game.getCurrentPlayer()->isHuman()) {
			glLoadIdentity();

            double height = static_cast<double>(m_options->getResolutionHeight());
            double width = static_cast<double>(m_options->getResolutionWidth());
            double aspectRatioDiff = (height/width)  - 0.75;
            glTranslated(33.5 - 50.0*aspectRatioDiff, -23.5, -106);

			glRotatef(SDL_GetTicks()/3.5,0,1,0);
			glRotatef(30,0,0,1);
			glTranslatef(0, -3.5, 0);

			// Make sure we can see this above the board
			glDisable(GL_DEPTH_TEST);
			Piece p(m_game.getCurrentPlayer()->getColor(), Piece::PAWN);
			glScalef(7.0, 7.0, 7.0);
			// Slightly transparent
			m_set->drawPiece(&p, 0.9, false);
			glScalef(1/7.0, 1/7.0, 1/7.0);
			glEnable(GL_DEPTH_TEST);
		}

		if (m_menu.isActive()) {
			m_menu.draw();
        }

        // See if the game has ended, if it has increment a timer that runs
        // to allow the player to see what happened before displaying the
        // end game menu.
        if (m_endgametimer.started()) {
            m_endgametimer++;
        } else if (m_endgametimer.done()) {
            buildMenu();
            m_menu.pushOptionsSet("Game Over");
            string condition;
            if (m_game.getBoard().isCheckMate(Piece::BLACK)) {
                condition = "Checkmate - White Wins";
            }
            else if (m_game.getBoard().isCheckMate(Piece::WHITE)) {
                condition = "Checkmate - Black Wins";
            }
            else {
                condition = "Stalemate";
            }
            m_menu.setHeader(condition);
            m_menu.activate();
            m_endgametimer.resetDone();
        }

    } else {
		glLoadIdentity();
		drawLoadingScreen();
		// Extra delay during the loading screen to avoid choking the CPU
		SDL_Delay(25);
	}		
}

bool GameCore::handleEvent(SDL_Event& e)
{
	if(!m_loaded)
		return false;

    // Needs to handle mouse, keyboard, and some application events
	// Most window level tasks will be handled in main
	if(m_menu.handleEvent(e)) {
		return true;
	}
	
	if (e.type == SDL_MOUSEMOTION) {
		if(m_rotate) {
			GLfloat viewport[4];
			glGetFloatv(GL_VIEWPORT, viewport);
			m_rotatey += 360*(e.motion.xrel / viewport[2]);
			m_rotatex += 360*(e.motion.yrel / viewport[3]);
			if(m_rotatex < -42)
				m_rotatex = -42;
			if(m_rotatex > 48)
				m_rotatex = 48;
			return true;
		}
		m_mousex = e.motion.x;
		m_mousey = e.motion.y;
		updateMouseBoardPos();
		m_theme->hoverPosition(m_mousepos);
		m_set->mousePosition(m_mouseboardx, m_mouseboardy);
		m_set->hoverPosition(m_game.getState(), m_mousepos);
		return true;
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		updateMouseBoardPos();
		bool needMove = m_game.getCurrentPlayer()->needMove();
		if (e.button.button == SDL_BUTTON_RIGHT) {
			// If a piece has already been picked up, release the piece.
			if(m_firstclick.isValid() && needMove) {
				SDL_SetCursor(m_defaultcur);
				m_set->deselectPosition();
				m_firstclick.invalidate();
			} else {
				m_rotate = true;
				m_mousex = e.button.x;
				m_mousey = e.button.y;
				m_theme->hoverPosition(BoardPosition());
				m_set->hoverPosition(m_game.getState(), BoardPosition());
				SDL_ShowCursor(SDL_DISABLE);
				SDL_WM_GrabInput(SDL_GRAB_ON);
			}
		}
		else if (!needMove) {
		} else if (e.button.button == SDL_BUTTON_LEFT) {
			if(m_rotate)
				return false;
			
			// Check to see if we're waiting for the player to select a piece to
			// promote his pawn to.
			if (this->isWaitingForPromotion()) {
				Piece::Type t = getPromotionSelection(m_mousepos);
				if (t == Piece::PAWN) {
					return false;
				}
				BoardMove bm(m_firstclick, m_secondclick, m_game.getBoard().getPiece(m_firstclick));
				bm.setPromotion(t);
				if (m_game.getCurrentPlayer()->needMove()) {
					m_game.getCurrentPlayer()->sendMove(bm);
				}
				m_set->drawPromotionSelector(false);
				this->setIsWaitingForPromotion(false);
				return true;
			}

			// Check to see if we've clicked yet
			if (m_firstclick.isValid()) {
				if(m_firstclick == m_mousepos) {
					// Clicked on the start position, unselect
					SDL_SetCursor(m_defaultcur);
					m_set->deselectPosition();
					m_firstclick.invalidate();
				} 
				else {
					// We've already clicked once, so save this new click position and
					// make the move from m_firstclick to the new position
					BoardMove bm(m_firstclick, m_mousepos, m_game.getBoard().getPiece(m_firstclick));
					if (bm.needPromotion()) {
						m_secondclick = m_mousepos;
						if (m_game.getCurrentPlayer()->isHuman()) {
							m_set->drawPromotionSelector(true);
							this->setIsWaitingForPromotion(true);
							SDL_SetCursor(m_defaultcur);
						}
						else {
							// TODO - Get promotion from AI
						}
					} 
					else if (m_game.getCurrentPlayer()->needMove()) {
						// The move doesn't need promotion, so simply send it off
						// to the current player.
						m_game.getCurrentPlayer()->sendMove(bm);
					}
				}
			} 
			else {	
				// We haven't clicked yet so set m_firstclick to this click's position
				if(m_mousepos.isValid() && m_game.getBoard().isOccupied(m_mousepos)) {
					// Make sure player is selecting his own piece
					if(m_game.getTurn() == m_game.getBoard().getPiece(m_mousepos)->color()) {
						SDL_SetCursor(m_blankcur);
						m_firstclick = m_mousepos;
						m_set->selectPosition(m_mousepos);
					}
				}
			}
		}
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_RIGHT && m_rotate) {
			m_rotate = false;
			SDL_ShowCursor(SDL_ENABLE);
			SDL_WM_GrabInput(SDL_GRAB_OFF);
			SDL_WarpMouse(m_mousex, m_mousey);
		}
	}
	else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				buildMenu();
				if (m_menu.isActive())
					m_menu.deactivate();
				else {
					// If a piece is picked up, drop it before showing the menu
					if(m_firstclick.isValid()) {
						SDL_SetCursor(m_defaultcur);
						m_set->deselectPosition();
						m_firstclick.invalidate();
					}
					m_menu.activate();
				}
			}
	}
	else if (e.type == SDL_USEREVENT) {
		if(e.user.code == 0) {
			// Received notification that the player is done thinking
			ChessPlayer *player = (ChessPlayer*)e.user.data1;
			//m_set->animateMove(player->getMove());
			if(m_game.tryMove(player->getMove())) {
				m_game.getCurrentPlayer()->opponentMove(player->getMove(), m_game.getState());
				SDL_SetCursor(m_defaultcur);
				m_set->deselectPosition();
				m_firstclick.invalidate();
			}
			SDL_WaitThread(m_thinkthread, NULL);
			m_thinkthread = NULL;

			// Only temporary, really want to do this after animation is done
	    	if (!(m_game.getBoard().containsCheckMate() || m_game.getState().isDraw())) {
				spawnThinkThread();
			} else {
                m_endgametimer = Timer(Timer::LINEAR);
                m_endgametimer.setDuration(1.0);
                m_endgametimer.start();
			}
		}
		else if (e.user.code == Menu::eQUIT) {
			SDL_Event quitevent;
			quitevent.type = SDL_QUIT;
			SDL_PushEvent(&quitevent);
		}
		else if (e.user.code == Menu::eREFLECTTOG) {
            m_options->reflections = !m_options->reflections;
		}
		else if (e.user.code == Menu::eSHADOWTOG) {
            m_options->shadows = !m_options->shadows;
		}
		else if (e.user.code == Menu::eHISTORYARROWSTOG) {
			m_theme->toggleHistoryArrows();
		}
		else if (e.user.code == Menu::eBBRUTALPLYCHANGED) {
			// Set the AI ply depth based on the ai difficulty string
			string difficulty = m_blackbrutalplychoices->getCurrentChoice();
			if (AI_DIFFICULTY_EASY == difficulty) {
                m_options->brutalplayer2ply = EASY;
			} 
			else if (AI_DIFFICULTY_MEDIUM == difficulty) {
				m_options->brutalplayer2ply = MEDIUM;
			} 
			else if (AI_DIFFICULTY_HARD == difficulty) {
				m_options->brutalplayer2ply = HARD;
			}
			BrutalPlayer* player = dynamic_cast<BrutalPlayer*>(m_game.getPlayer2());
			if (player) {
				player->setPly(m_options->brutalplayer2ply);
			}
		}
		else if (e.user.code == Menu::eBLACKPLAYERCHANGED) {
			m_suggestedblackplayer = m_blackplayerchoices->getCurrentChoice();
			m_blackbrutalplychoices->setCollapsed(PLAYER_BRUTAL != m_suggestedblackplayer);
		}
		else if (e.user.code == Menu::eWBRUTALPLYCHANGED) {
			// Set the AI ply depth based on the ai difficulty string
			string difficulty = m_whitebrutalplychoices->getCurrentChoice();
			if (AI_DIFFICULTY_EASY == difficulty) {
				m_options->brutalplayer1ply = EASY;
			} 
			else if (AI_DIFFICULTY_MEDIUM == difficulty) {
				m_options->brutalplayer1ply = MEDIUM;
			} 
			else if (AI_DIFFICULTY_HARD == difficulty) {
				m_options->brutalplayer1ply = HARD;
			}
			BrutalPlayer* player = dynamic_cast<BrutalPlayer*>(m_game.getPlayer1());
			if (player) {
				player->setPly(m_options->brutalplayer1ply);
			}
		}
		else if (e.user.code == Menu::eWHITEPLAYERCHANGED) {
			m_suggestedwhiteplayer = m_whiteplayerchoices->getCurrentChoice();
			m_whitebrutalplychoices->setCollapsed(PLAYER_BRUTAL != m_suggestedwhiteplayer);
		}
		else if (e.user.code == Menu::eSTARTNEWGAME) {
			ChessPlayer * whiteplayer = PlayerFactory(m_suggestedwhiteplayer);
			BrutalPlayer* brutalplayer = dynamic_cast<BrutalPlayer*>(whiteplayer);
			if (brutalplayer) {
				brutalplayer->setPly(m_options->brutalplayer1ply);
			}
			whiteplayer->setIsWhite(true);
			ChessPlayer * blackplayer = PlayerFactory(m_suggestedblackplayer);
			brutalplayer = dynamic_cast<BrutalPlayer*>(blackplayer);
			if (brutalplayer) {
				brutalplayer->setPly(m_options->brutalplayer2ply);
			}
			blackplayer->setIsWhite(false);
			m_game.setPlayer1(whiteplayer);
			m_game.setPlayer2(blackplayer);
			m_game.newGame();
			if (m_thinkthread) {
				SDL_KillThread(m_thinkthread);
			}
			spawnThinkThread();
			SDL_Event backEvent;
			backEvent.type = SDL_USEREVENT;
			backEvent.user.code = Menu::eBACK;
			SDL_PushEvent(&backEvent);
			SDL_Event backEvent2;
			backEvent2.type = SDL_USEREVENT;
			backEvent2.user.code = Menu::eBACK;
			SDL_PushEvent(&backEvent2);
			SDL_Event backEvent3;
			backEvent3.type = SDL_USEREVENT;
			backEvent3.user.code = Menu::eBACK;
			SDL_PushEvent(&backEvent3);
		}
	}
	return false;
}

bool GameCore::preload()
{
	if(m_loadpawn.load("../models/pawn.obj")) {
		// add debugging
	} else if(m_loadpawn.load(MODELS_DIR + string("pawn.obj"))) {
		// add debugging
	} else {
		cerr << "Failed to load pawn." << endl;
		return false;
	}
			
	m_loadpawn.findNorms();
	m_loadpawn.setScale(m_loadpawn.scale()*12);
	
	if(FontLoader::loadFont("sans", "../fonts/ZEROES__.TTF", 32)) {
		// add debugging
	} else if(FontLoader::loadFont("sans", FONTS_DIR + string("ZEROES__.TTF"), 32)) {
		// add debugging
	} else {
		cerr << "Failed to load fonts." << endl;
		return false;
	}

	if(m_logotexture.load("../art/brutalchesslogo.png")) {
		// add debugging
	} else if(m_logotexture.load(ART_DIR + string("brutalchesslogo.png"))) {
		// add debugging
	} else {
		cerr << "Failed to load logo" << endl;
		return false;
	}

	m_logotexture.loadGL();
	m_preloaded = true;
	return true;
}

void GameCore::drawLoadingScreen()
{
	if(!m_preloaded)
		preload();
	glTranslated(0,0,-106);

	// Drawing the Brutal Chess Logo
	m_logotexture.use();
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
		glNormal3d(0,0,-1);
		glTexCoord2d(0.0,0.0);
		glVertex3f(-35.0,25.0,0.0);
		glTexCoord2d(0.0,1.0);
		glVertex3f(-35.0,-10.0,0.0);
		glTexCoord2d(1.0,1.0);
		glVertex3f(35.0,-10.0,0.0);
		glTexCoord2d(1.0,0.0);
		glVertex3f(35.0,25.0,0.0);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// Alpha value to create a pulsing effect
	double c = 0.6+0.4*cos(0.15*SDL_GetTicks()*3.1415/180.0);

	// Drawing the Loading text	
	glEnable(GL_BLEND);
	glColor4f(1.0, 1.0, 1.0, c);
	glTranslated(28, -18, 0);
	glScaled(1/10.0, 1/10.0, 1/10.0);
	FontLoader::print(-28,-7,"Loading...");
	glScaled(10, 10, 10);
	glDisable(GL_BLEND);

	// Spinning pawn
	glRotatef(SDL_GetTicks()/2.5,0,1,0);
	glRotatef(30,0,0,1);

	m_loadpawn.draw();
}

void GameCore::projectShadows()
{
	glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// Generate project matrix for planar shadows
	GLfloat shadowMat[4][4];
	GLfloat groundplane[4];

	// Specified in board space, y is up
	groundplane[0] = 0;
	groundplane[1] = 1;
	groundplane[2] = 0;
	groundplane[3] = 0;
	GLfloat lightpos[4];
	lightpos[0] = 6;
	lightpos[1] = 12;
	lightpos[2] = 10;
	// 1, since this is a point light
	lightpos[3] = 1;

	GLfloat mat[16];

	for(int i = 0; i < 16; i++)
		mat[i] = 0;

	mat[0] = lightpos[0];
	mat[4] = lightpos[1];
	mat[8] = lightpos[2];
	mat[12] = lightpos[3];

	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(mat);
	glRotated(m_rotatex, 1, 0, 0);
	glRotated(m_rotatey, 0, 1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	glPopMatrix();

	lightpos[0] = mat[0] + 4;
	lightpos[2] = mat[8] - 4;

	GLfloat dot;

	dot = groundplane[0] * lightpos[0] +
		groundplane[1] * lightpos[1] +
		groundplane[2] * lightpos[2] +
		groundplane[3] * lightpos[3];
	
	shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
	shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
	shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
	shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];

	shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
	shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
	shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
	shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];

	shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
	shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
	shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
	shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];

	shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
	shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
	shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
	shadowMat[3][3] = dot - lightpos[3] * groundplane[3];
	
	glPushMatrix();
		
	// Apply the project matrix
	glMultMatrixf((GLfloat*)shadowMat);

  	glDisable(GL_LIGHTING);

	// Only want to write to the stencil buffer
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Write a 1 to where the shadows should appear
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	m_set->draw(m_game.getState());

	// Only want to draw where the stencil buffer is 1
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 0xffffffff);
	glPopMatrix();

	// Prevent depth buffer glitch
	glTranslatef(0, 0.01, 0);
	
	// Draw the actual shadow, a black 50% alpha polygon
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 0, 0, 0.5);
	glBegin( GL_QUADS );
	glNormal3d( 0, 7, 0 );
	glVertex3d( -0.1, 0,  0.1 );
	glVertex3d( 8.1, 0, 0.1 );
	glVertex3d( 8.1, 0, -8.1 );
	glVertex3d( -0.1, 0,  -8.1 );
	glEnd();
	
	// Restore OpenGL state
	glTranslatef(0, -0.01, 0);
	glPopAttrib();
}

void GameCore::drawReflections()
{
	// This plane is drawn and then erased. The purpose is to obtain the mouse
	// coordinates, and set the stencil buffer for the reflections
	// It is exactly the same size as the board, and is in exactly the same spot
	glDisable( GL_DEPTH_TEST );	
	glEnable( GL_STENCIL_TEST );
	glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
	glStencilFunc( GL_ALWAYS, 1, 0xffffffff );
	
	glBegin( GL_QUADS );
		glNormal3d( 0, 7, 0 );
		glVertex3d( 0.0, 0.0,  0.0 );
		glVertex3d( 8.0, 0.0,  0.0 );
		glVertex3d( 8.0, 0.0, -8.0 );
		glVertex3d( 0.0, 0.0, -8.0 );
	glEnd();

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Only draw if the stencil buffer has a 1 here
	glEnable( GL_DEPTH_TEST );
	glStencilFunc( GL_EQUAL, 1, 0xffffffff );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	
	GLfloat light_position[4];
	glGetLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Invert over the board plane
	glScalef( 1, -1, 1 );
	glCullFace(GL_FRONT);

	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glEnable(GL_LIGHT1);
	glDisable(GL_LIGHT0);
	
	// Draw the reflected pieces
	m_set->draw(m_game.getState());
	
	// Return to normal (not inverted)
	glCullFace(GL_BACK);
	glScalef( 1, -1, 1 );
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable( GL_STENCIL_TEST );
	glClear( GL_STENCIL_BUFFER_BIT );
}

void GameCore::buildMenu()
{
	static bool built = false;
	if(!built) {
	/* Main
		New game
		Options ->
		Quit
	*/
	m_menu.addMenuItem("Brutal Chess", new ChangeMenuItem("New Game", "New Game"));
	m_menu.addMenuItem("Brutal Chess", new ChangeMenuItem("Game Options", "Game Options"));
	m_menu.addMenuItem("Brutal Chess", new ChangeMenuItem("Graphics", "Graphics"));
	m_menu.addMenuItem("Brutal Chess", new ActionItem("Quit", Menu::eQUIT));
	m_menu.addMenuItem("Brutal Chess", new SeparatorItem());
	m_menu.addMenuItem("Brutal Chess", new ActionItem("Back To Game", Menu::eBACK));

	// New Game Menu
	m_whiteplayerchoices = new ChoicesItem("White Player");
	m_whiteplayerchoices->addChoice(PLAYER_BRUTAL, Menu::eWHITEPLAYERCHANGED);
	m_whiteplayerchoices->addChoice(PLAYER_HUMAN, Menu::eWHITEPLAYERCHANGED);
	m_whiteplayerchoices->addChoice(PLAYER_RANDOM, Menu::eWHITEPLAYERCHANGED);
	m_whiteplayerchoices->setChoice(PLAYER_HUMAN);
	m_whitebrutalplychoices = new ChoicesItem("  Difficulty");
	m_whitebrutalplychoices->addChoice(AI_DIFFICULTY_EASY, Menu::eWBRUTALPLYCHANGED);
	m_whitebrutalplychoices->addChoice(AI_DIFFICULTY_MEDIUM, Menu::eWBRUTALPLYCHANGED);
	m_whitebrutalplychoices->addChoice(AI_DIFFICULTY_HARD, Menu::eWBRUTALPLYCHANGED);
    m_whitebrutalplychoices->setChoice(AI_DIFFICULTY_MEDIUM);
	m_blackplayerchoices = new ChoicesItem("Black Player");
	m_blackplayerchoices->addChoice(PLAYER_BRUTAL, Menu::eBLACKPLAYERCHANGED);
	m_blackplayerchoices->addChoice(PLAYER_HUMAN, Menu::eBLACKPLAYERCHANGED);
	m_blackplayerchoices->addChoice(PLAYER_RANDOM, Menu::eBLACKPLAYERCHANGED);
	m_blackplayerchoices->setChoice(PLAYER_BRUTAL);
	m_blackbrutalplychoices = new ChoicesItem("  Difficulty");
	m_blackbrutalplychoices->addChoice(AI_DIFFICULTY_EASY, Menu::eBBRUTALPLYCHANGED);
	m_blackbrutalplychoices->addChoice(AI_DIFFICULTY_MEDIUM, Menu::eBBRUTALPLYCHANGED);
	m_blackbrutalplychoices->addChoice(AI_DIFFICULTY_HARD, Menu::eBBRUTALPLYCHANGED);
    m_blackbrutalplychoices->setChoice(AI_DIFFICULTY_MEDIUM);
	m_menu.addMenuItem("New Game", m_whiteplayerchoices);
	m_menu.addMenuItem("New Game", m_whitebrutalplychoices);
	m_menu.addMenuItem("New Game", m_blackplayerchoices);
	m_menu.addMenuItem("New Game", m_blackbrutalplychoices);
	m_menu.addMenuItem("New Game", new ActionItem("Start New Game", Menu::eSTARTNEWGAME));
	m_menu.addMenuItem("New Game", new SeparatorItem());
	m_menu.addMenuItem("New Game", new ActionItem("Back", Menu::eBACK));
    
	// Game Options
    m_menu.addMenuItem("Game Options", new ToggleItem("History Arrows", Menu::eHISTORYARROWSTOG, m_options->historyarrows));
	m_menu.addMenuItem("Game Options", new SeparatorItem());
	m_menu.addMenuItem("Game Options", new ActionItem("Back", Menu::eBACK));

	/* Graphics
		Resolution		640x480 800x600 1024x768 1280x1024 1400x1050 1600x1200
		Fullscreen
        Apply resolution
		-
		Reflections		On Off
		Shadows			On Off
	*/
	m_resolutionchoices = new ChoicesItem("Resolution");
	m_resolutionchoices->addChoice("640x480", Menu::e640X480);
	m_resolutionchoices->addChoice("800x600", Menu::e800X600);
	m_resolutionchoices->addChoice("1024x768", Menu::e1024X768);
	m_resolutionchoices->addChoice("1280x1024", Menu::e1280X1024);
	m_resolutionchoices->addChoice("1400x1050", Menu::e1400X1050);
	m_resolutionchoices->addChoice("1600x1200", Menu::e1600X1200);
	m_resolutionchoices->setChoice("800x600");
	m_menu.addMenuItem("Graphics", m_resolutionchoices);
    m_fullscreentoggle = new ToggleItem("Fullscreen", Menu::eFULLSCREENTOG, m_options->fullscreen);
	m_menu.addMenuItem("Graphics", m_fullscreentoggle);
	m_menu.addMenuItem("Graphics", new ActionItem("Apply resolution", Menu::eAPPLYRES));
	m_menu.addMenuItem("Graphics", new SeparatorItem());
    m_menu.addMenuItem("Graphics", new ToggleItem("Reflections", Menu::eREFLECTTOG, m_options->reflections));
    m_menu.addMenuItem("Graphics", new ToggleItem("Shadows", Menu::eSHADOWTOG, m_options->shadows));
	m_menu.addMenuItem("Graphics", new SeparatorItem());
	m_menu.addMenuItem("Graphics", new ActionItem("Back", Menu::eBACK));

	m_menu.addMenuItem("Game Over", new ChangeMenuItem("New Game", "New Game"));
	m_menu.addMenuItem("Game Over", new ActionItem("Repeat Game", Menu::eSTARTNEWGAME));
	m_menu.addMenuItem("Game Over", new SeparatorItem());
    m_menu.addMenuItem("Game Over", new ActionItem("Main Menu", Menu::eBACK));
	m_menu.addMenuItem("Game Over", new ActionItem("Quit", Menu::eQUIT));

	m_menu.pushOptionsSet("Brutal Chess");
	built = true;
	}
}

void GameCore::setResolution(std::string resolution)
{
	m_resolutionchoices->setChoice(resolution);
}

void GameCore::setFullscreen(bool fullscreen)
{
	m_fullscreentoggle->setOn(fullscreen);
}


void GameCore::toBoardSpace()
{
	// Setup board space coordinates
	glLoadIdentity();
	glTranslated(0, 0, -106);
	glRotated(42, 1, 0, 0);

	glRotated(m_rotatex, 1, 0, 0);
	glRotated(m_rotatey, 0, 1, 0);

	glScaled(7, 7, 7);
	glTranslated(-4.0, 0.0, 4.0);
}
void GameCore::updateMouseBoardPos()
{
	// Make sure the screen is cleared
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	toBoardSpace();

	// Draw a large quad to project the mouse position onto
	glBegin(GL_QUADS);
	glVertex3d(-13, 0,  9.5);
	glVertex3d( 20, 0,  9.5);
	glVertex3d( 20, 0, -13);
	glVertex3d(-13, 0, -13);
	glEnd();
	
	GLdouble mvmatrix[16];
	GLdouble pjmatrix[16];
	GLint viewport[4];

	// Get the current window size, modelview and projection matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, pjmatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Measure mouse y coordinate from the bottom of the window
	int mousey = viewport[3] - m_mousey;
	GLfloat mousedepth;

	// Get the depth of the single pixel under the mouse
	glReadPixels(m_mousex, mousey, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, 
			&mousedepth);

	GLdouble x, y, z;
	// Project the mouse position into board coordinates
	gluUnProject(m_mousex, mousey, mousedepth, mvmatrix, pjmatrix, viewport, 
			&x, &y, &z);

	m_mouseboardx = x;
	m_mouseboardy = z;

	m_mousepos = BoardPosition((int)floor(x), -(int)ceil(z));
}

int callThink(void *pt)
{
	ChessGame* game = (ChessGame*)pt;
	// Give the player time to think
	game->getCurrentPlayer()->think(game->getState());

	// Finished thinking, let the main thread know
	SDL_Event thinkevent;
	thinkevent.type = SDL_USEREVENT;
	thinkevent.user.code = 0;
	thinkevent.user.data1 = game->getCurrentPlayer();
	thinkevent.user.data2 = NULL;
	SDL_PushEvent(&thinkevent);
	return 0;
}

void GameCore::spawnThinkThread()
{
	m_thinkthread = SDL_CreateThread(callThink, &m_game);
	if(m_thinkthread == NULL) {
		cerr << "Unable to create think thread: " << SDL_GetError() << endl;
	}

	/* 
	if(!m_game.getCurrentPlayer()->isHuman()) {
		SDL_WaitThread(m_thinkthread, NULL);
	}
	*/
}

Piece::Type GameCore::getPromotionSelection(const BoardPosition & bp)
{
	if (m_game.getCurrentPlayer()->isWhite()) {
		if (bp == BoardPosition('c', 9))
			return Piece::QUEEN;
		else if (bp == BoardPosition('d', 9))
			return Piece::KNIGHT;
		else if (bp == BoardPosition('e', 9))
			return Piece::BISHOP;
		else if (bp == BoardPosition('f', 9))
			return Piece::ROOK;
	}
	else {
		if (bp == BoardPosition('c', 0))
			return Piece::ROOK;
		else if (bp == BoardPosition('d', 0))
			return Piece::BISHOP;
		else if (bp == BoardPosition('e', 0))
			return Piece::KNIGHT;
		else if (bp == BoardPosition('f', 0))
			return Piece::QUEEN;
	}
	return Piece::PAWN;
}

// End of file gamecore.cpp
