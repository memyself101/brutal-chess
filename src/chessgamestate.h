/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessgamestate.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef CHESSGAMESTATE_H
#define CHESSGAMESTATE_H

#include <map>
#include <stack>
#include <vector>

#include "board.h"

class Piece;

class ChessGameState {

  public:

	/** Default constructor for ChessGameState */
	ChessGameState()
		{ reset(); }

	/** Reset to the beginning ChessGameState */
	void reset();

	/** 
	 * Update ChessGameState to reflect current state of the game
	 * @param - bm - The legal BoardMove for updating the game.
	 */
	void update(const BoardMove& bm);

	/** Returns true if the current player is in check */
	bool isCheck() const
		{ return m_check; }

	/** Returns true if the game is a draw*/
	bool isDraw();

	/** Returns true if it is white's turn, false otherwise */
	bool isWhiteTurn() const
		{ return m_white_turn; }

	/**
	 * Return true if there is a piece at the board position and
	 * can be selected by the current player, false otherwise.
	 * @param - The BoardPosition to check selectibility for.
	 */
	bool isPositionSelectable(const BoardPosition& bp) const;

	/** Get the color of the current player */
	Piece::Color getTurn() const 
		{ return m_white_turn ? Piece::WHITE : Piece::BLACK; }

	/** Returns the current game board */
	Board getBoard() const 
		{ return m_board; }

	/** Returns the last move made */
	BoardMove getLastMove() const
		{ return m_last_move; }

	/** Returns the number of turns played so far */
	int getTurnNumber() const
		{ return m_turn_number; }
	
    friend class ChessGame;

  private:

    std::vector<std::pair<SerialBoard, int> > m_threefold_count;
	Piece* m_pieces[Board::BOARDSIZE*Board::BOARDSIZE];
	Board m_board;
	BoardMove m_last_move;
	bool m_white_turn, m_check, m_threefold;
	int m_turn_number, m_50_moves;
};

#endif

// end of file chessgamestate.h
