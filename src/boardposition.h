/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardposition.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef BOARDPOSITION_H
#define BOARDPOSITION_H

#include "boardposition.h"

#include <iosfwd>

/**
 * This class represents a square on a chess board.
 */
class BoardPosition {
 public:

	/**
	 * Default constructor for a BoardPosition.  The position constructed is 
	 * not valid one.
	 */
	BoardPosition() : m_file0(-1), m_rank0(-1) {}

	/**
	 * Constructs a BoardPosition from common chess notation like 'a1'.
	 * @param file - File that the piece is in.
	 * @param rank - Rank that the piece is in.
	 */
	BoardPosition(const char & file, const int & rank);
	
	/**
	 * Constructs a BoardPosition from x-y coordinates in the pieces array.
	 * @param x - The x-coordinate in the pieces array to put this piece in.
	 * @param y - The y-coordinate in the pieces array to put this piece in.
	 */
	BoardPosition(const int & x, const int & y) : m_file0(x), m_rank0(y) {}

	/**
	 * Constructs a board position from and int (0-63).  Useful for loops
	 * and whatnot.
	 * @param num - The integer for constructing a BoardPosition.
	 */
	BoardPosition(int num) : m_file0(num % 8), m_rank0(num / 8) {}

	/** Convert the board positioin into an integer */
	int hash() const { return 8*m_rank0 + m_file0; }

	/** Set the file of this BoardPosition without changing the rank */
	void setFile(const char & file) { m_file0 = file-'a'; }
	/** Set the rank of this BoardPosition without changing the file */
	void setRank(const int rank) { m_rank0 = rank - 1; }
	/** Set the x position of this BoardPosition without changing the y */
	void setX(const int x) { m_file0 = x; }
	/** Set the y position of this BoardPosition without changing the x */
	void setY(const int y) { m_rank0 = y; }

	/** Returns the file of this BoardPosition as an integer (1 indexed). */
	inline int file() const { return m_file0+1; }
	/** Returns the file of this BoardPosition as a character. */
	inline char filec() const { return 'a' + m_file0; }
	/** Returns the rank of this BoardPosition (1 indexed). */
	inline int rank() const { return m_rank0+1; }
	/** Returns the file of this BoardPosition as an integer (0 indexed). */
	inline int file0() const { return m_file0; }
	/** Returns the rank of this BoardPosition (0 indexed). */
	inline int rank0() const { return m_rank0; }

	/** Returns x position of this BoardPosition as an integer (0 indexed). */
	inline int x() const { return m_file0; }
	/** Returns y position of this BoardPosition as an integer (0 indexed). */
	inline int y() const { return m_rank0; }

	/** Returns true if the position is actually on the chess board. */
	bool isValid() const;
	/** Makes the board position invalid (Destroys all data!). */
	void invalidate() 
		{	m_rank0 = -1;
			m_file0 = -1; }

	/** Retuns the BoardPosition immediately north of this. */
	const BoardPosition N() const;
	/** Retuns the BoardPosition immediately northeast of this. */
	const BoardPosition NE() const;
	/** Retuns the BoardPosition immediately east of this. */	
	const BoardPosition E() const;
	/** Retuns the BoardPosition immediately southeast of this. */
	const BoardPosition SE() const;
	/** Retuns the BoardPosition immediately south of this. */
	const BoardPosition S() const;
	/** Retuns the BoardPosition immediately southwest of this. */
	const BoardPosition SW() const;
	/** Retuns the BoardPosition immediately west of this. */
	const BoardPosition W() const;
	/** Retuns the BoardPosition immediately northwest of this. */
	const BoardPosition NW() const;		

	const bool outN() const;
	const bool outNE() const;
	const bool outE() const;
	const bool outSE() const;
	const bool outS() const;
	const bool outSW() const;
	const bool outW() const;
	const bool outNW() const;

	/** Moves the BoardPosition to the north. No bounds checking. */
	void moveN();
	/** Moves the BoardPosition to the northeast. No bounds checking. */
	void moveNE();
	/** Moves the BoardPosition to the east. No bounds checking. */
	void moveE();
	/** Moves the BoardPosition to the southeast. No bounds checking. */
	void moveSE();
	/** Moves the BoardPosition to the south. No bounds checking. */
	void moveS();
	/** Moves the BoardPosition to the soutwest. No bounds checking. */
	void moveSW();
	/** Moves the BoardPosition to the west. No bounds checking. */
	void moveW();
	/** Moves the BoardPosition to the northwest. No bounds checking. */
	void moveNW();
	
	friend class Board;
	friend bool operator== (const BoardPosition& bp1, const BoardPosition& bp2);
	friend bool operator!= (const BoardPosition& bp1, const BoardPosition& bp2);
	friend std::ostream& operator<< (std::ostream& os, const BoardPosition& bp);
	friend unsigned long long getMask(const BoardPosition & bp);
	friend void setBit(unsigned long long&, const BoardPosition & bp);
	friend void unsetBit(unsigned long long&, const BoardPosition & bp);
	
 private:
	int m_file0;
	int m_rank0;
};

/** Returns true of bp1 and bp2 have the same rank and file */
bool operator== (const BoardPosition& bp1, const BoardPosition& bp2);

/** Returns true of bp1 and bp2 have the same rank and file */
bool operator!= (const BoardPosition& bp1, const BoardPosition& bp2);

/** Prints out bp in the form 'rank-file', as in 'a1', or 'b3' */
std::ostream& operator<< (std::ostream& os, const BoardPosition& bp);

#endif // BOARDPOSITION_H

// End of file boardposition.h
