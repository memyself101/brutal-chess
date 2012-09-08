/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : chessgamestate.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "chessgamestate.h"

using namespace std;

inline bool compareBoards(SerialBoard, SerialBoard);

void ChessGameState::reset()
{
    m_50_moves = 0;
	m_check = false;
    m_threefold = false;
	m_turn_number = 1;
	m_white_turn = true;
	m_last_move = BoardMove();
	m_threefold_count.clear();
	m_board.reset();

	for(int rank = 1; rank <=8; rank++) {
		for(char file = 'a'; file <= 'h'; file++) {
			Piece::Color color= (rank < 3) ? Piece::WHITE : Piece::BLACK;
			Piece::Type type;
			BoardPosition bp(file, rank);
			
			if(rank >= 3 && rank <= 6) {
				m_pieces[bp.hash()] = NULL;
				break;
			} else if(rank == 2 || rank == 7) {
				type = Piece::PAWN;
			} else if(file == 'a' || file == 'h') {
				type = Piece::ROOK;
			} else if(file == 'b' || file == 'g') {
				type = Piece::KNIGHT;
			} else if(file == 'c' || file == 'f') {
				type = Piece::BISHOP;
			} else if(file == 'd') {
				type = Piece::QUEEN;
			} else {
				type = Piece::KING;
			}

			Piece* piece = new Piece(color, type);
			m_pieces[bp.hash()] = piece;
			m_board.addPiece(piece, bp);
		}
	}
}

bool ChessGameState::isDraw()
{
    return  m_board.isStaleMate(this->getTurn()) ||
            m_board.isMaterialDraw() ||
            m_50_moves > 99 ||
            m_threefold;
}

// Change this function to return enum about the state of the board
// i.e. check, checkmate, move out of check,stalemate draw, 50 move draw, 
// normal, etc... used by chessgame to push user events 
// onto the stack to handle the various animations and endgame scenarios.
void ChessGameState::update(const BoardMove& bm)
{
	SerialBoard sb;
	
	if(m_turn_number > 1 && m_last_move.needPromotion()) {
		return;
	}

	// Handle the start of a game
	if(m_turn_number == 1 && m_white_turn) {
		sb = m_board.serialize();
		sb.white_turn = 1;
		m_threefold_count.push_back(make_pair(sb, 1));
	}

	if(m_board.getPiece(bm.origin())->type() != Piece::PAWN ||
	   m_board.isOccupied(bm.dest())) {
		m_50_moves++;
	} else {
		m_50_moves = 0;
		m_threefold_count.clear();
	}

	m_board.update(bm);
	m_last_move = bm;
	m_white_turn = !m_white_turn;

	Piece::Color color = bm.getPiece()->color();
	Piece::Type type = bm.getPiece()->type();

	if(type == Piece::PAWN && bm.fileDiff() == bm.rankDiff() && !m_pieces[bm.dest().hash()]) {
		// EnPassant capture
		if(color == Piece::WHITE) {
			m_pieces[bm.dest().S().hash()] = NULL;
		} else {
			m_pieces[bm.dest().N().hash()] = NULL;
		}
	} else if(type == Piece::KING && bm.signedFileDiff() == -2) {
		// Castling to the left
		BoardPosition corner = (color == Piece::WHITE) ?
			BoardPosition('a',1) : BoardPosition('a',8);
		m_pieces[bm.origin().W().hash()] = m_pieces[corner.hash()];
		m_pieces[corner.hash()] = NULL;
	} else if(type == Piece::KING && bm.signedFileDiff() == 2) {
		// Castling to the right
		BoardPosition corner = (color == Piece::WHITE) ?
			BoardPosition('h',1) : BoardPosition('h',8);
		m_pieces[bm.origin().E().hash()] = m_pieces[corner.hash()];
		m_pieces[corner.hash()] = NULL;
	}
	
	m_pieces[bm.dest().hash()] = m_pieces[bm.origin().hash()];
	m_pieces[bm.dest().hash()]->promotePawn(bm.getPromotion());
	m_pieces[bm.origin().hash()] = NULL;
	
	m_check = m_board.isCheck(getTurn());

	// Update the threefold repetiiton counter
	int i = 0;
	sb = m_board.serialize();
	sb.white_turn = m_white_turn ? 1 : 0;

	for(; i < (int)m_threefold_count.size(); i++) {
		if(compareBoards(sb, m_threefold_count[i].first)) {
			m_threefold_count[i].second++;
			if(m_threefold_count[i].second >= 3) {
				m_threefold = true;
			}
			break;
		}
	}

	if(i == m_threefold_count.size()) {
		m_threefold_count.push_back(make_pair(sb, 1));
	}

	cout << m_turn_number << ". " << bm.origin() << " " << bm.dest() << endl;

	if(m_white_turn) {
		m_turn_number++;
	}
}

bool ChessGameState::isPositionSelectable(const BoardPosition& bp) const
{
	if(!bp.isValid())
		return false;
	if(!m_board.isOccupied(bp))
		return false;
	if(getTurn() != m_board.getPiece(bp)->color())
		return false;
	return true;
}

inline bool compareBoards(SerialBoard x, SerialBoard y) {
	for(int i=0; i < Piece::LAST_TYPE; i++) {
		if(x.pieces[i] != y.pieces[i]) {
			return false;
		}
	}

	if((x.color[0] != y.color[0]) || (x.color[1] != y.color[1])) {
		return false;
	}

	return x.all_flags == y.all_flags;
}

// end of file chessgamestate.cpp
