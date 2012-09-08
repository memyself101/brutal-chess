/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : granitetheme.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include <iostream>
#include <string>

#include "boardtheme.h"
#include "config.h"
#include "SDL_opengl.h"
#include "texture.h"

using namespace std;

static const int BOARDSIZE = 8;

GraniteTheme::GraniteTheme()
{
	for(int i = 0; i < Board::BOARDSIZE; i++)
		for(int j = 0; j < Board::BOARDSIZE; j++)
			m_overlayalpha[i][j] = 0.0;
}

void GraniteTheme::draw(const ChessGameState & cgs)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	// Draw the squares
	bool white_square = false;
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < BOARDSIZE; ++i) {
		for (int j = 0; j < BOARDSIZE; ++j) {
			
			if(white_square) {
				m_whitetexture.use();
			} else {
				m_blacktexture.use();
			}

			// Draw the square
			glColor4d(1.0, 1.0, 1.0, 0.7);
			glNormal3f(0.0, 7.0, 0.0);
			glBegin(GL_QUADS);
				glTexCoord2d( j*1/8.0, i*1/8.0 );
				glVertex3f(i+1, 0.0, -j  );				
				glTexCoord2d( (j+1)*1/8.0, i*1/8.0 );
				glVertex3f(i+1, 0.0, -j-1);
				glTexCoord2d( (j+1)*1/8.0, (i+1)*1/8.0 );
				glVertex3f(i,   0.0, -j-1);
				glTexCoord2d( j*1/8.0, (i+1)*1/8.0 );
				glVertex3f(i,   0.0, -j  );
			glEnd();

			if(m_overlaytimer[i][j].started()) {
				m_overlaytimer[i][j]++;
				m_overlayalpha[i][j] += m_overlaytimer[i][j].change();
			}

			if(m_overlaytimer[i][j].done()) {
				m_overlayalpha[i][j] = 0.0;
				m_overlaytimer[i][j].resetDone();
			}

			if(m_overlayalpha[i][j] != 0) {
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_DEPTH_TEST);
				glColor4d(0.5, 0.5, 0.5, m_overlayalpha[i][j]);
				glNormal3f(0.0, 7.0, 0.0);
				glBegin(GL_QUADS);
					glVertex3f(i+1, 0.0, -j  );				
					glVertex3f(i+1, 0.0, -j-1);
					glVertex3f(i,   0.0, -j-1);
					glVertex3f(i,   0.0, -j  );
				glEnd();
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_TEXTURE_2D);
			}

			white_square = !white_square;
		}
		white_square = !white_square;
	}
	glDisable(GL_TEXTURE_2D);
	
	drawMoveArrows(cgs);
	
	// TODO - OMFG This is ugly, find a more algorithmic way to do this.
	// Draw the edges of the board
	glPushMatrix();
	glEnable( GL_LIGHTING );
	// Move back to the center of the board.
	glTranslated( 4, 0, -4 );	
	glBegin( GL_QUADS );
		// Darker of the edge colors.
		glColor4d( 0.2, 0.2, 0.2, 0.95 );
		glNormal3d( 0, 7, 0);
		// White side.
		glVertex3d( -4.1, 0,  4.1 );
		glVertex3d(  4.1, 0,  4.1 );
		glVertex3d(  4.0, 0,  4.0 );
		glVertex3d( -4.0, 0,  4.0 );
		// Right of white side.
		glVertex3d( -4.1, 0, -4.1 );
		glVertex3d( -4.1, 0,  4.1 );
		glVertex3d( -4.0, 0,  4.0 );
		glVertex3d( -4.0, 0, -4.0 );
		// Black side.
		glVertex3d(  4.1, 0, -4.1 );
		glVertex3d( -4.1, 0, -4.1 );
		glVertex3d( -4.0, 0, -4.0 );
		glVertex3d(  4.0, 0, -4.0 );
		// Left of white side.
		glVertex3d(  4.1, 0,  4.1 );
		glVertex3d(  4.1, 0, -4.1 );
		glVertex3d(  4.0, 0, -4.0 );
		glVertex3d(  4.0, 0,  4.0 );
		// Drawing sides of the darker portion.
		glColor3d( 0.2, 0.2, 0.2 );
		glNormal3d( 0, 0, 7);
		glVertex3d( -4.1, -0.075, 4.1 );
		glVertex3d(  4.1, -0.075, 4.1 );
		glVertex3d(  4.1, 0, 4.1 );
		glVertex3d( -4.1, 0, 4.1 );

		glNormal3d( 7, 0, 0 );
		glVertex3d(  4.1, -0.075,  4.1 );
		glVertex3d(  4.1, -0.075, -4.1 );
		glVertex3d(  4.1, 0, -4.1 );
		glVertex3d(  4.1, 0,  4.1 );
		
		glNormal3d( 0, 0, -7 );
		glVertex3d(  4.1, -0.075, -4.1 );
		glVertex3d( -4.1, -0.075, -4.1 );
		glVertex3d( -4.1, 0, -4.1 );
		glVertex3d(  4.1, 0, -4.1 );

		glNormal3d( -7, 0, 0 );
		glVertex3d( -4.1, -0.075, -4.1 );
		glVertex3d( -4.1, -0.075,  4.1 );
		glVertex3d( -4.1, 0,  4.1 );
		glVertex3d( -4.1, 0, -4.1 );

		// Now draw the outer border in light gray.
		glColor4d( 0.45, 0.45, 0.45, 0.95 );
		glNormal3d( 0, 7, 0 );
		// White side.
		glVertex3d( -4.2, -0.075,  4.2 );
		glVertex3d(  4.2, -0.075,  4.2 );
		glVertex3d(  4.1, -0.075,  4.1 );
		glVertex3d( -4.1, -0.075,  4.1 );

		// Right of white side.
		glVertex3d( -4.2, -0.075, -4.2 );
		glVertex3d( -4.2, -0.075,  4.2 );
		glVertex3d( -4.1, -0.075,  4.1 );
		glVertex3d( -4.1, -0.075, -4.1 );

		// Black side.
		glVertex3d(  4.2, -0.075, -4.2 );
		glVertex3d( -4.2, -0.075, -4.2 );
		glVertex3d( -4.1, -0.075, -4.1 );
		glVertex3d(  4.1, -0.075, -4.1 );

		// Left of white side.
		glVertex3d(  4.2, -0.075,  4.2 );
		glVertex3d(  4.2, -0.075, -4.2 );
		glVertex3d(  4.1, -0.075, -4.1 );
		glVertex3d(  4.1, -0.075,  4.1 );

		// Drawing sides of the darker portion.
		glColor3d( 0.45, 0.45, 0.45 );
		glNormal3d( 0, 0, 7);
		glVertex3d( -4.2, -0.15, 4.2 );
		glVertex3d(  4.2, -0.15, 4.2 );
		glVertex3d(  4.2, -0.075, 4.2 );
		glVertex3d( -4.2, -0.075, 4.2 );

		glNormal3d( 7, 0, 0 );
		glVertex3d(  4.2, -0.15,  4.2 );
		glVertex3d(  4.2, -0.15, -4.2 );
		glVertex3d(  4.2, -0.075, -4.2 );
		glVertex3d(  4.2, -0.075,  4.2 );
		
		glNormal3d( 0, 0, -7 );
		glVertex3d(  4.2, -0.15, -4.2 );
		glVertex3d( -4.2, -0.15, -4.2 );
		glVertex3d( -4.2, -0.075, -4.2 );
		glVertex3d(  4.2, -0.075, -4.2 );

		glNormal3d( -7, 0, 0 );
		glVertex3d( -4.2, -0.15, -4.2);
		glVertex3d( -4.2, -0.15,  4.2 );
		glVertex3d( -4.2, -0.075,  4.2 );
		glVertex3d( -4.2, -0.075, -4.2 );
	glEnd();
	glDisable(GL_BLEND);
	glPopMatrix();
}

