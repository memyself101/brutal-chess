/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : board.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include <iostream>
#include <vector>

#include "board.h"

using std::cout;
using std::endl;
using std::vector;

unsigned long long Board::pawnAttacks[2][64];
unsigned long long Board::knightAttacks[64];
unsigned long long Board::kingAttacks[64];
unsigned long long Board::rankAttacks[64][256];
unsigned long long Board::fileAttacks[64][256];
unsigned long long Board::diagAttacksSE[64][256];
unsigned long long Board::diagAttacksNE[64][256];

Piece* Board::m_allpieces[Piece::LAST_COLOR + 1][Piece::LAST_TYPE + 1];
bool Board::m_setup = false;

Board::Board()
{
	reset();
	if(!m_setup) {
		setupPieces();
	}
}

Board::~Board()
{
}

void Board::reset()
{
	for (int i=0; i <= Piece::LAST_COLOR; i++) {
		m_color[i] = 0LL;
	}

	for (int i=0; i <= Piece::LAST_TYPE; i++) {
		m_pieces[i] = 0LL;
		m_piece_count[Piece::WHITE][i] = 0;
	       	m_piece_count[Piece::BLACK][i] = 0;
	}

	// Hex value to initialize the appropriate castling flags
	m_castling_flags = 0x9100000000000091LL;
	m_enpassant_flags = 0LL;
	m_total_pieces[Piece::WHITE] = 0;
	m_total_pieces[Piece::BLACK] = 0;
}

void Board::setupPieces()
{
	for(int i=0; i <= Piece::LAST_COLOR; i++) {
		for(int j=0; j <= Piece::LAST_TYPE; j++) {
			m_allpieces[i][j] = new Piece(Piece::Color(i), Piece::Type(j));
		}
	}

	m_setup = true;
}

// Returns the Piece at BoardPosition 'bp'.
Piece* Board::getPiece(const BoardPosition & bp) const
{
	if(!isOccupied(bp)) {
		return NULL;
	}

	unsigned long long mask = getMask(bp);
	Piece::Color color;

	color = m_color[Piece::WHITE] & mask ? Piece::WHITE : Piece::BLACK;
	// TODO - What happens its not present, see above.

	for (int i = 0; i <= Piece::LAST_TYPE; i++) {
		if (m_pieces[i] & mask) {
			return m_allpieces[color][i];
		}
	}

	// Should never get here.
	return NULL;
}

// Sets the boardposition to piece p of type t
void Board::setPiece(Piece::Color c, Piece::Type t, const BoardPosition& bp)
{
	if(isOccupied(bp)) {
		Piece * taken = getPiece(bp); 
		m_piece_count[taken->color()][taken->type()]--;
		m_total_pieces[taken->color()]--;
		removePiece(bp);
	}

	setBit(m_pieces[t], bp);
	setBit(m_color[c], bp);

	if(t == Piece::KING) {
		m_king_pos[c] = bp;
	}
}

// Puts a new piece onto the board, overwrites whatever is at that
// BoardPosition.
void Board::setPiece(Piece * piece, const BoardPosition & bp)
{
	if(isOccupied(bp)) {
		Piece * taken = getPiece(bp);
		m_piece_count[taken->color()][taken->type()]--;
		m_total_pieces[taken->color()]--;
		removePiece(bp);
	}
    
	setBit(m_pieces[piece->m_type], bp);
	setBit(m_color[piece->m_color], bp);

	if(piece->m_type == Piece::KING) {
		m_king_pos[piece->m_color] = bp;
	}
}

// Deletes the pieces at 'bp' and sets the pointer to 0.
void Board::removePiece(const BoardPosition & bp)
{
	unsetAllBits(bp);
}

void Board::addPiece(Piece * p, const BoardPosition & bp)
{
	m_total_pieces[p->m_color]++;
	m_piece_count[p->m_color][p->m_type]++;
	setPiece(p, bp);
}
	
