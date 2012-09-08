/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : options.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <map>
#include <string>

enum BoardType {GRANITE, WOOD};
enum PiecesType {BASIC, DEBUG, QUAKE};
enum PlayerType {BRUTAL, FAILE, HUMAN, RANDOM, TEST, XBOARD};
enum Difficulty {EASY=2, MEDIUM=3, HARD=4};

#define DEFAULT_PLY_DEPTH MEDIUM

// Keep things synced between menu events and this
enum Resolution {r640X480, r800X600, r1024X768, r1280X1024, r1400X1050, r1600X1200, };

class Options {
 public:
	static Options* getInstance();

	bool animations, fullscreen, historyarrows, reflections, shadows;
	BoardType board;
	Difficulty player1diff, player2diff;
	PiecesType pieces;
	PlayerType player1, player2;
	Resolution resolution;

	// Won't need eventually
	int brutalplayer1ply, brutalplayer2ply;

    std::string getBoardString() 
		{ return m_boardTypeString[board]; }
	
    std::string getPiecesString()
		{ return m_piecesTypeString[pieces]; }

    std::string getPlayer1String()
		{ return m_playerTypeString[player1]; }

    std::string getPlayer1DiffString()
		{ return m_difficultyString[player1diff]; }

    std::string getPlayer2String()
		{ return m_playerTypeString[player2]; }

    std::string getPlayer2DiffString()
		{ return m_difficultyString[player2diff]; }

    std::string getResolutionString()
		{ return m_resolutionString[resolution]; }

	int getResolutionHeight()
		{ return m_resolutionHeightInt[resolution]; }

	int getResolutionWidth()
		{ return m_resolutionWidthInt[resolution]; }

 private:

	Options();

	static Options* m_instance;

    std::map<BoardType, std::string>    m_boardTypeString;
    std::map<PiecesType, std::string>   m_piecesTypeString;
    std::map<PlayerType, std::string>   m_playerTypeString;
    std::map<Difficulty, std::string>   m_difficultyString;
    std::map<Resolution, std::string>   m_resolutionString;
    std::map<Resolution, int>           m_resolutionHeightInt;
    std::map<Resolution, int>           m_resolutionWidthInt;
};

#endif

// end of file options.h
