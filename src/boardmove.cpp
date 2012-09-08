/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardmove.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "boardmove.h"

#include <map>

using std::map;

map<Piece::Type, BoardMove::fp_isLegalFunc> BoardMove::m_isMoveLegalForPiece;

// This is an awesome hack.  BoardMove contains one static member variable of
// this class, so that when it gets constructed, it initializes the map.  This
// means we don't need any special logic to check if the map has been initialized
// already.
BoardMove::LegalMapInitializer BoardMove::m_mapinitializer;
BoardMove::LegalMapInitializer::LegalMapInitializer()
{
	BoardMove::m_isMoveLegalForPiece[Piece::PAWN] = &BoardMove::isLegalPawnMove;
	BoardMove::m_isMoveLegalForPiece[Piece::ROOK] = &BoardMove::isLegalRookMove;
	BoardMove::m_isMoveLegalForPiece[Piece::KNIGHT] = &BoardMove::isLegalKnightMove;
	BoardMove::m_isMoveLegalForPiece[Piece::BISHOP] = &BoardMove::isLegalBishopMove;
	BoardMove::m_isMoveLegalForPiece[Piece::QUEEN] = &BoardMove::isLegalQueenMove;
	BoardMove::m_isMoveLegalForPiece[Piece::KING] = &BoardMove::isLegalKingMove;
}

// Returns true if this move put a pawn into a promotable boardposition.
bool BoardMove::needPromotion() const
{
	if (m_moved->type() != Piece::PAWN) {
		return false;
	}
	if (m_dest.rank() != 1 && m_dest.rank() != 8) {
		return false;
	}
	if (m_promote != Piece::NOTYPE) {
		return false;
	}
	if (!isLegal()) {
		return false;
	}
	return true;
}

// Returns true if this move is legal for its piece type.
bool BoardMove::isLegal() const
{
	if (!m_moved) {
		return false;
	}

	map<Piece::Type, fp_isLegalFunc>::iterator mli = m_isMoveLegalForPiece.find(m_moved->type());
	return (this->*(mli->second))();
}

// Returns true if the boardmove is a legal Pawn move. 
bool BoardMove::isLegalPawnMove() const
{
	// Clearly a wrong move
	if(rankDiff() > 2 || fileDiff() > 1) {
		return false;
	}

	if(m_moved->color() == Piece::WHITE) {
		// White can only move up ranks
		if(signedRankDiff() < 1) {
			return false;
		}

		// Check if this is the first move
		if(rankDiff() == 2) {
			if(m_origin.rank() == 2) {
				return true;
			} else {
				return false;
			}
		}

	} else {
		// Black can only move down ranks
		if(signedRankDiff() > -1) {
			return false;
		}

		// Check if this is the first move
		if(rankDiff() == 2) {
			if(m_origin.rank() == 7) {
				return true;
			} else {
				return false;
			}
		}
	}

	// Board will check en passant and capture possibilities.
	// As Far as we know, this is a legitimate move.
	return true;
}

// Returns true if the move is a legal Rook move.
bool BoardMove::isLegalRookMove() const
{
	if (rankDiff() != 0 && fileDiff() != 0) {
		return false;
	}
	return true;
}

// Returns true if the move is a legal Knight move.
bool BoardMove::isLegalKnightMove() const
{
	if (rankDiff() != 1 && rankDiff() != 2) {
		return false;
	} else if (rankDiff() == 1 && fileDiff() != 2) {
		return false;
	} else if (rankDiff() == 2 && fileDiff() != 1) {
		return false;
	}
	return true;
}

// Returns true if the move is a legal Bishop move.
bool BoardMove::isLegalBishopMove() const
{
	if (rankDiff() != fileDiff()) {
		return false;
	}
	return true;
}

// Returns true if the move is a legal Queen move.
bool BoardMove::isLegalQueenMove() const
{
	if ((rankDiff() != fileDiff()) && (rankDiff() && fileDiff())) {
		return false;
	}
	return true;
}

// Returns true if the move is a legal King move.
bool BoardMove::isLegalKingMove() const
{
	if (rankDiff() > 1) {
		return false;
	}
	if (fileDiff() > 2) {
		return false;
	}
	if (fileDiff() == 2 && rankDiff()) {
		return false;
	}
	return true;
}

// End of file boardmove.cpp