// Returns false if there are any pieces between 'start' and 'end' exclusive.
bool Board::isPathClear(const BoardPosition & start, const BoardPosition & end) const
{
	bool swap = (start.m_file0 > end.m_file0);
	const BoardPosition & st = (swap) ? end : start;
	const BoardPosition & en = (swap) ? start : end;
	
	int EWsign = (en.m_file0-st.m_file0) ? 1 : 0;
	int NSsign = (en.m_rank0-st.m_rank0)/((en.m_rank0-st.m_rank0 == 0) ? 1 : abs(en.m_rank0-st.m_rank0));
	
	BoardPosition currPos = st;
	while (true) {
		currPos.m_file0 += EWsign;
		currPos.m_rank0 += NSsign;
		if (currPos == en)
			break;
		if (isOccupied(currPos))
			return false;
	}
		
	return true;
}

// Checks to see if the bit at 'file-rank' is set on the occupied_bitfield.
bool Board::isOccupied(const BoardPosition & bp) const
{
	return (m_color[Piece::WHITE] | m_color[Piece::BLACK]) & getMask(bp);
}

bool Board::isMoveLegal(const BoardMove & bm) const
{
	Piece* piece = getPiece(bm.origin());
       	Piece* captured = getPiece(bm.dest());

	if(!piece) { 
		return false;
	}

	if(!bm.isValid()) {
		return false;
	}

	Piece::Color color = piece->color();
	Piece::Type type = piece->type();

	// If the move is obviously wrong, bail early
	if(!bm.isLegal()) { 
		return false;
	}

	// Legal moves must actually go somewhere
	if(bm.origin() == bm.dest()) {
		return false;
	}

	// Can't attack one of your own pieces
	if(captured && (captured->color() == color)) {
		return false;
	}
	
	// Check path for all pieces except knight
	if(type != Piece::KNIGHT && !isPathClear(bm)) {
		return false;
	}
	
	// Check special cases for the pawn
	if(type == Piece::PAWN) 
	{
		if(bm.rankDiff() == bm.fileDiff()) {
			if(!isOccupied(bm.dest()) && !isEnPassantSet(bm.dest())) {
				return false;
			}
		} else if(bm.fileDiff()) {
			return false;
		} else if (isOccupied(bm.dest())) {
			return false;
		}
	}

	// Check for all the castling stuff
	if(type == Piece::KING && bm.fileDiff() == 2)
	{
		// Check if the king has moved
		if(!(getMask(bm.origin()) & m_castling_flags)) {
			return false;
		}
		
		// Check if the king is in check, or if castling places him in check.
        if(this->isAttacked(bm.origin(), color) || this->isAttacked(bm.dest(), color)) {
			return false;
		}

		char oldRookFile;
		if(bm.signedFileDiff() == -2) {
			oldRookFile = 'a';
		} else {
			oldRookFile = 'h';
		}

		// Check if the rook has moved
		BoardPosition corner = BoardPosition(oldRookFile, bm.origin().rank());
		if(!(getMask(corner) & m_castling_flags)) {
			return false;
		}

        // There must be no pieces between the king and rook.
        BoardPosition pos;
        bool castleEast = (bm.signedFileDiff() > 0);
        for (pos = castleEast ? bm.origin().E() : bm.origin().W(); pos != corner; pos = (castleEast) ? pos.E() : pos.W()) {
            if (this->getPiece(pos)) {
                return false;
            }
        }

        // The king can not pass through squares that are under attack by enemy pieces.
        if (castleEast && (this->isAttacked(bm.origin().E(), color))) {
            return false;            
        }
        else if (!castleEast && this->isAttacked(bm.origin().W(), color)) {
            return false;
        }
	}

    /**********************DEBUG CODE*******************************/
    /*
    unsigned long long attack = 0LL;
    BoardPosition bp = bm.dest();
    switch(type) {
        case Piece::KING:
            attack = kingAttacks[bp.hash()];
            break;
        case Piece::KNIGHT:
            attack = knightAttacks[bp.hash()];
            break;
        case Piece::PAWN:
            attack = pawnAttacks[color][bp.hash()];
            break;
        case Piece::QUEEN:
        case Piece::BISHOP:
            attack |= diagAttacksSE[bp.hash()][getSEDiagState(bp)];
            attack |= diagAttacksNE[bp.hash()][getNEDiagState(bp)];
            if(type == Piece::BISHOP)
                break;
        case Piece::ROOK:
            attack |= rankAttacks[bp.hash()][getRankState(bp)];
            attack |= fileAttacks[bp.hash()][getFileState(bp)];
            break;
    }
    */
    /***********************************************************/

	if(isResultCheck(bm)) {
		return false;
	}
	
	return true;
}

