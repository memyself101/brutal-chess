/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : q3set.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "pieceset.h"
#include "SDL_opengl.h"

bool Q3Set::load()
{
	cout << "Q3Set::load()" << endl;
	m_pieces[Piece::KING].load("../art/md3/cloud/");
	m_pieces[Piece::QUEEN].load("../art/md3/yum/");
	m_pieces[Piece::KNIGHT].load("../art/md3/dragon/");
	m_pieces[Piece::PAWN].load("../art/md3/grunt/");
	m_pieces[Piece::ROOK].load("../art/md3/sonic/");
	m_pieces[Piece::BISHOP].load("../art/md3/sarge/");

	return true;
}

bool Q3Set::loadGL()
{
	m_pieces[Piece::KING].loadGL();
	m_pieces[Piece::QUEEN].loadGL();
	m_pieces[Piece::KNIGHT].loadGL();
	m_pieces[Piece::PAWN].loadGL();
	m_pieces[Piece::ROOK].loadGL();
	m_pieces[Piece::BISHOP].loadGL();
		
	for(int i = 0; i <= Piece::LAST_TYPE; i++) {
		Piece::Type p = Piece::Type(i);
		m_upIdle[p] = m_pieces[p].getUpperAnimation("TORSO_STAND1");
		m_lowIdle[p] = m_pieces[p].getLowerAnimation("LEGS_IDLE");
	}
	return true;
}

void Q3Set::draw(const ChessGameState& gs)
{
	for(int i = 0; i <= Piece::LAST_TYPE; i++) {
		Piece::Type p = Piece::Type(i);
		m_pieces[p].updateAnim(m_upIdle[p]);
		m_pieces[p].updateAnim(m_lowIdle[p]);
	}
	Board b = gs.getBoard();
	
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
			
		
			if(b.isOccupied(bp))
			{
				glTranslated(0.0, m_hoverheight[bp.x()][bp.y()], 0.0);
				drawPiece(b.getPiece(bp));
				glTranslated(0.0, -m_hoverheight[bp.x()][bp.y()], 0.0);
			}

			glTranslated(1,0,0);
		}

		// Move to the next rank
		glTranslated(-8,0,-1);
	}

	glPopMatrix();

	if(m_selected.isValid()) {
		glTranslated(m_mouseX, 0.2, m_mouseY);	
		drawPiece(b.getPiece(m_selected), 0.5);
		glTranslated(-m_mouseX, -0.2, -m_mouseY);	
	}
}

void Q3Set::drawPiece(Piece *p, double alpha)
{
	if(p->color() == Piece::WHITE)
		glRotated(180, 0.0, 1.0, 0.0);
	glColor3f(0.7, 0.7, 0.7);
	if(alpha != 1.0)
		glColor3f(0.1, 0.1, 0.1);
	glScaled(1/40.0, 1/40.0, 1/40.0);
	glEnable(GL_TEXTURE_2D);
	m_pieces[p->type()].draw(m_upIdle[p->type()], m_lowIdle[p->type()]);
	glScaled(40.0, 40.0, 40.0);
	if(p->color() == Piece::WHITE)
		glRotated(-180, 0.0, 1.0, 0.0);
}

// end of file q3set.cpp
