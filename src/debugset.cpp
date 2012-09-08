/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : debugset.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "pieceset.h"
#include "SDL_opengl.h"

bool DebugSet::load()
{
	// Make the base of the piece its center
	for(int i = 0; i <= Piece::LAST_TYPE; i++)
		m_pieces[i].setRecenter(true, false, true);
	
	// Load the piece models
	if(!m_pieces[Piece::PAWN].load("../models/debug.obj"))
		return false;
	if(!m_pieces[Piece::ROOK].load("../models/debug.obj"))
		return false;
	if(!m_pieces[Piece::KNIGHT].load("../models/debug.obj"))
		return false;
	if(!m_pieces[Piece::BISHOP].load("../models/debug.obj"))
		return false;
	if(!m_pieces[Piece::QUEEN].load("../models/debug.obj"))
		return false;
	if(!m_pieces[Piece::KING].load("../models/debug.obj"))
		return false;

	// Create smooth normals (Phong Shading)
	for(int i = 0; i <= Piece::LAST_TYPE; i++)
		m_pieces[i].findNorms();

	// Scale the pieces to the proper size
	m_pieces[0].setScale(m_pieces[0].scale()*7);
	for(int i = 1; i <= Piece::LAST_TYPE; i++)
		m_pieces[i].setScale(m_pieces[0].scale());
	
	return true;
}

// end of file debugset.cpp