void GraniteTheme::hoverPosition(const BoardPosition& bp)
{
	if(m_lasthoverpos.isValid()) {
		m_overlaytimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setType(Timer::LOGARITHMIC);
		m_overlaytimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setDuration(0.25);
		m_overlaytimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setRange(0.7, 0);
		m_overlaytimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.start();
	}
	if(bp.isValid()) {
		m_overlayalpha[bp.x()][bp.y()] = 0.7;
		m_overlaytimer[bp.x()][bp.y()].stop();
	}
	m_lasthoverpos = bp;
}
 
bool GraniteTheme::load()
{
	if(m_blacktexture.load("../art/marblehugeblack.png")) {
		// add debugging
	} else if(m_blacktexture.load(ART_DIR + string("marblehugeblack.png"))) {
		// add debugging
	} else {
		cerr << "Failed to load marblehugeblack.png texture" << endl;
		return false;
	}

	if(m_whitetexture.load("../art/marblehugewhite.png")) {
		// add debugging
	} else if(m_whitetexture.load(ART_DIR + string("marblehugewhite.png"))) {
		// add debugging
	} else {
		cerr << "Failed to load marblehugewhite.png texture" << endl;
		return false;
	}

	return true;
}

bool GraniteTheme::loadGL()
{
	m_blacktexture.loadGL();
	m_whitetexture.loadGL();
	return true;
}

void GraniteTheme::unloadGL()
{
	m_blacktexture.unloadGL();
	m_whitetexture.unloadGL();
}

// End of file granitetheme.cpp
