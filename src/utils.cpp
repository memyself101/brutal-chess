/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : utils.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#include "options.h"
#include "utils.h"

using namespace std;

void Quit(int returnCode)
{
	// Delete our singleton class here
	SDL_Quit();
	exit(returnCode);
}

void printUsage()
{
	cerr << "Usage: brutalchess [-h] [-afrs] [-l PLAYER1 PLAYER2] [-p PIECE_SET]\n";
	cerr <<	"                   [-b BOARD_THEME] [-z SCREEN_RESOLUTION]";
	cerr << endl << endl << endl;
	cerr << " -a  --animations=on|off\t\t\t Turn off animations, on by default.";
	cerr << endl << endl;
	cerr << " -b BOARD_THEME  --board=BOARD_THEME\t\t Set the in-game board. Choices are granite.";
	cerr << endl << endl;
	cerr << " -f  --fullscreen=on|off\t\t\t Play in fullscreen mode, windowed by default.";
        cerr << endl << endl;
	cerr << " -h  --help\t\t\t\t\t Print this help screen.";
	cerr << endl << endl;
	cerr << " -l PLAYER1 PLAYER2  --player1=PLAYER1\t\t Set your player and opponent. Choices are brutal,\n";
	cerr << "                     --player2=PLAYER2\t\t faile, human, random, test, xboard.";
	cerr << endl << endl;
	cerr << " -p PIECE_SET  --pieces=PIECE_SET\t\t Select the piece set. Choices are basic, quake.";
	cerr << endl << endl;
	cerr << " -r  --reflections=on|off\t\t\t Turn off reflections, on by default.";
	cerr << endl << endl;
	cerr << " -s  --shadows=on|off\t\t\t\t Turn off shadows, on by default.";
	cerr << endl << endl;
	cerr << " -z WIDTHxHEIGHT  --resolution=WIDTHxHEIGHT\t Set screen resolution. Currently support 640x480,\n";
	cerr << "                                           \t 800x600, 1024x768, 1280x1024, 1400x1050, 1600x1200.";
	cerr << endl << endl;
	Quit(1);
}

