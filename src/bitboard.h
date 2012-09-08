/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : bitboard.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
 
#ifndef BITBOARD_H
#define BITBOARD_H

#include "boardposition.h"
#include <iosfwd>

/**
 * This class represents a bit board.
 */
class BitBoard {
 public:
	/** Default constructor.  Creates an empty board. */
	BitBoard() : m_board(0LL) {}

	/** Constructs a BitBoard from a unsigned long long (a 64-bit field). */
	BitBoard(const unsigned long long & board) : m_board(board) {}

	/** Turns on the bit at 'bp' */
	void setBit(const BoardPosition & bp);
	
	/** Turns off the bit at 'bp' */
	void unsetBit(const BoardPosition & bp);

	/** Flips all of the bits in the BitBoard */
	void invert();

	/** Returns a BitBoard with all of the bits in 'bp.rank()' turned on. */
	static unsigned long long maskRank(const BoardPosition & bp);
		
	/** Returns a BitBoard with all of the bits in 'bp.file()' turned on. */
	static unsigned long long maskFile(const BoardPosition & bp);
		
	/**
	 * Returns a BitBoard with all of the bits in the NorthEast diagonal passing
	 * through 'bp' turned on.
	 */
	static BitBoard maskDiagNE(const BoardPosition & bp);
	
	/**
	 * Returns a BitBoard with all of the bits in the NorthWest diagonal passing
	 * through 'bp' turned on.
	 */
	static BitBoard maskDiagNW(const BoardPosition & bp);
	
	friend std::ostream& operator<< (std::ostream &, const BitBoard &);
	
	unsigned long long getBoard() const
		{ return m_board; }

 private:
	unsigned long long m_board;
};

std::ostream& operator<< (std::ostream & os, const BitBoard & b);

#endif
 
// End of file bitboard.h