unsigned long long Board::isAttacked(const BoardPosition& bp, Piece::Color c) const
{
	Piece::Color attacker = Piece::opposite(c);
	int pos = bp.hash();
	int rank = getRankState(bp);
	int file = getFileState(bp);
	int diagSE = getSEDiagState(bp);
	int diagNE = getNEDiagState(bp);

	unsigned long long board = 0LL;
	board |= pawnAttacks[c][pos] & m_pieces[Piece::PAWN] & m_color[attacker];
	board |= knightAttacks[pos] & m_pieces[Piece::KNIGHT] & m_color[attacker];
	board |= kingAttacks[pos] & m_pieces[Piece::KING] & m_color[attacker];
	board |= rankAttacks[pos][rank] & m_pieces[Piece::ROOK] & m_color[attacker];
	board |= rankAttacks[pos][rank] & m_pieces[Piece::QUEEN] & m_color[attacker];
	board |= fileAttacks[pos][file] & m_pieces[Piece::ROOK] & m_color[attacker];
	board |= fileAttacks[pos][file] & m_pieces[Piece::QUEEN] & m_color[attacker];
	board |= diagAttacksSE[pos][diagSE] & m_pieces[Piece::BISHOP] & m_color[attacker];
	board |= diagAttacksSE[pos][diagSE] & m_pieces[Piece::QUEEN] & m_color[attacker];
	board |= diagAttacksNE[pos][diagNE] & m_pieces[Piece::BISHOP] & m_color[attacker];
	board |= diagAttacksNE[pos][diagNE] & m_pieces[Piece::QUEEN] & m_color[attacker];

	return board;
}

bool Board::isResultCheck(const BoardMove& bm) const
{
	Board board = *this;
	board.update(bm);
	return board.isCheck(bm.getPiece()->color());
}

bool Board::isCheckMate(Piece::Color c) const
{
	// Player is in check and has no legal moves
	if(!isCheck(c)) {
		return false;
	} else if(possibleMoves(c, true).empty()) {
		return true;
	} return false;
}

bool Board::isStaleMate(Piece::Color c) const
{
	// Player isn't in check but has no legal moves
	if(isCheck(c)) {
		return false;
	} else if(possibleMoves(c, true).empty()) {
		return true;
	} return false;
}

bool Board::isMaterialDraw() const
{
	// Still plenty of material
	if(m_total_pieces[Piece::WHITE] + m_total_pieces[Piece::BLACK] >= 4) {
		return false;
	}

	// King v. King
	if(m_total_pieces[Piece::WHITE] + m_total_pieces[Piece::BLACK] == 2) {
		return true;
	}

	// At least one Rook, Queen or Pawn still on the board
	if(m_piece_count[Piece::WHITE][Piece::ROOK] || m_piece_count[Piece::WHITE][Piece::QUEEN] ||
	   m_piece_count[Piece::BLACK][Piece::ROOK] || m_piece_count[Piece::BLACK][Piece::QUEEN] ||
	   m_piece_count[Piece::WHITE][Piece::PAWN] || m_piece_count[Piece::BLACK][Piece::PAWN]) {
		return false;
	}

	// King v. King and Knight or King v.King and Bishop
	return true;
}

