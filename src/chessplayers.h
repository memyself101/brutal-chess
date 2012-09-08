/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessplayers.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifdef INCHESSPLAYER_H

#include <vector>

using std::vector;

class HumanPlayer : public ChessPlayer {
 public:
	HumanPlayer();

	/**
	 * Kills time while the Human user thinks about what move he/she
	 * wants to make.
	 */
	void think(const ChessGameState & cgs);
	
	/**
	 * Returns true because the Human user needs to input a move before
	 * play can continue;
	 */
	bool needMove() { return true; }
	
	/**
	 * Sets the specified move as this players move to make.
	 */
	void sendMove(const BoardMove & m);
};

class BrutalPlayer : public ChessPlayer {
 public:
	BrutalPlayer();
	void think(const ChessGameState & cgs);

	int getPly() { return m_ply; }
	void setPly(int ply) { m_ply = ply; }

 protected:
	int evaluateBoard(const Board & board, Piece::Color color);
	int search(Board board, Piece::Color color, int depth, int alpha, int beta, BoardMove& move);
	int pawnBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
	int knightBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
	int bishopBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
	int rookBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
	int queenBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);
	int kingBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame);

	int numAttackedSquares(const unsigned long long & pieceAttacks);

	bool isIsolatedPawn(const BoardPosition & bp, const Board & board);
	bool isDoubledPawn(const BoardPosition & bp, const Board & board);
   
	// Used to calculate positional bonuses
	static int m_bishop[64];
	static int m_knight[64];
	static int m_wpawn[64];
	static int m_bpawn[64];
	static int m_wking[64];
	static int m_bking[64];
	static int m_end_king[64];

	int m_ply;
};

class RandomPlayer : public ChessPlayer {
 public:
	RandomPlayer();
	void think(const ChessGameState & cgs);
};

class TestPlayer : public ChessPlayer {
 public:
	TestPlayer() {}
	void think(const ChessGameState & cgs);
};

#ifndef WIN32

/**
 * XboardPlayer is a ChessPlayer that can be used to interface with
 * chess engines that support the Xboard protocol.
 */
class XboardPlayer : public ChessPlayer {
 public:
	XboardPlayer();

	~XboardPlayer();

	/**
	 * Create a new game of chess using an XboardPlayer. This function 
	 * starts the gnuchess process.
	 */
	void newGame();

	/**
	 * Load a saved game ofchess into the XboardPlayer. This function
	 * starts the gnuchess process using the saved game.
	 */
	void loadGame(const ChessGameState& cgs);

	/**
	 * Notify the XboardPlayer that the game is starting
	 */
	void startGame();

	/**
	 * Gets a move from the XboardPlayer
	 */
	void think(const ChessGameState & cgs);

	/**
	 * Sends the opponents move to the XboardPlayer
	 */
	void opponentMove(const BoardMove & move, const ChessGameState & cgs);

	void undoMove();
	
 private:
	// Starts GnuChess
	void runChessEngine();
	
	int m_to[2];
	int m_from[2];
	bool m_initialized;
};

/**
 * FailePlayer is a ChessPlayer that can be used to interface with
 * the Faile chess engine. Very similar to XboardPlayer
 */
class FailePlayer : public ChessPlayer {
 public:
	FailePlayer();

	~FailePlayer();

	/**
	 * Create a new game of chess using an FailePlayer. This function 
	 * starts the gnuchess process.
	 */
	void newGame();

	/**
	 * Load a saved game ofchess into the FailePlayer. This function
	 * starts the gnuchess process using the saved game.
	 */
	void loadGame(const ChessGameState& cgs);

	/**
	 * Notify the FailePlayer that the game is starting
	 */
	void startGame();

	/**
	 * Gets a move from the FailePlayer
	 */
	void think(const ChessGameState & cgs);

	/**
	 * Sends the opponents move to the FailePlayer
	 */
	void opponentMove(const BoardMove & move, const ChessGameState & cgs);

	void undoMove();
	
 private:
	// Starts Faile
	void runChessEngine();
	
	int m_to[2];
	int m_from[2];
	bool m_initialized;
};

#endif // #ifndef WIN32

#endif

// end of file chessplayers.h
