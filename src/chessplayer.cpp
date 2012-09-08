/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "chessplayer.h"

#include <string>

ChessPlayer * PlayerFactory(const std::string & playertype)
{
    if (playertype == "Brutal") {
        return new BrutalPlayer();
    }
    else if (playertype == "Human") {
        return new HumanPlayer();
    }
    else if (playertype == "Random") {
        return new RandomPlayer();
    }
#ifndef WIN32
    else if (playertype == "GnuChess") {
        return new XboardPlayer();
    }
    else if (playertype == "Faile") {
        return new FailePlayer();
    }
#endif // #ifndef WIN32
    else {
        return 0;
    }
}

// End of file chessplayer.cpp
