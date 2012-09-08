/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : bitboard.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "board.h"
#include "bitboard.h"
#include <iostream>

void BitBoard::setBit(const BoardPosition & bp)
{
	if(bp.isValid())
		::setBit(m_board, bp);
}

void BitBoard::unsetBit(const BoardPosition & bp)
{
	::unsetBit(m_board, bp);
}

void BitBoard::invert()
{
	m_board = ~m_board;
}

unsigned long long BitBoard::maskRank(const BoardPosition & bp)
{
	return ::maskRank(bp);
}

unsigned long long BitBoard::maskFile(const BoardPosition & bp)
{
	return ::maskFile(bp);
}

BitBoard BitBoard::maskDiagNE(const BoardPosition & bp)
{
	unsigned long long mask = 0LL;
	BoardPosition p = bp;
	
	for (p; !p.outNE(); p.moveNE()) {
		mask |= getMask(p);
	}
	
	p = bp;
	for (p = p.SW(); !p.outSW(); p.moveSW()) {
		mask |= getMask(p);
	}
	
	return mask;
}

BitBoard BitBoard::maskDiagNW(const BoardPosition & bp)
{
	unsigned long long mask = 0LL;
	BoardPosition p = bp;
	
	for (p; !p.outNW(); p.moveNW()) {
		mask |= getMask(p);
	}
	
	p = bp;
	for (p = p.SE(); !p.outSE(); p.moveSE()) {
		mask |= getMask(p);
	}
	
	return mask;
}

std::ostream& operator<< (std::ostream & os, const BitBoard & bitboard)
{
	unsigned long long b = bitboard.getBoard();
	for (int rank = 8; rank > 0; --rank) {
		for (char file = 'a'; file <= 'h'; ++file) {
			bool o = b & getMask(BoardPosition(file, rank));
			os << o << " ";
		}
		os << std::endl;
	}
	return os;
}

// End of file bitboard.cpp
