/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : board.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
 
#ifndef BOARD_H
#define BOARD_H

#include <cstdio>
#include <iostream>
#include <vector>

#include "bitboard.h"
#include "boardmove.h"

using std::vector;

struct SerialBoard {
	unsigned long long pieces[Piece::LAST_TYPE+1];
	unsigned long long color[Piece::LAST_COLOR+1];
	union {
		int all_flags;
		struct {
			int white_enpassant:8;
			int black_enpassant:8;
			int white_turn:1;
			int wk_castling:1;
			int wq_castling:1;
			int bk_castling:1;
			int bq_castling:1;
			int:11;
		};
	};
};

/**
 * This class represents a chess board.
 */
class Board {
 public:
	/** Default constructor.  Creates an empty board. */
	Board();

	/** Board destructor. Deletes all the board stuff */
	~Board();

	/** Performs all the static initializations */
	static void init();

	/** Reset the board to an empty state. */
	void reset();

	void setupPieces();
	
	/** Returns the piece at the BoardPosition 'bp'. */
	Piece* getPiece(const BoardPosition & bp) const;
	
	/**
	 * Returns the bit board with the positions of pieces of type t
	 * @param t - The type of pieces to return
	 */
	BitBoard getPieces(Piece::Type t) const 
		{ return BitBoard(m_pieces[t]); }

	/**
	 * Returns the bit board with the positions of pieces of type t and color c
	 * @param c - The color of pieces to return
	 * @param t - The type of pieces to return
	 */
	BitBoard getPieces(Piece::Color c, Piece::Type t) const
		{return BitBoard(m_pieces[t] & m_color[c]);}

	/**
	 * Sets the internal bitboard at BoardPosition bp to indicate a piece of
	 * type t and color c
	 * @param c - The Piece::Color to assign the piece.
	 * @param t - The Piece::Type to assign the piece.
	 * @param bp - The BoardPosition to set the piece at.
	 */
	void setPiece(Piece::Color c, Piece::Type t, const BoardPosition& bp);
	
	/* Puts a new piece onto the board. Overwrites whatever is at that
	 * BoardPosition.
	 * @param - Piece to be created
	 * @param - BoardPosition where piece is being created
	 */
	void setPiece(Piece * piece, const BoardPosition & bp);

	/**
	 * Remove the piece from given BoardPosition
	 * @param bp - BoardPosition to remove piece from
	 */
	void removePiece(const BoardPosition & bp);

	/**
	 * Returns true if there are no pieces between start and end exclusive.
	 * @param start - The starting BoardPosition.
	 * @param end - The ending BoardPosition.
	 */
	bool isPathClear(const BoardPosition & start, const BoardPosition & end) const;

	/**
	 * Returns true if there are no pieces between start and end of the BoardMove,
	 * exclusive.
	 * @param bm - The BoardMove to check path for.
	 */
	bool isPathClear(const BoardMove & bm) const
		{ return isPathClear(bm.origin(), bm.dest()); }	

	/**
	 * Returns true if a piece is present at the specified BoardPosition.
	 * @param pos - The BoardPosition to check.
	 */
	bool isOccupied(const BoardPosition & pos) const;	

	/**
	 * Returns true if a move is legal for the piece at its origin for the
	 * current board setup.
	 * @param bm - The BoardMove to be validated.
	 */
	bool isMoveLegal(const BoardMove & bm) const;

	/**
	 * Returns true if the given position is under attack by the opposing color,
	 * returns false otherwise.
	 * @param bp - The BoardPosition to check.
	 * @param c - The Color that could be under attack
	 */
	unsigned long long isAttacked(const BoardPosition & bp, Piece::Color c) const;

	/**
	 * Returns true if the players attempted move leaves him in check, returns
	 * false otherwies.
	 * @param bm - The BoardMove being attempted.
	 */
	bool isResultCheck(const BoardMove & bm) const;

	/**
	 * Returns true if the players is in check, false otherwies.
	 * @param c - The color that could be in check
	 */
	bool isCheck(Piece::Color c) const
		{ return (0 != this->isAttacked(m_king_pos[c], c)); }
	
