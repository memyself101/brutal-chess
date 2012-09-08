/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : randomplayer.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include <vector>
#include <time.h>
#include "board.h"
#include "chessplayer.h"
#include "SDL.h"

using namespace std;

RandomPlayer::RandomPlayer()
{
	m_trustworthy = true;
	srand(time(NULL));
}

void RandomPlayer::think(const ChessGameState & cgs)
{
	vector<BoardMove> moves = cgs.getBoard().possibleMoves(getColor());
    SDL_Delay(150);
	m_move = moves[rand() % moves.size()];
}

// end of file randomplayer.h

