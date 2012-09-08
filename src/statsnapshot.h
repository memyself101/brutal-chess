/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : statsnapshot.h
 * Authors : Mike Cook, Joe Flint
 **************************************************************************/
 
#ifndef STATSNAPSHOT_H
#define STATSNAPSHOT_H

#include "boardposition.h"
#include <vector>

using std::vector;

// DO NOT include piece.h, it depends on this class
class Piece;

/**
 * This class represents the statistics for a single piece.
 */
class StatSnapshot {
 public:
	/**
	 * Default constructor.  Creates empty statistics.
	 */
	StatSnapshot();

	/** Returns the number of moves the piece has made */
	inline int moveCount() const { return (int)m_movehistory.size(); }

	/** Returns a vector with all of the moves the piece has made */
	inline vector<BoardPosition> moveHistory() const { return m_movehistory; }

	/** Returns a vector with all of the pieces this piece has taken */
	inline vector<Piece*> takenPieces() const { return m_takenpieces; }
	
 private:
	vector<BoardPosition> m_movehistory;
	vector<Piece*> m_takenpieces;

};
 
#endif
 
// End of file statsnapshot.h
