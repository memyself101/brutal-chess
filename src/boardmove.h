/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardmove.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef BOARDMOVE_H
#define BOARDMOVE_H

#include <cstdlib>
#include <map>
#include <string>

#include "boardposition.h"
#include "piece.h"

/**
 * This class represents a move that a piece can make on the board.
 */
class BoardMove {

 public:

	/** Default constructor, doesn't do anything. */
	BoardMove() {}

	/** Constructs a board move that goes from 'origin' to 'dest'. */
	BoardMove(const BoardPosition& origin, const BoardPosition& dest, Piece* moved, 
	  Piece::Type promote=Piece::NOTYPE) :
		m_origin(origin), m_dest(dest), m_moved(moved), m_promote(promote) {}

	/** Returns the origin BoardPosition of the BoardMove */
	inline const BoardPosition& origin() const 
		{ return m_origin; }

	/** Returns the destination BoardPosition of the BoardMove */
	inline const BoardPosition& dest() const
		{ return m_dest; }

	inline const Piece* getPiece() const
		{ return m_moved; }

	/** Returns the promotion char of the BoardMove */
	inline const Piece::Type getPromotion() const
		{ return m_promote; }

	inline void setPromotion(const Piece::Type & promote)
		{ m_promote = promote; }
    
    /** Returns true if this move put a pawn into a promotable boardposition */
	bool needPromotion() const;

	/** Returns true if the origin and destination are on the board */
	inline const bool isValid() const
		{ return m_origin.isValid() && m_dest.isValid(); }

	/** Returns true if the move is legal for the piece being moved */
	bool isLegal() const;

	/** Invalidate the BoardMove */
	inline void invalidate()
		{ m_origin.invalidate(); m_dest.invalidate(); }

	/**
	 * Returns the unsigned rank difference of the boardposition's
	 * origin and destination.
	 */
	inline int rankDiff() const
		{ return abs(m_dest.rank() - m_origin.rank()); }

	/**
	 * Returns the unsigned file difference of the boardposition's
	 * origin and destination.
	 */
	inline int fileDiff() const
		{ return abs(m_dest.file() - m_origin.file()); }

	/**
	 * Returns the signed rank difference of the boardposition's
	 * destination from the origin (destination-origin).
	 */
	inline int signedRankDiff() const
		{ return m_dest.rank() - m_origin.rank(); }
		
	/**
	 * Returns the signed file difference of the boardposition's
	 * destination from the origin (destination-origin).
	 */
	inline int signedFileDiff() const
		{ return m_dest.file() - m_origin.file(); }

 private:

    // Sweet hack, See .cpp for explanation
    class LegalMapInitializer { public: LegalMapInitializer(); };    
    static LegalMapInitializer  m_mapinitializer;

    // Maps piece types to their respective "isLegal" functions defined below.
    typedef bool (BoardMove::*fp_isLegalFunc)(void) const;
    static std::map<Piece::Type, fp_isLegalFunc>     m_isMoveLegalForPiece;
    bool isLegalPawnMove() const;
    bool isLegalRookMove() const;
    bool isLegalKnightMove() const;
    bool isLegalBishopMove() const;
    bool isLegalQueenMove() const;
    bool isLegalKingMove() const;

	BoardPosition m_origin;
	BoardPosition m_dest;
	Piece* m_moved; 
	Piece::Type m_promote;
};

#endif // BOARDMOVE_H

// End of file boardmove.h

