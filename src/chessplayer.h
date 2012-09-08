/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessplayer.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef CHESSPLAYER_H
#define CHESSPLAYER_H

#include "boardmove.h"
#include "chessgamestate.h"

#include <string>

class ChessPlayer {
 public:
	 
	ChessPlayer() : m_is_thinking(false), m_is_human(false) {}

	virtual ~ChessPlayer() {}

	virtual void newGame() {}

	virtual void startGame() {}

	virtual void loadGame(const ChessGameState& cgs) {}

	virtual void opponentMove(const BoardMove & move, const ChessGameState & cgs) {}

	virtual bool isThinking() const
		{ return m_is_thinking; }

	virtual bool isHuman() const
		{ return m_is_human; }

	virtual void think(const ChessGameState & cgs) = 0;

	virtual BoardMove getMove()
		{ return m_move; }
	
	virtual void interruptThinking()
		{ m_is_thinking = false; }		

	virtual bool needMove()
		{ return false; }

	virtual void sendMove(const BoardMove & bm) {}
		
	virtual void undoMove() {}

	void setIsWhite(bool is_white)
		{ m_is_white = is_white; }
	
	bool isWhite() const
		{ return m_is_white; }

	bool isTrustworthy() const
		{ return m_trustworthy; }

	Piece::Color getColor() const
		{ return (m_is_white ? Piece::WHITE : Piece::BLACK); }

 protected:

	bool m_is_white;
	bool m_is_thinking;
	bool m_is_human;	
	bool m_trustworthy;
	BoardMove m_move;
};

ChessPlayer * PlayerFactory(const std::string & playertype);

#define INCHESSPLAYER_H
	#include "chessplayers.h"
#undef INCHESSPLAYER_H

#endif
