/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : piecesets.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifdef INPIECESET_H
#include "texture.h"
#include "piece.h"
#include "timer.h"
#include "q3charmodel.h"

class BasicSet : public PieceSet {

  public:
	BasicSet();
	bool load();
	bool unload();
	bool loadGL();
	bool unloadGL();
	void draw(const ChessGameState& cgs);
	void drawPiece(Piece* p, double alpha, bool alert);
	void hoverPosition(const ChessGameState& gs, const BoardPosition& bp);
	void animateMove(const BoardMove & bm);

  protected:
	ObjFile m_pieces[Piece::LAST_TYPE+1];
	Texture m_textures[Piece::LAST_COLOR+1][Piece::LAST_TYPE+1];
	BoardPosition m_lasthoverpos;
	double m_hoverheight[Board::BOARDSIZE][Board::BOARDSIZE];
	Timer m_hovertimer[Board::BOARDSIZE][Board::BOARDSIZE];
	pair<Timer, Timer> m_movetimer[Board::BOARDSIZE][Board::BOARDSIZE];
};

class DebugSet : public BasicSet {

  public:
	virtual bool load();
};

class Q3Set : public BasicSet {
  public:
	bool load();
	bool loadGL();

	void draw(const ChessGameState& gs);
	void drawPiece(Piece* p, double alpha=1.0);

  protected:
	Q3CharModel m_pieces[Piece::LAST_TYPE+1];
	Q3CharModel::Q3AnimState m_upIdle[Piece::LAST_TYPE+1];
	Q3CharModel::Q3AnimState m_lowIdle[Piece::LAST_TYPE+1];
	Q3CharModel::Q3AnimState m_hover[Piece::LAST_TYPE+1];
};

#endif

// end of file piecesets.h
