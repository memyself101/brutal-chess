/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : piece.cpp
 * Authors : Mike Cook, Joe Flint
 **************************************************************************/

#include "piece.h"

// Clears the stack of StatSnapshots for the piece.
void Piece::clearStats()
{
	while (!m_stats.empty())
		m_stats.pop();
}

void Piece::promotePawn(const Type & t)
{
	if(m_type != PAWN || t == PAWN || t == KING || t == NOTYPE)
		return;
	m_type = t;
}

// End of file piece.cpp