vector<BoardMove> Board::possibleMoves(Piece::Color c, bool findOne) const
{
	vector<BoardPosition> pos;
	vector<BoardMove> goodMoves, trialMoves;
	
	// Find all of this players pieces
	for(int i=0; i < BOARDSIZE*BOARDSIZE; i++) {
		if((m_color[c] >> i) & 1)
			pos.push_back(BoardPosition(i));
	}

	// Find all possible move for each piece
	for(int i=0; i < pos.size(); i++) {
		Piece* p = getPiece(pos[i]);
		BoardPosition bp;

        // This is here since C++ is gay
        int last;
		
        // Generate possible moves for pieces performing a few simple tests
		// to make sure that they are valid
		switch(p->type()) {
			case Piece::PAWN:
				if(c == Piece::WHITE) {
					trialMoves.push_back(BoardMove(pos[i], pos[i].N().N(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].N(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].NE(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].NW(), p));
				} else {
				    trialMoves.push_back(BoardMove(pos[i], pos[i].S().S(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].S(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].SE(), p));
					trialMoves.push_back(BoardMove(pos[i], pos[i].SW(), p));
				}

				// Needed outside the loop since size of trialMoves changes
				last = trialMoves.size()-1;
				for(int j=0; j < 3; j++) {
					if(trialMoves[last-j].needPromotion()) {
						BoardMove move = trialMoves[last-j];
						trialMoves[last-j].setPromotion(Piece::QUEEN);
						move.setPromotion(Piece::ROOK);
						trialMoves.push_back(move);
						move.setPromotion(Piece::BISHOP);
						trialMoves.push_back(move);
						move.setPromotion(Piece::KNIGHT);
						trialMoves.push_back(move);
					}
				}
				break;
			case Piece::KNIGHT:
				trialMoves.push_back(BoardMove(pos[i], pos[i].NE().N(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].NE().E(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].NW().N(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].NW().W(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SE().S(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SE().E(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SW().S(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SW().W(), p));
				break;
			case Piece::QUEEN:
			case Piece::BISHOP:
				for(bp = pos[i].NE(); !bp.outNE(); bp.moveNE()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 
				for(bp = pos[i].NW(); !bp.outNW(); bp.moveNW()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 

				for(bp = pos[i].SW(); !bp.outSW(); bp.moveSW()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 

				for(bp = pos[i].SE(); !bp.outSE(); bp.moveSE()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				}
				if(p->type() == Piece::BISHOP)
					break;
			case Piece::ROOK:
				for(bp = pos[i].N(); !bp.outN(); bp.moveN()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 

				for(bp = pos[i].W(); !bp.outW(); bp.moveW()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 

				for(bp = pos[i].S(); !bp.outS(); bp.moveS()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				} 

				for(bp = pos[i].E(); !bp.outE(); bp.moveE()) {
					trialMoves.push_back(BoardMove(pos[i], bp, p));
					if(isOccupied(bp))
						break;
				}
				break;
			case Piece::KING:
				trialMoves.push_back(BoardMove(pos[i], pos[i].N(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].S(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].E(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].W(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].NE(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].NW(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SE(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].SW(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].E().E(), p));
				trialMoves.push_back(BoardMove(pos[i], pos[i].W().W(), p));
				break;
		}

		// For the moves that passed the basic test
		for(int j=0; j < trialMoves.size(); j++) {
			if(isMoveLegal(trialMoves[j])) {
				goodMoves.push_back(trialMoves[j]);
			}
			if(goodMoves.size() && findOne) {
				return goodMoves;
			}
		}
		trialMoves.clear();
	}
	
	return goodMoves;
}

int Board::getRankState(const BoardPosition & bp) const
{
	return ((m_color[Piece::WHITE] | m_color[Piece::BLACK]) >> bp.rank0()*BOARDSIZE) & 0xff;
}

int Board::getFileState(const BoardPosition & bp) const
{
        long long state = 0LL;
        int ret = 0;
        unsigned char rank = bp.rank0();
        unsigned char file = bp.file0();

        state = (m_color[Piece::WHITE] | m_color[Piece::BLACK]) & maskFile(bp);
        char shift = file;
        for(int i = 0; i < 7; i++) {
                ret |= (state >> shift);
                shift += BOARDSIZE - 1;
        }

        return ret & 0xFF;
}

int Board::getSEDiagState(const BoardPosition & bp) const
{
	// Change this to use bit shifting in the future so it's faster
	int state = 0, f = bp.file0(), r = bp.rank0();
	BoardPosition tmp;

	if(f+r < BOARDSIZE)
		tmp = BoardPosition(0, f+r);
	else
		tmp = BoardPosition(f+r-7, 7);

	for(int i = 0; !tmp.outSE(); tmp.moveSE(), i++)
		if(getMask(tmp) & (m_color[Piece::WHITE] | m_color[Piece::BLACK]))
			state += pow2[i];

	return state;
}

int Board::getNEDiagState(const BoardPosition & bp) const
{
	// Change this to use bit shifting in the future so it's faster
	int state = 0, f = bp.file0(), r = bp.rank0();
	BoardPosition tmp;

	if(f > r)
		tmp = BoardPosition(f-r, 0);
	else
		tmp = BoardPosition(0, r-f);

	for(int i = 0; !tmp.outNE(); tmp.moveNE(), i++)
		if(getMask(tmp) & (m_color[Piece::WHITE] | m_color[Piece::BLACK]))
			state += pow2[i];
	
	return state;
}

// Given a valid and legal move, updates the board to reflect the move.
// This function should only be called from ChessGame::tryMove.
void Board::update(const BoardMove & bm)
{
	Piece::Color color = getPiece(bm.origin())->color();
	Piece::Type type = getPiece(bm.origin())->type();
	
	if(type == Piece::PAWN) {
		if(isEnPassantSet(bm.dest()) && color == Piece::WHITE) {
			removePiece(bm.dest().S());
		} else if(isEnPassantSet(bm.dest()) && color == Piece::BLACK) {
			removePiece(bm.dest().N());
		} else if(bm.dest().rank() == 8 || bm.dest().rank() == 1) {
			type = bm.getPromotion();
		}
	} else if(type == Piece::KING && bm.signedFileDiff() == -2) {
		// Castling to the left
		BoardPosition corner = (color == Piece::WHITE) ?
			BoardPosition('a',1) : BoardPosition('a',8);

		setPiece(getPiece(corner), bm.origin().W());
		removePiece(corner);

		m_castling_flags &= ~getMask(corner);
	} else if(type == Piece::KING && bm.signedFileDiff() == 2) {
		// Castling to the right
		BoardPosition corner = (color == Piece::WHITE) ?
			BoardPosition('h',1) : BoardPosition('h',8);

		setPiece(getPiece(corner), bm.origin().E());
		removePiece(corner);

		m_castling_flags &= ~getMask(corner);
	}

	setPiece(color, type, bm.dest());
	removePiece(bm.origin());
	setSpecialPieceFlags(bm);
}

void Board::setSpecialPieceFlags(const BoardMove & bm)
{
	Piece::Color color = bm.getPiece()->color();
	Piece::Type  type  = bm.getPiece()->type();

	// Reset En Passant flag from previous turn
	if (color == Piece::BLACK) {
		m_enpassant_flags &= ~maskRank(BoardPosition('a', 3));
	} else {
		m_enpassant_flags &= ~maskRank(BoardPosition('a', 6));
	}

	// Remove castling flags if any piece attacks a corner
	m_castling_flags &= ~getMask(bm.dest());

	// Queens, knights and bishops don't have any special moves. If the 
	// piece is a pawn but doesn't move 2 ranks, nothing to be done.
	if (type == Piece::QUEEN || type == Piece::KNIGHT || type == Piece::BISHOP) {
		return;
	}
	if (type == Piece::PAWN && bm.rankDiff() != 2) {
		return;
	}

	// Handle En-Passant
	if (type == Piece::PAWN && color == Piece::WHITE) {
		// Set the en-passant bit for the pawn that moved.	
		m_enpassant_flags |= getMask(bm.origin().N());
		return;
	} else if (type == Piece::PAWN && color == Piece::BLACK) {
		// Pretty much same as the white case
		m_enpassant_flags |= getMask(bm.origin().S());
		return;
	} 
	// Remove castling flag because a king or rook was moved. Even
	// though most positions don't have castling flags, this seems
	// like the best way to do it. If the move being made is actually
	// castling, the update function explicitly removes the rook's
	// flag while this here will remove the king's.
	else {
		m_castling_flags &= ~getMask(bm.origin());
		return;
	}
}

SerialBoard Board::serialize() const
{
	SerialBoard sb;
	for(int i=0; i < Piece::LAST_TYPE; i++) {
		sb.pieces[i] = m_pieces[i];
	}
	sb.color[0] = m_color[0];
	sb.color[1] = m_color[1];

	sb.white_enpassant = (m_enpassant_flags >> 2*BOARDSIZE) & 0xFF;
	sb.black_enpassant = (m_enpassant_flags >> 5*BOARDSIZE) & 0xFF;
	sb.wk_castling = (m_castling_flags>>0) & (m_castling_flags>>3) & 1;
	sb.wq_castling = (m_castling_flags>>7) & (m_castling_flags>>3) & 1;
	sb.bk_castling = (m_castling_flags>>56) & (m_castling_flags>>59) & 1;
	sb.bq_castling = (m_castling_flags>>63) & (m_castling_flags>>59) & 1;
	return sb;
}

// Unsets all of the pieces bits, and the occupied bit for 'bp'
inline void Board::unsetAllBits(const BoardPosition & bp)
{
	unsigned long long mask = ~getMask(bp);

	// Unset all the piece bits
	for(int i = 0; i <= Piece::LAST_TYPE; i++)
		m_pieces[i] &= mask;

	for (int i = 0; i <= Piece::LAST_COLOR; i++)
		m_color[i] &= mask;
}

std::ostream& operator<< (std::ostream& os, const Board& b)
{
	for (int rank = 8; rank > 0; rank--) {
		for (char file = 'a'; file <= 'h'; file++) {
			os << b.isOccupied(BoardPosition(file, rank));
			if ('h' == file)
				os << endl;
		}
	}
	
	return os;
}

void Board::init()
{
	// Power of 2 array for computing current rank, file, and diag status
	pow2[0] = 1;
	for(int i = 1; i < BOARDSIZE; i++)
		pow2[i] = 2*pow2[i-1];
	
	// Initialize attack masks for non-sliding pieces
	for(int i = 0; i < BOARDSIZE*BOARDSIZE; i++) {

		BitBoard pawnW, pawnB, knight, king;
		
		// Initialize pawn attack masks
		pawnW.setBit(BoardPosition(i).NW());
		pawnW.setBit(BoardPosition(i).NE());
		pawnB.setBit(BoardPosition(i).SW());
		pawnB.setBit(BoardPosition(i).SE());
		pawnAttacks[Piece::WHITE][i] = pawnW.getBoard();
		pawnAttacks[Piece::BLACK][i] = pawnB.getBoard();
		
		// Initialize knight attack masks
		knight.setBit(BoardPosition(i).NW().N());
		knight.setBit(BoardPosition(i).NW().W());
		knight.setBit(BoardPosition(i).NE().N());
		knight.setBit(BoardPosition(i).NE().E());
		knight.setBit(BoardPosition(i).SW().S());
		knight.setBit(BoardPosition(i).SW().W());
		knight.setBit(BoardPosition(i).SE().S());
		knight.setBit(BoardPosition(i).SE().E());
		knightAttacks[i] = knight.getBoard();

		// Initialize king attack masks
		king.setBit(BoardPosition(i).N());
		king.setBit(BoardPosition(i).NW());
		king.setBit(BoardPosition(i).W());
		king.setBit(BoardPosition(i).SW());
		king.setBit(BoardPosition(i).S());
		king.setBit(BoardPosition(i).SE());
		king.setBit(BoardPosition(i).E());
		king.setBit(BoardPosition(i).NE());
		kingAttacks[i] = king.getBoard();
	}

	// Initialize all the masks for the sliding pieces (queens, rooks, bishops)
	unsigned long long state;
	for(int rank = 0; rank < BOARDSIZE; rank++) {
		for(int file = 0; file < BOARDSIZE; file++) {
			for(state = 0LL; state < 256LL; state++) {
				
				// Initialize the rank attack masks for sliding pieces
				rankAttacks[rank*BOARDSIZE + file][state] = 0LL;
				BoardPosition bp(file, rank);
				bool occupied = false;

				// Search to the east
				for(bp.moveE(); !bp.outE() && !occupied; bp.moveE()) {
					rankAttacks[rank*BOARDSIZE + file][state] |= getMask(bp);
					occupied = getMask(bp) & (state << rank*BOARDSIZE);
				}

				bp = BoardPosition(file, rank);
				occupied = false;

				// Search to the west
				for(bp.moveW(); !bp.outW() && !occupied; bp.moveW()) {
					rankAttacks[rank*BOARDSIZE + file][state] |= getMask(bp);
					occupied = getMask(bp) & (state << rank*BOARDSIZE);
				}

				// Initialize the file attack masks for sliding pieces
				fileAttacks[rank*BOARDSIZE + file][state] = 0LL;
				bp = BoardPosition(file, rank);
				occupied = false;

				// Search to the north
				for(bp.moveN(); !bp.outN() && !occupied; bp.moveN()) {
					fileAttacks[rank*BOARDSIZE + file][state] |= getMask(bp);
					occupied = 1LL & (state >> bp.rank0());
				}

				bp = BoardPosition(file, rank);
				occupied = false;

				// Search to the south
				for(bp.moveS(); !bp.outS() && !occupied; bp.moveS()) {
					fileAttacks[rank*BOARDSIZE + file][state] |= getMask(bp);
					occupied = 1LL & (state >> bp.rank0());
				}
                
				// Initialize the southeast diagonal attack masks
				diagAttacksSE[rank*BOARDSIZE + file][state] = 0LL;
				bp = BoardPosition(file, rank);
				occupied = false;
				int shift;

				// Search to the southeast
				for(bp.moveSE(); !bp.outSE() && ! occupied; bp.moveSE()) {
					diagAttacksSE[rank*BOARDSIZE + file][state] |= getMask(bp);
					shift = (rank+file < BOARDSIZE) ? bp.file0() : 7-bp.rank0();
					occupied = (state >> shift) & 1LL;
				}

				bp = BoardPosition(file, rank);
				occupied = false;
				
				// Search to the northwest
				for(bp.moveNW(); !bp.outNW() && ! occupied; bp.moveNW()) {
					diagAttacksSE[rank*BOARDSIZE + file][state] |= getMask(bp);
					shift = (rank+file < BOARDSIZE) ? bp.file0() : 7-bp.rank0();
					occupied = (state >> shift) & 1LL;
				}

				// Initialize the northeast diagonal attack masks
				diagAttacksNE[rank*BOARDSIZE + file][state] = 0LL;
				bp = BoardPosition(file, rank);
				occupied = false;

				// Search to the southwest
				for(bp.moveSW(); !bp.outSW() && ! occupied; bp.moveSW()) {
					diagAttacksNE[rank*BOARDSIZE + file][state] |= getMask(bp);
					shift = (rank < file) ? bp.rank0() : bp.rank0() - (rank-file);
					occupied = (state >> shift) & 1LL;
				}

				bp = BoardPosition(file, rank);
				occupied = false;
				
				// Search to the northeast
				for(bp.moveNE(); !bp.outNE() && ! occupied; bp.moveNE()) {
					diagAttacksNE[rank*BOARDSIZE + file][state] |= getMask(bp);
					shift = (rank < file) ? bp.rank0() : bp.rank0() - (rank-file);
					occupied = (state >> shift) & 1LL;
				}
			}
		}
	}
}

// End of file board.cpp
