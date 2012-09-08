/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardtheme.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
 
#ifndef BOARDTHEME_H
#define BOARDTHEME_H
 
#include "boardposition.h"
#include "chessgamestate.h"

class BoardTheme {
 public:
 
 	/** Default constructor */
	BoardTheme() {}
 
	/** Draws the board */
	virtual void draw(const ChessGameState & cgs) = 0;
 
	/** Tells the BoardTheme where the mouse is. */
	virtual void hoverPosition(const BoardPosition& bp) {}

	/** Loads the textures for the theme. */
	virtual bool load() { return true; }

	/** Unloads the textures for the theme. */
	virtual void unload() {};		
		
	/** Creates the display list stuff */
	virtual bool loadGL() { return true; }
		
	/** Destroy the display list stuff */
	virtual void unloadGL() {}

    /** Sets whether history arrows should be drawn or not. */
    void setHistoryArrows(bool historyArrows);

    /** Toggles the drawing of history arrows on and off */
    void toggleHistoryArrows();

 protected:
	/** Draws arrows indicating the last moves made for a BoardTheme */
	virtual void drawMoveArrows(const ChessGameState & cgs);
};

#define INBOARDTHEME_H
	#include "boardthemes.h"
#undef INBOARDTHEME_H


#endif
 
// End of file boardtheme.h
