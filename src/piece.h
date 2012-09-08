/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : piece.h
 * Authors : Mike Cook, Joe Flint
 **************************************************************************/

#ifndef PIECE_H
#define PIECE_H

#include "statsnapshot.h"
#include <stack>

using std::stack;

/**
 * This is the base class for the representation of a chess piece.
 */
class Piece {
public:
	/** Used to distinguish the type of a piece, e.g. King, Queen, etc. */
	enum Type {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NOTYPE};
	static const Type LAST_TYPE = KING;
	/** Used to distinguish the color of a piece. */
	enum Color {BLACK, WHITE, NOCOLOR};
	static const Color LAST_COLOR = WHITE;

	static Color opposite(Color c) { return c == WHITE ? BLACK : WHITE; }

	/**
	 * Constructs a piece of Color col and Type typ
	 * @param col - The color of the piece.
	 * @param typ - The type of the piece.
	 */
	Piece(Color col, Type typ) : m_color(col), m_type(typ) {}	

	// ******** Accessors ******** 
		
	/** Returns the Color of the piece. */
	Color color() const
		{ return m_color; }
		
	/** Returns the Type of the piece. */
	Type type() const
		{ return m_type; }
		
	// ******** Utilities ********
		
	/** Clears the stack of StatSnapshots for the piece. */ 
	void clearStats();

	/** Promotes a pawn to given piece type. */
	void promotePawn(const Type& type);

	friend class Board;

protected:
	/** The piece's Type. */
	Type m_type;
	/** The pieces Color. */
	Color m_color;
	/** Keeps track of statistics */
	stack<StatSnapshot> m_stats;
};

//Piece * PieceFactory(Piece::Color color, Piece::Type type);

#endif

// End of file piece.h