	/**
	 * Returns true if the player is in checkmate, false otherwies.
	 * @param c - The color that could be in checkmate
	 */
	bool isCheckMate(Piece::Color c) const;

	/**
	 * Returns true if either player is in Check Mate
	 */
	bool containsCheckMate() const
		{ return (isCheckMate(Piece::WHITE) || isCheckMate(Piece::BLACK)); }

	/**
	 * Returns true if the players is in stalemate, false otherwies.
	 * @param c - The color that could be in stalemate
	 */
	bool isStaleMate(Piece::Color c) const;

	bool isMaterialDraw() const;

	/**
	 * Returns true if a Pawn can move to the specified BoardPosition to
	 * perform an en-passant move.
	 * @param bp - The BoardPosition at which to check the en passant bit.
	 */
	bool isEnPassantSet(const BoardPosition & bp) const
		{ return (0 != (getMask(bp) & m_enpassant_flags)); }

	vector<BoardMove> possibleMoves(Piece::Color color, bool findOne=false) const;

	/** */
	BoardPosition getKing(Piece::Color c) const
		{ return m_king_pos[c]; }
	
	/** */
	int getRankState(const BoardPosition & bp) const;

	/** */
	int getFileState(const BoardPosition & bp) const;

	/** */
	int getSEDiagState(const BoardPosition & bp) const;

	/** */
	int getNEDiagState(const BoardPosition & bp) const;

	/**
	 * This function handles the setting and removal of castling and en
	 * passant flags on the specialty flag boards.
	 * @param bm - The BoardMove that just occured.
	 */
	void setSpecialPieceFlags(const BoardMove & bm);
	
	/**
	 * Given a valid and legal move, updates the board to reflect the move.
	 * This function should only be called from ChessGame::tryMove.
	 * @param bm - The move to update the board with.
	 */
	void update(const BoardMove & bm);

	/** */
	void addPiece(Piece * p, const BoardPosition & bp);

	/** */
	SerialBoard serialize() const;
	
	/**
	 * This is just the size of the board, useful for looping over a board.
	 */
	const static int BOARDSIZE = 8;

	static unsigned long long pawnAttacks[2][64];
	static unsigned long long knightAttacks[64];
	static unsigned long long kingAttacks[64];
	static unsigned long long rankAttacks[64][256];
	static unsigned long long fileAttacks[64][256];
	static unsigned long long diagAttacksSE[64][256];
	static unsigned long long diagAttacksNE[64][256];

	friend class BrutalPlayer;

 private:
	static Piece* m_allpieces[Piece::LAST_COLOR + 1][Piece::LAST_TYPE + 1];
	static bool m_setup;

	int m_total_pieces[Piece::LAST_COLOR + 1];
	int m_piece_count[Piece::LAST_COLOR + 1][Piece::LAST_TYPE + 1];

	// Current state of the board
	unsigned long long m_pieces[Piece::LAST_TYPE + 1];
	unsigned long long m_color[Piece::LAST_COLOR + 1];
	unsigned long long m_enpassant_flags;
	unsigned long long m_castling_flags;

	// Nice to have this around
	BoardPosition m_king_pos[Piece::LAST_COLOR + 1];

	inline void unsetAllBits(const BoardPosition & bp);
};

static int pow2[8];

std::ostream& operator<< (std::ostream & os, const Board & b);

inline unsigned long long getMask(const BoardPosition & bp)
{
	return 1LL << (Board::BOARDSIZE*(bp.m_rank0) + bp.m_file0);
}

inline unsigned long long maskRank(const BoardPosition & bp)
{
	return (0xffLL << bp.rank0()*8);
}

inline unsigned long long maskFile(const BoardPosition & bp)
{
	unsigned long long mask = 0LL;
	
	for (int i = 0; i < 8; ++i)
		mask |= (1LL << (bp.file0() + 8*i));
	
	return mask;
}

inline void setBit(unsigned long long & bitfield, const BoardPosition & bp)
{
	bitfield |= getMask(bp);		
}

inline void unsetBit(unsigned long long & bitfield, const BoardPosition & bp)
{
	bitfield ^= getMask(bp);		
}

#endif
 
// End of file board.h
