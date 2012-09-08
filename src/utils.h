/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : utils.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include "boardtheme.h"
#include "chessplayer.h"
#include "pieceset.h"

using std::string;
using std::vector;

void Quit(int returnCode);

BoardTheme* toBoard(BoardType board);
ChessPlayer* toPlayer(PlayerType player);
PieceSet* toPieces(PiecesType pieces);

void parseCommandLine(int argc, char* argv[]);

void printUsage();

int numParams(const vector<string>& args, int i);

#endif

// end of file utils.h
