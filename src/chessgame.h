/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File: chessgame.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "board.h"
#include "boardmove.h"
#include "chessplayer.h"
#include "chessgamestate.h"

/**
 * This class represents a Chess Game
 */
class ChessGame {
 public:
	
	/** Default constructor. Initializes both players to zero. */ 
	ChessGame() :
		m_player1(0),
		m_player2(0) {}
 
	/** Constructor that initializes both ChessPlayers. */
	ChessGame(ChessPlayer * p1, ChessPlayer * p2) :
		m_player1(p1),
		m_player2(p2) 
	{
		m_player1->setIsWhite(true);
		m_player2->setIsWhite(false);
	}

	/**
	 * A ChessGame is responsible for deleting both of its players.
	 */
	~ChessGame();
	
	/** Resets the board, history, and statistics for a new game. */
	void newGame();
			
	/** Loads a previously saved game */
	void loadGame();
	
	/** Starts a new game of chess. */
	void startGame();

    ChessPlayer* getPlayer1() const
        { return m_player1; }

    ChessPlayer* getPlayer2() const
        { return m_player2; }

	/** Sets the ChessPlayer to use for player 1. */
	void setPlayer1(ChessPlayer * p1)
 		{ delete m_player1; m_player1 = p1; }
			
	/** Sets the ChessPlayer to use for player 2. */
	void setPlayer2(ChessPlayer * p2)
		{ delete m_player2; m_player2 = p2; }
			
	/** Retrieves the player whose turn it is. */
	ChessPlayer * getCurrentPlayer() const
		{ return (m_state.isWhiteTurn()) ? m_player1 : m_player2; }

	/** Retrieves the player who is inactive */
	ChessPlayer * getInactivePlayer() const
		{ return (m_state.isWhiteTurn()) ? m_player2 : m_player1; }
	
	/** Attempts to apply bm, return true if applied */
	bool tryMove(const BoardMove & bm);
	
	/** Returns the current state of the chess game */
	ChessGameState getState() const 
		{ return m_state; }

	/** Accessor function for getting the board */
	inline Board getBoard() const 
		{ return m_state.getBoard(); }

	/** Accessor function for getting the turn */
	inline Piece::Color getTurn() const
		{ return m_state.getTurn(); }

	/** Undo the last move by popping it off the history stack */
	void undoMove();

						
 private:
	ChessPlayer * m_player1;
	ChessPlayer * m_player2;
	ChessGameState m_state;

	stack<ChessGameState> m_history_stack;
	stack<ChessGameState> m_redo_stack;
 
 	bool m_is_game_in_progress;
};

#endif // CHESSGAME_H

// End of file chessgame.h
