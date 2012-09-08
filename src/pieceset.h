/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : pieceset.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef PIECESET_H
#define PIECESET_H

#include "board.h"
#include "boardposition.h"
#include "objfile.h"
#include "chessgamestate.h"

/**
 * This abstract base class defines methods for the current set of 
 * pieces on the board. This class will handle all manipulations that
 * are associated with the pieces
 */
class PieceSet {

  public:

	PieceSet() : m_drawPromotionSelector(false) {}
	
	/**
	 * Load the current piece models into memory
	 */
	virtual bool load();

	/**
	 * Remove the current piece models from memory
	 */
	virtual bool unload();

	/**
	 * Used for all the OpenGL context based methods
	 */
	virtual bool loadGL();

	/**
	 * Used for all the OpenGL context based methods
	 */
	virtual bool unloadGL();

	/**
	 * Display the pieces on the current board
	 * @param b - Board with positions of all pieces
	 */
	virtual void draw(const ChessGameState& cgs) = 0;

	/**
	 * Draw a single piece
	 * @param p - Piece to be drawn
	 * @param alpha - Alpha channel for the piece
	 */
	virtual void drawPiece(Piece* p, double alpha, bool alert) = 0;

	/**
	 * Try to select the piece at the given board postion 
	 * @param bp - BoardPosition being selected
	 */
	virtual void selectPosition(const BoardPosition& bp);

	/**
	 * Deselect the currently selected position
	 */
	virtual void deselectPosition();

	/**
	 * Tell PieceSet where the mouse is hovering
	 * @param bp - BoardPosition that mouse is hovering over
	 * @param gs - current state of the chess game
	 */
	virtual void hoverPosition(const ChessGameState& gs, const BoardPosition& bp);

	/**
	 * Mouse position in board coordinates
	 * @param x - X coord of mouse position in board space
	 * @param y - Y coord of mouse position in board space
	 */
	virtual void mousePosition(float x, float y);

	/**
	 * Animate move
	 */
	virtual void animateMove(const BoardMove & bm) = 0;

    void drawPromotionSelector(bool draw_promotion_selector)
        { m_drawPromotionSelector = draw_promotion_selector; }

  protected:
	
	BoardPosition m_selected;
	BoardPosition m_current;
	float m_mouseX;
	float m_mouseY;
	bool m_drawPromotionSelector;

};

#define INPIECESET_H
	#include "piecesets.h"
#undef INPIECESET_H

#endif

// end of file pieceset.h
