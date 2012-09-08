/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessgame.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "chessgame.h"
#include "piece.h"
#include "menu.h"
#include "SDL.h"

#include <iostream>
using namespace std;

ChessGame::~ChessGame()
{
	delete m_player1;
	m_player1 = 0;
	
	delete m_player2;
	m_player2 = 0;
}

void ChessGame::startGame()
{
	m_player1->startGame();
	m_player2->startGame();
	m_is_game_in_progress = true;
}

void ChessGame::newGame()
{
	// reset the state of the game
	m_state.reset();

	// indicate that the game hasn't yet bgun
	m_is_game_in_progress = false;
	m_player1->newGame();
	m_player2->newGame();

	m_history_stack = stack<ChessGameState>();
	m_redo_stack = stack<ChessGameState>();
}

void ChessGame::loadGame()
{
	// Only temporary
	ChessGameState cgs;
	m_player1->loadGame(cgs);
	m_player2->loadGame(cgs);
}

bool ChessGame::tryMove(const BoardMove & bm)
{
	if(!bm.isValid())
		return false;
	
	// Check that the move is legal
	if(!getCurrentPlayer()->isTrustworthy())
		if(!m_state.m_board.isMoveLegal(bm))
			return false;

//	cout << m_state.getTurnNumber() << ". " << bm.origin() << " " << bm.dest() << " " << bm.getPiece()->type() << endl;

	// Since the move is an okay one, update the board
	m_state.update(bm);

	// Update the stacks
	m_history_stack.push(m_state);
	m_redo_stack = stack<ChessGameState>();
	
	return true;
}

void ChessGame::undoMove()
{
	if(!m_history_stack.empty()) {
		m_state = m_history_stack.top();
		m_redo_stack.push(m_history_stack.top());
		m_history_stack.pop();
	}
	m_player1->undoMove();
	m_player2->undoMove();
}

// End of file chessgame.cpp
