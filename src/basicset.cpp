/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : basicpieceset.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "config.h"
#include "pieceset.h"
#include "SDL_opengl.h"

BasicSet::BasicSet()
{
	for(int i = 0; i < Board::BOARDSIZE; i++)
		for(int j = 0; j < Board::BOARDSIZE; j++)
			m_hoverheight[i][j] = 0.01;
}

bool BasicSet::load()
{
	// Make the base of the piece its center
	for(int i = 0; i <= Piece::LAST_TYPE; i++) {
		m_pieces[i].setRecenter(true, false, true);
#ifdef TEXTUREPIECES
		m_pieces[i].enableTexture();
#endif
	}

#ifdef TEXTUREPIECES
	m_textures[Piece::WHITE][Piece::PAWN].load("../art/pawnw.png");
	m_textures[Piece::WHITE][Piece::ROOK].load("../art/rookw.png");
	m_textures[Piece::WHITE][Piece::KNIGHT].load("../art/knightw.png");
	m_textures[Piece::WHITE][Piece::BISHOP].load("../art/bishopw.png");
	m_textures[Piece::WHITE][Piece::QUEEN].load("../art/queenw.png");
	m_textures[Piece::WHITE][Piece::KING].load("../art/kingw.png");

	m_textures[Piece::BLACK][Piece::PAWN].load("../art/pawnb.png");
	m_textures[Piece::BLACK][Piece::ROOK].load("../art/rookb.png");
	m_textures[Piece::BLACK][Piece::KNIGHT].load("../art/knightb.png");
	m_textures[Piece::BLACK][Piece::BISHOP].load("../art/bishopb.png");
	m_textures[Piece::BLACK][Piece::QUEEN].load("../art/queenb.png");
	m_textures[Piece::BLACK][Piece::KING].load("../art/kingb.png");
#endif
	// Load the piece models 
	if(m_pieces[Piece::PAWN].load("../models/pawn.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::PAWN].load(MODELS_DIR + string("pawn.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	if(m_pieces[Piece::ROOK].load("../models/rook.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::ROOK].load(MODELS_DIR + string("rook.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	if(m_pieces[Piece::KNIGHT].load("../models/knight.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::KNIGHT].load(MODELS_DIR + string("knight.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	if(m_pieces[Piece::BISHOP].load("../models/bishop.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::BISHOP].load(MODELS_DIR + string("bishop.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	if(m_pieces[Piece::QUEEN].load("../models/queen.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::QUEEN].load(MODELS_DIR + string("queen.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	if(m_pieces[Piece::KING].load("../models/king.obj")) {
		// Add debugging
	} else if(m_pieces[Piece::KING].load(MODELS_DIR + string("king.obj"))) {
		// Add debugging
	} else {
		return false;
	}

	// Create smooth normals (Phong Shading)
	for(int i = 0; i <= Piece::LAST_TYPE; i++)
		m_pieces[i].findNorms();

	// Scale the pieces to the proper size
	m_pieces[0].setScale(m_pieces[0].scale()*7);
	for(int i = 1; i <= Piece::LAST_TYPE; i++)
		m_pieces[i].setScale(m_pieces[0].scale());
	
	return true;
}

bool BasicSet::unload()
{
	unloadGL();
	return true;
}

bool BasicSet::loadGL()
{
	// Create display lists for the pieces
	for(int i = 0; i <= Piece::LAST_TYPE; i++) {
		m_pieces[i].build();
	}

#ifdef TEXTUREPIECES
	for(int j = 0; j <= Piece::LAST_COLOR; j++) {
		for(int i = 0; i <= Piece::LAST_TYPE; i++) {
			m_textures[j][i].loadGL();
		}
	}
#endif
	return true;
}

bool BasicSet::unloadGL()
{
	// Delete display lists for the pieces
	for(int i = 0; i <= Piece::LAST_TYPE; i++) {
		m_pieces[i].unbuild();
	}

	for(int j = 0; j <= Piece::LAST_COLOR; j++) {
		for(int i = 0; i <= Piece::LAST_TYPE; i++) {
			m_textures[j][i].unloadGL();
		}
	}
	return true;
}


void BasicSet::draw(const ChessGameState& cgs)
{
	Board b = cgs.getBoard();
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// Move to center of bottom left square
	glPushMatrix();
	glTranslated(0.5,0,-0.5);

	for(BoardPosition bp('a', 1); !bp.outN(); bp.moveN())
	{
		for(bp.setFile('a'); !bp.outE(); bp.moveE())
		{
			if(m_hovertimer[bp.x()][bp.y()].started()) {
				m_hovertimer[bp.x()][bp.y()]++;
				m_hoverheight[bp.x()][bp.y()] += 
					m_hovertimer[bp.x()][bp.y()].change();
			}
			if(m_hovertimer[bp.x()][bp.y()].done()) {
				m_hoverheight[bp.x()][bp.y()] = 0.01;
				m_hovertimer[bp.x()][bp.y()].resetDone();
			}

			double offx = 0.0, offy = 0.0;
			if(m_movetimer[bp.x()][bp.y()].first.started()) {
				cout << bp.x() << " " << bp.y() << endl;
				m_movetimer[bp.x()][bp.y()].first++;
				m_movetimer[bp.x()][bp.y()].second++;
				offx = m_movetimer[bp.x()][bp.y()].first.value();
				offy = m_movetimer[bp.x()][bp.y()].second.value();
			}
			
	
			if(b.isOccupied(bp))
			{
				Piece* p = b.getPiece(bp);
				bool alert = false;
				if(cgs.isCheck() && p->type() == Piece::KING && p->color() == cgs.getTurn())
					alert = true;
				
				glTranslated(offx, m_hoverheight[bp.x()][bp.y()], -offy);
				drawPiece(p, 1.0, alert);
				glTranslated(-offx, -m_hoverheight[bp.x()][bp.y()], offy);
			}

			glTranslated(1,0,0);
		}

		// Move to the next rank
		glTranslated(-8,0,-1);
	}

	// Draw the pieces for pawn promotion selection
	if (m_drawPromotionSelector) {
		if (cgs.isWhiteTurn()) {
			// We're waiting for the white player to select which
			// piece to promote to, so draw the pieces at the black
			// end of the board.
			glTranslated(2,0,0);
			static Piece * q = new Piece(Piece::WHITE, Piece::QUEEN);
			drawPiece(q, 1.0, false);
			glTranslated(1,0,0);
			static Piece * k = new Piece(Piece::WHITE, Piece::KNIGHT);
			drawPiece(k, 1.0, false);
			glTranslated(1,0,0);
			static Piece * b = new Piece(Piece::WHITE, Piece::BISHOP);
			drawPiece(b, 1.0, false);
			glTranslated(1,0,0);
			static Piece * r = new Piece(Piece::WHITE, Piece::ROOK);
			drawPiece(r, 1.0, false);
			glTranslated(-5,0,0);
		}
		else {
			// We're waiting for the black player to select which
			// piece to promote to, so draw the pieces at the white
			// end of the board.
			glTranslated(5,0,9);
			static Piece * q = new Piece(Piece::BLACK, Piece::QUEEN);
			drawPiece(q, 1.0, false);
			glTranslated(-1,0,0);
			static Piece * k = new Piece(Piece::BLACK, Piece::KNIGHT);
			drawPiece(k, 1.0, false);
			glTranslated(-1,0,0);
			static Piece * b = new Piece(Piece::BLACK, Piece::BISHOP);
			drawPiece(b, 1.0, false);
			glTranslated(-1,0,0);
			static Piece * r = new Piece(Piece::BLACK, Piece::ROOK);
			drawPiece(r, 1.0, false);
			glTranslated(-2,0,-9);
		}
	}
	
    glPopMatrix();

	if(m_selected.isValid() && !m_drawPromotionSelector) {
		Piece* selected = b.getPiece(m_selected);
		bool alert = false;
		if(cgs.isCheck() && selected->type() == Piece::KING && selected->color() == cgs.getTurn())
			alert = true;

		glTranslated(m_mouseX, 0.2, m_mouseY);	
		drawPiece(b.getPiece(m_selected), 0.5, alert);
		glTranslated(-m_mouseX, -0.2, -m_mouseY);	
	}

}

void BasicSet::drawPiece(Piece* p, double alpha, bool alert)
{
	double rotation = 0;
	if(p->type() == Piece::KNIGHT || p->type() == Piece::BISHOP)
		p->color() == Piece::WHITE ? rotation = -90.0 : rotation = 90.0;

	double color = 0.7;
	if(p->color() == Piece::BLACK)
		color = 0.15;

	if(alpha != 1.0)
		glEnable(GL_BLEND);

#ifdef TEXTUREPIECES
	color = 0.8;
#endif
	glScalef(1/7.0, 1/7.0, 1/7.0);
	glRotated(rotation, 0.0, 1.0, 0.0);
	glColor4f(color, color, color, alpha);

	if(alert)
		glColor4f(0.7, 0.15, 0.15, alpha);
	
#ifdef TEXTUREPIECES
	glEnable(GL_TEXTURE_2D);
	m_textures[p->color()][p->type()].use();
#endif
	m_pieces[p->type()].draw();
#ifdef TEXTUREPIECES
	glDisable(GL_TEXTURE_2D);
#endif

	glRotated(-rotation, 0.0, 1.0, 0.0);
	glScalef(7.0, 7.0, 7.0);
	
	if(alpha != 1.0)
		glDisable(GL_BLEND);
}

void BasicSet::hoverPosition(const ChessGameState & gs, const BoardPosition& bp)
{
	if(!m_selected.isValid()) {
		if(m_lasthoverpos.isValid()) {
			m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setType(Timer::LOGARITHMIC);
			m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setDuration(0.5);
			m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.setRange(0.2, 0.01);
			m_hovertimer[m_lasthoverpos.x()][m_lasthoverpos.y()]
				.start();
			m_lasthoverpos.invalidate();
		}
		if(gs.isPositionSelectable(bp)) {
			m_hoverheight[bp.x()][bp.y()] = 0.2;
			m_hovertimer[bp.x()][bp.y()].stop();
			m_lasthoverpos = bp;
		}
	}
	
}

void BasicSet::animateMove(const BoardMove & bm)
{
	Timer t;
	t.setRange(0, bm.signedFileDiff());
	t.setDuration(1.0);
	m_movetimer[bm.origin().x()][bm.origin().y()].first = t;
	m_movetimer[bm.origin().x()][bm.origin().y()].first.start();
	t.setRange(0, bm.signedRankDiff());
	t.setDuration(1.0);
	m_movetimer[bm.origin().x()][bm.origin().y()].second = t;
	m_movetimer[bm.origin().x()][bm.origin().y()].second.start();
}

// end of file basicpieceset.cpp
