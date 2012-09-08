/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : xboardplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
#ifndef WIN32

#include "board.h"
#include "chessgamestate.h"
#include "chessplayer.h"

#include <cassert>
#include <sstream>
#include <string>

using namespace std;


XboardPlayer::XboardPlayer()
{
	m_trustworthy = true;
}

XboardPlayer::~XboardPlayer()
{
	write(m_to[1], "quit\n", 5);
}

void XboardPlayer::newGame()
{
	runChessEngine();
}

void XboardPlayer::loadGame(const ChessGameState& cgs)
{
}

void XboardPlayer::startGame()
{
	if(m_is_white)
		write(m_to[1], "go\n", 3);
}

// Set up gnuchess
void XboardPlayer::runChessEngine()
{
	pipe( m_to );
	pipe( m_from );

	if (fork() == 0) {
		// Child Process
		nice( 20 );
		dup2( m_to[0], 0 );
		dup2( m_from[1], 1 );
		close( m_to[0] );
		close( m_to[1] );
		close( m_from[0] );
		execvp( "gnuchess", NULL );
		cerr << "Couldn't run gnuchess" << endl;
	}
	write( m_to[1], "xboard\n", 7);
	m_initialized = true;
}

// Get a move from GnuChess
void XboardPlayer::think(const ChessGameState & cgs)
{
	if (!m_initialized)
		runChessEngine();
	
	string output;
	char c;
	while ( output.substr(0, 11) != "My move is:") {
		output = "";
		while ( read( m_from[0], &c, 1 ) ) {
			if ( c == '\n' )
				break;
			output += c;
		}
		cout << output << endl;
	}
	output = output.substr(12, 5);

	cout << output << endl;
	
	// Construct a BoardMove from the move string.
	stringstream oss(output);
	int rank;
		
	oss >> c;
	oss >> rank;
	BoardPosition origin(c, rank);
	
	oss >> c;
	oss >> rank;
	BoardPosition dest(c, rank);
	
	Board b = cgs.getBoard();
	BoardMove move(origin, dest, b.getPiece(origin));
	m_move = move;
}

// Send your move to GnuChess
void XboardPlayer::opponentMove(const BoardMove & move, const ChessGameState & cgs)
{
	if (!m_initialized)
		runChessEngine();

	Board board = cgs.getBoard();
	
	Piece * piece = board.getPiece(move.dest());

	int characters = 5;
	string movestr = "";
	movestr += move.origin().filec();
	movestr += '0' + move.origin().rank();
	movestr += move.dest().filec();
	movestr += '0' + move.dest().rank();
	if(move.getPromotion() != Piece::NOTYPE) {
		movestr += 'q';
		characters++;
	}
	movestr += '\n';

	cout << movestr << endl;

	write( m_to[1], movestr.c_str(), characters);
}

void XboardPlayer::undoMove()
{
	string undo = "undo\n";
	write(m_to[1], undo.c_str(), 5);
}

#endif

// end of file xboardplayer.cpp

