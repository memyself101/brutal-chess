/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardtheme.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "boardmove.h"
#include "boardtheme.h"
#include "chessgamestate.h"
#include "options.h"
#include "vector.h"

#include <cmath>

// Helper declarations
static void drawArrowShaft(const BoardMove & bm);
static void drawArrowHead(const BoardMove & bm, const float & angle = -1.0);
static void drawAngleIron(const BoardMove & bm);
static float getAngleFromMove(const BoardMove & bm);

// Draws either an arrow from the piece's origin to its destination, or,
// if the piece is a knight, draws an "L" shaped angle-iron representing
// the path of the piece

void BoardTheme::drawMoveArrows(const ChessGameState & cgs)
{
    if (!Options::getInstance()->historyarrows)
        return;

    BoardMove move = cgs.getLastMove();
	
	// Make sure that we have moved at least once
	if (!move.origin().isValid())
		return;
	
	// Get the piece, since if its a Knight we need to draw an angle instead
	// of drawing a direct arrow
	const Piece * piece = move.getPiece();
	glNormal3d(0.0, 7.0, 0.0);
	glColor4d(1.0, 1.0, 1.0, 0.5);
	if (Piece::KNIGHT == piece->type())
		drawAngleIron(move);
	else {
		drawArrowShaft(move);
		drawArrowHead(move);
	}
}

// ******** Helpers ********

// Draws the shaft of the arrow.. duh
inline void drawArrowShaft(const BoardMove & bm)
{
	float arw_tks = 0.075;	// Arrow thickness
	float offset = 0.21;	// How close the arrow head is to the piece
	float height = 0.01;	// height above the board to draw the shaft
	float length  = sqrt(pow(bm.rankDiff(), 2.0) + pow(bm.fileDiff(), 2.0)) - offset;

	// Move to the origin of the BoardMove
	glPushMatrix();
	glTranslated(bm.origin().file0() + 0.5, 0.01, -(bm.origin().rank0() + 0.5));
	
	// Rotate the shaft
	float angle = getAngleFromMove(bm);
	glRotated( angle, 0.0, 1.0, 0.0);	
	
	// Draw the shaft of the arrow
	glBegin(GL_QUADS);
		glVertex3f( arw_tks, height, 0.0);
		glVertex3f( arw_tks, height, -length + offset);
		glVertex3f(-arw_tks, height, -length + offset);
		glVertex3f(-arw_tks, height, 0.0);
	glEnd();

	glPopMatrix();
}

// Draws the head of the arrow either computed from the boardmove, or from
// the angle passed into the function
inline void drawArrowHead(const BoardMove & bm, const float & in_angle)
{
	float width = 0.18;	// Width of the arrow
	float height = 0.01;	// height above the board to draw the arrow

	// Move to the destination of the BoardMove
	glPushMatrix();
	glTranslated(bm.dest().file0() + 0.5, 0.0, -(bm.dest().rank0() + 0.5 ));
	
	// Rotate the arrow head to match up with the shaft
	float angle = getAngleFromMove(bm);
	angle = in_angle == -1.0 ? angle : in_angle;
	glRotated( angle, 0.0, 1.0, 0.0 );
	
	// Draw the arrow head at the rotated location
	glBegin(GL_TRIANGLES);
		glVertex3f( 0.0, height, 0.25);
		glVertex3f(- width, height, 0.46);
		glVertex3f( width, height, 0.46);
	glEnd();

	glPopMatrix();
}

// Draws the "L" shaped path that represents a Knight move
inline void drawAngleIron(const BoardMove & bm)
{
	float arw_width = 0.075;
	float height = 0.01;
	float start_len = bm.rankDiff();
	float end_len = start_len == 2 ? 1.0 : 2.0;

	// Draw the first section
	glPushMatrix();
	glTranslated(bm.origin().file0() + 0.5, 0.0, -(bm.origin().rank0() + 0.5));

	// If we're moving down the board, flip the shaft
	if (bm.signedRankDiff() < 0)
		glRotated( 180.0, 0.0, 1.0, 0.0 );
	
	glBegin(GL_QUADS);
		glVertex3f( arw_width, height, 0.0);
		glVertex3f( arw_width, height, -start_len - arw_width);
		glVertex3f(-arw_width, height, -start_len - arw_width);
		glVertex3f(-arw_width, height, 0.0);
	glEnd();

	glPopMatrix();

	// Now draw the second section
	glPushMatrix();
	glTranslated(bm.dest().file0() + 0.5, 0.0, -(bm.dest().rank0() + 0.5 ));

	if (bm.signedFileDiff() > 0)
		glRotated(  90.0, 0.0, 1.0, 0.0 );
	else
		glRotated( -90.0, 0.0, 1.0, 0.0 );
	
	glBegin(GL_QUADS);
		glVertex3f( arw_width, height, -0.46);
		glVertex3f( arw_width, height, -end_len + arw_width);
		glVertex3f(-arw_width, height, -end_len + arw_width);
		glVertex3f(-arw_width, height, -0.46);
	glEnd();
		
	glPopMatrix();

	// Now draw the arrow head
	if (bm.signedFileDiff() > 0)
		drawArrowHead(bm, -90.0);
	else
		drawArrowHead(bm,  90.0);
}

// Calculates the angle between north and the piece's movement vector
inline float getAngleFromMove(const BoardMove & bm)
{
	// Rotate the arrow head to line up w/ its body
	Vector v1(0.0, 0.0, -1.0);
	Vector v2(bm.signedFileDiff(), 0.0, -bm.signedRankDiff());
	v2.normalize();
	float angle = acos(v1.dot(v2))*(180.0/3.14159);
	if (bm.signedFileDiff() > 0)
		angle *= -1;
	return angle;
}

void BoardTheme::toggleHistoryArrows()
{
    Options* opts = Options::getInstance();
    opts->historyarrows = !opts->historyarrows;
}

void BoardTheme::setHistoryArrows(bool historyArrows)
{
    Options::getInstance()->historyarrows = historyArrows;
}

// End of file boardtheme.cpp

