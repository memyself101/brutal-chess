/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : options.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "options.h"

Options* Options::m_instance = 0;

Options* Options::getInstance()
{
	if(m_instance == 0) {
		m_instance = new Options();
	}
	return m_instance;
}

Options::Options()
{
	// Defaults for all the options
	animations = true;
	fullscreen = false;
	historyarrows = true;
	reflections = true;
	shadows = true;
	board = GRANITE;
	pieces = BASIC;
	player1 = HUMAN;
	player1diff = MEDIUM;
	player2 = BRUTAL;
	player2diff = MEDIUM;
	resolution = r800X600;
	brutalplayer1ply = DEFAULT_PLY_DEPTH;
	brutalplayer2ply = DEFAULT_PLY_DEPTH;

	// Initialize the enum maps
	m_boardTypeString[GRANITE] = "Granite";
	m_boardTypeString[WOOD] = "Wood";

	m_piecesTypeString[BASIC] = "Basic";
	m_piecesTypeString[DEBUG] = "Debug";
	m_piecesTypeString[QUAKE] = "Quake";

	m_playerTypeString[BRUTAL] = "Brutal";
	m_playerTypeString[FAILE] = "Faile";
	m_playerTypeString[HUMAN] = "Human";
	m_playerTypeString[RANDOM] = "Random";
	m_playerTypeString[TEST] = "Test";
	m_playerTypeString[XBOARD] = "Xboard";

	m_difficultyString[EASY] = "Easy";
	m_difficultyString[MEDIUM] = "Medium";
	m_difficultyString[HARD] = "Hard";
	
	m_resolutionString[r640X480] = "640x480";
	m_resolutionString[r800X600] = "800x600";
	m_resolutionString[r1024X768] = "1024x768";
	m_resolutionString[r1280X1024] = "1280x1024";
	m_resolutionString[r1400X1050] = "1400x1050";
	m_resolutionString[r1600X1200] = "1600x1200";

	m_resolutionHeightInt[r640X480] = 480;
	m_resolutionHeightInt[r800X600] = 600;
	m_resolutionHeightInt[r1024X768] = 768;
	m_resolutionHeightInt[r1280X1024] = 1024;
	m_resolutionHeightInt[r1400X1050] = 1050;
	m_resolutionHeightInt[r1600X1200] = 1200;

	m_resolutionWidthInt[r640X480] = 640;
	m_resolutionWidthInt[r800X600] = 800;
	m_resolutionWidthInt[r1024X768] = 1024;
	m_resolutionWidthInt[r1280X1024] = 1280;
	m_resolutionWidthInt[r1400X1050] = 1400;
	m_resolutionWidthInt[r1600X1200] = 1600;
}

// end of file options.cpp
