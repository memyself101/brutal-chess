/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardthemes.h
 * Authors : Mike Cook, Joe Flint
 **************************************************************************/
 
#ifdef INBOARDTHEME_H
#include "texture.h"
#include "board.h"
#include "boardposition.h"
#include "timer.h"

class GraniteTheme : public BoardTheme {
 public:
	GraniteTheme();
	void draw(const ChessGameState & cgs);
	void hoverPosition(const BoardPosition& bp);
 	bool load();
 	bool loadGL();
	void unloadGL();
 protected:
	Texture m_blacktexture, m_whitetexture;
	BoardPosition m_lasthoverpos;
	double m_overlayalpha[Board::BOARDSIZE][Board::BOARDSIZE];
	Timer m_overlaytimer[Board::BOARDSIZE][Board::BOARDSIZE];
};

#endif