void parseCommandLine(int argc, char* argv[])
{
	Options* opts = Options::getInstance();

	vector<string> args;
	for(int i=1; i < argc; i++)
		args.push_back(string(argv[i]));

	// -z WIDTHxHEIGHT  --resolution=WIDTHxHEIGHT
	for(int i=0; i < args.size(); i++) {
		if(args[i].substr(0,13) == "--animations=") {
			if(args[i].substr(13,3) == "on") {
				opts->animations = true;
			} else if(args[i].substr(13,4) == "off") {
				opts->animations = false;
			} else {
				printUsage();
			}
		} else if(args[i] == "-b" && numParams(args,i) == 1) {
			if(args[i+1] == "granite") {
				opts->board = GRANITE;
			} else if(args[i+1] == "wood") {
				opts->board = WOOD;
			} else {
				printUsage();
			}
			i++;
		} else if(args[i].substr(0,8) == "--board=") {
			if(args[i+1] == "granite") {
				opts->board = GRANITE;
			} else if(args[i+1] == "wood") {
				opts->board = WOOD;
			} else {
				printUsage();
			}
		} else if(args[i].substr(0,13) == "--fullscreen=") {
			if(args[i].substr(13,3) == "on") {
				opts->fullscreen = true;
			} else if(args[i].substr(13,4) == "off") {
				opts->fullscreen = false;
			} else {
				printUsage();
			}
		} else if(args[i] == "-h" || args[i] == "--help") {
			printUsage();
		} else if(args[i] == "-l" && numParams(args,i) == 2) {
			if(args[i+1] == "brutal") {
				opts->player1 = BRUTAL;
			} else if(args[i+1] == "human") {
				opts->player1 = HUMAN;
			} else if(args[i+1] == "random") {
				opts->player1 = RANDOM;
			} else if(args[i+1] == "test") {
				opts->player1 = TEST;
			} else if(args[i+1] == "xboard") {
				opts->player1 = XBOARD;
			} else {
				printUsage();
			}
			if(args[i+2] == "brutal") {
				opts->player1 = BRUTAL;
			} else if(args[i+2] == "human") {
				opts->player1 = HUMAN;
			} else if(args[i+2] == "random") {
				opts->player1 = RANDOM;
			} else if(args[i+2] == "test") {
				opts->player1 = TEST;
			} else if(args[i+2] == "xboard") {
				opts->player1 = XBOARD;
			} else {
				printUsage();
			}
			i+=2;
		} else if(args[i].substr(0,10) == "--player1=") {
			if(args[i].substr(10, 10) == "brutal") {
				opts->player1 = BRUTAL;
			} else if(args[i].substr(10, 10) == "human") {
				opts->player1 = HUMAN;
			} else if(args[i].substr(10, 10) == "random") {
				opts->player1 = RANDOM;
			} else if(args[i].substr(10, 10) == "test") {
				opts->player1 = TEST;
			} else if(args[i].substr(10, 10) == "xboard") {
				opts->player1 = XBOARD;
			} else {
				printUsage();
			}
		} else if(args[i].substr(0,10) == "--player2=") {
			if(args[i].substr(10, 10) == "brutal") {
				opts->player2 = BRUTAL;
			} else if(args[i].substr(10, 10) == "human") {
				opts->player2 = HUMAN;
			} else if(args[i].substr(10, 10) == "random") {
				opts->player2 = RANDOM;
			} else if(args[i].substr(10, 10) == "test") {
				opts->player2 = TEST;
			} else if(args[i].substr(10, 10) == "xboard") {
				opts->player2 = XBOARD;
			} else {
				printUsage();
			}
		} else if(args[i] == "-p" && numParams(args,i) == 1) {
			if(args[i+1] == "basic") {
				opts->pieces = BASIC;
			} else if(args[i+1] == "debug") {
				opts->pieces = DEBUG;
			} else if(args[i+1] == "quake") {
				opts->pieces = QUAKE;
			} else {
				printUsage();
			}
			i++;
		} else if(args[i].substr(0,9) == "--pieces=") {
			if(args[i].substr(9, args[i].size()) == "basic") {
				opts->pieces = BASIC;
			} else if(args[i].substr(9, args[i].size()) == "debug") {
				opts->pieces = DEBUG;
			} else if(args[i].substr(9, args[i].size()) == "quake") {
				opts->pieces = QUAKE;
			} else {
				printUsage();
			}
		} else if(args[i].substr(0,14) == "--reflections=") {
			if(args[i].substr(14,3) == "on") {
				opts->reflections = true;
			} else if(args[i].substr(14,4) == "off") {
				opts->reflections = false;
			} else {
				printUsage();
			}
		} else if(args[i].substr(0,10) == "--shadows=") {
			if(args[i].substr(10,3) == "on") {
				opts->shadows = true;
			} else if(args[i].substr(10,4) == "off") {
				opts->shadows = false;
			} else {
				printUsage();
			}
		} else if(args[i] == "-z" && numParams(args,i) == 1) {
			if(args[i+1] == "640x480") {
				opts->resolution = r640X480;
			} else if(args[i+1] == "800x600") {
				opts->resolution = r800X600;
			} else if(args[i+1] == "1024x768") {
				opts->resolution = r1024X768;
			} else if(args[i+1] == "1280x1024") {
				opts->resolution = r1280X1024;
			} else if(args[i+1] == "1400x1050") {
				opts->resolution = r1400X1050;
			} else if(args[i+1] == "1600x1200") {
				opts->resolution = r1600X1200;
			} else {
				printUsage();
			}
			i++;
		} else if(args[i].substr(0,13) == "--resolution=") {
			if(args[i].substr(13,8) == "640x480") {
				opts->resolution = r640X480;
			} else if(args[i].substr(13,8) == "800x600") {
				opts->resolution = r800X600;
			} else if(args[i].substr(13,9) == "1024x768") {
				opts->resolution = r1024X768;
			} else if(args[i].substr(13,10) == "1280x1024") {
				opts->resolution = r1280X1024;
			} else if(args[i].substr(13,10) == "1400x1050") {
				opts->resolution = r1400X1050;
			} else if(args[i].substr(13,10) == "1600x1200") {
				opts->resolution = r1600X1200;
			} else {
				printUsage();
			}
		} else if(args[i].substr(0,2) == "-a" || args[i].substr(0,2) == "-f" ||
		   args[i].substr(0,2) == "-r" || args[i].substr(0,2) == "-s" || args[i].substr(0,2) == "-w") {
			for(int j=1; j < args[i].size(); j++) {
				if(args[i][j] == 'a') {
					opts->animations = false;
				} else if(args[i][j] == 'f') {
					opts->fullscreen = true;
				} else if(args[i][j] == 'r') {
					opts->reflections = false;
				} else if(args[i][j] == 's') {
					opts->shadows = false;
				} else if (args[i][j] == 'w') {
					opts->historyarrows = false;
				} else {
					printUsage();
				}
			}
		} else {
			printUsage();
		}
	}
}

BoardTheme* toBoard(BoardType board)
{
	if(board == GRANITE) {
		return new GraniteTheme();
	} printUsage();
}

ChessPlayer* toPlayer(PlayerType player)
{
	if(player == BRUTAL) {
		return new BrutalPlayer();
	} else if(player == HUMAN) {
		return new HumanPlayer();
	} else if(player == RANDOM) {
		return new RandomPlayer();
//	} else if(player == TEST) {
//		return new TestPlayer();
	} 
#ifndef WIN32
	else if(player == FAILE) {
		return new FailePlayer();
	} else if(player == XBOARD) {
		return new XboardPlayer();
	} 
#endif	
	printUsage();
}

PieceSet* toPieces(PiecesType pieces)
{
	if(pieces == BASIC) {
		return new BasicSet();
	} else if(pieces == DEBUG) {
		return new DebugSet();
	} else if(pieces == QUAKE) {
		return new Q3Set();
	} printUsage();
}

int numParams(const vector<string>& args, int i)
{
	int num = 0;
	while(i+1 < args.size() && args[i+1][0] != '-') {
		i++;
		num++; 
	}
	return num;
}

// end of file utils.cpp

