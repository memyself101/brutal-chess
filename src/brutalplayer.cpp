/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : brutalplayer.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "board.h"
#include "chessplayer.h"
#include "options.h"

#include <vector>
#include <time.h>
#include <climits>

using namespace std;

BrutalPlayer::BrutalPlayer()
{
    m_ply = Options::getInstance()->brutalplayer2ply;
	m_trustworthy = true;
	srand(time(NULL));
}

void BrutalPlayer::think(const ChessGameState & cgs)
{
	BoardMove move;
    Board board = cgs.getBoard();
	search(board, getColor(), m_ply, -INT_MAX, INT_MAX, move);
	m_move = move;
}

int BrutalPlayer::search(Board board, Piece::Color color, int depth, int alpha, int beta, BoardMove& move)
{
	BoardMove testMove;
	Board testBoard = board;
	int moveScore, bestScore = -INT_MAX;
	vector<BoardMove> moves = board.possibleMoves(color);

	bool gotmove = false;

	for(int i=0; i < moves.size(); i++) {
		if(!board.isMoveLegal(moves[i])) {
			continue;
		}

		if(!gotmove) {
			move = moves[i];
			gotmove = true;
		}

		testBoard = board;
		testBoard.update(moves[i]);
	
        if(depth == 0) {
			moveScore = evaluateBoard(testBoard, color);
		} else {
			moveScore = -search(testBoard, Piece::opposite(color), depth-1, -beta, -alpha, testMove);
		}

        if(moveScore > bestScore) {
			bestScore = moveScore;
			move = moves[i];
        }
        if(bestScore > alpha) {
			alpha = bestScore;
		}
        if(alpha >= beta) {
			return beta;
		}
	}
	
	return bestScore;
}

int BrutalPlayer::evaluateBoard(const Board & board, Piece::Color turn)
{
    vector< vector<BoardPosition> > locations(Piece::LAST_TYPE+1);
    
    // Raw material balance
    int balance = 0;
    int endgamecount = 0;
            
    int queenval = 900, rookval = 500, bishopval = 325, knightval = 310, pawnval = 100, kingval = 100000;
    
    for(int i=0; i < 64; i++) {
        BoardPosition bp = BoardPosition(i);
        if(board.isOccupied(bp)) {
            Piece *p = board.getPiece(bp);
            switch(p->type()) {
                case Piece::QUEEN:
                    locations[Piece::QUEEN].push_back(bp);
                    balance += (p->color() == turn) ? queenval : -queenval;
                    endgamecount += queenval;
                    break;
                case Piece::ROOK:
                    locations[Piece::ROOK].push_back(bp);
                    balance += (p->color() == turn) ? rookval : -rookval;
                    endgamecount += rookval;
                    break;
                case Piece::BISHOP:
                    locations[Piece::BISHOP].push_back(bp);
                    balance += (p->color() == turn) ? bishopval : -bishopval;
                    endgamecount += bishopval;
                    break;
                case Piece::KNIGHT:
                    locations[Piece::KNIGHT].push_back(bp);
                    balance += (p->color() == turn) ? knightval : -knightval;
                    endgamecount += knightval;
                    break;
                case Piece::PAWN:
                    locations[Piece::PAWN].push_back(bp);
                    balance += (p->color() == turn) ? pawnval : -pawnval;
                    endgamecount += pawnval;
                    break;
                case Piece::KING:
                    locations[Piece::KING].push_back(bp);
                    if ((p->color() == turn) && !board.isCheckMate(turn)) {
                        balance += kingval;
                    }
                    else if ((p->color() != turn) && !board.isCheckMate(turn)) {
                        balance -= kingval;
                    }
                    break;
            }
        }
    }

    // Determine end game or not.  THIS IS A TWEAKABLE VALUE.
    bool endgame = false;
    if (endgamecount < 3500) {
        endgame = true;
    }

    for(int i=0; i < locations[Piece::QUEEN].size(); i++) {
        balance += queenBonus(locations[Piece::QUEEN][i], board, turn, endgame);
    }
    for(int i=0; i < locations[Piece::ROOK].size(); i++) {
        balance += rookBonus(locations[Piece::ROOK][i], board, turn, endgame);
    }
    for(int i=0; i < locations[Piece::BISHOP].size(); i++) {
        balance += bishopBonus(locations[Piece::BISHOP][i], board, turn, endgame);
    }
    for(int i=0; i < locations[Piece::KNIGHT].size(); i++) {
        balance += knightBonus(locations[Piece::KNIGHT][i], board, turn, endgame);
    }
    for(int i=0; i < locations[Piece::PAWN].size(); i++) {
        balance += pawnBonus(locations[Piece::PAWN][i], board, turn, endgame);
    }
    for(int i=0; i < locations[Piece::KING].size(); i++) {
        balance += kingBonus(locations[Piece::KING][i], board, turn, endgame);
    }

    return balance;
}

// Queen Bonuses are finished, queens get bonuses for being near an opposing king.
int BrutalPlayer::queenBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
	if (!endgame) {
        return 0;
	}

    Piece * p = board.getPiece(bp);
    int bonus = 0;
	BoardMove dist;
    
    if (p->color() == Piece::WHITE) {
        dist = BoardMove(bp, board.getKing(Piece::BLACK), p);
	}
	else {
		dist = BoardMove(bp, board.getKing(Piece::WHITE), p);
	}

	bonus = -2*abs(dist.fileDiff()+dist.rankDiff());

    return (turn == p->color()) ? bonus : -bonus;
}

int BrutalPlayer::rookBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
    int bonus = 0;

	// Rooks get a bonus of 0 points if blocked, or up to 20 points if
	// attacking 12 squares or more.
	unsigned long long rookAttacks = Board::rankAttacks[bp.hash()][board.getRankState(bp)] ||
                                     Board::fileAttacks[bp.hash()][board.getFileState(bp)];
    int numAttacked = numAttackedSquares(rookAttacks);
    bonus += (numAttacked < 12) ? 2*numAttacked-4 : 20;

    return bonus;
}


int BrutalPlayer::bishopBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
    Piece::Color color = board.getPiece(bp)->color();
    int bonus = m_bishop[bp.hash()];

    return (turn == color) ? bonus: -bonus;
}


int BrutalPlayer::knightBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
    Piece::Color color = board.getPiece(bp)->color();
    int bonus = m_knight[bp.hash()];

	bool drivenBonus = false;

	// Knights are given a bonus for being within 2 squares of each enemy piece.
	

    return (turn == color) ? bonus : -bonus;
}

int BrutalPlayer::pawnBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
    Piece::Color color = board.getPiece(bp)->color();
    int bonus = (color == Piece::WHITE) ? m_wpawn[bp.hash()] : m_bpawn[bp.hash()];

	if(isIsolatedPawn(bp, board)) {
        char file = bp.filec();
        if(file == 'a' || file == 'h') {
            bonus -= 12;
        } else if (file == 'b' || file == 'g') {
            bonus -= 14;
        } else if (file == 'c' || file == 'f') {
            bonus -= 16;
        } else if (file == 'd' || file == 'e') {
            bonus -= 20;
        }
    }
	/*
    if(isDoubledPawn(bp, board)) {
    }
	*/
    return (turn == color) ? bonus : -bonus;
}

int BrutalPlayer::kingBonus(const BoardPosition & bp, const Board & board, Piece::Color turn, bool endgame) {
    Piece::Color color = board.getPiece(bp)->color();
    int bonus = 0;

    // Count all pieces attacking the king
    unsigned long long attacked = board.isAttacked(bp, color);
    for(int i=0; i < 64; i++) {
        bonus -= 25*((attacked >> i) & 1);
    }
    
    if(endgame) {
        bonus = m_end_king[bp.hash()];
    } else {
        bonus = (color == Piece::WHITE) ? m_wking[bp.hash()] : m_bking[bp.hash()];
    }
    
    return (turn == color) ? bonus : -bonus;
}

int BrutalPlayer::numAttackedSquares(const unsigned long long & pieceAttacks)
{
	int numSquares = 0;
	for (int i = 0; i < 64; i++) {
		if (pieceAttacks && (1LL << i)) {
			numSquares++;
		}
	}
	return numSquares;
}

bool BrutalPlayer::isIsolatedPawn(const BoardPosition & bp, const Board & board)
{
        Piece::Color c = board.getPiece(bp)->color();
	unsigned long long mask = board.m_color[c] & board.m_pieces[Piece::PAWN];

	BoardPosition test = bp.E();
	if(test.isValid() && (mask & maskFile(test))) {
		return false;
	}

	test = bp.W();
	if(test.isValid() && (mask & maskFile(test))) {
		return false;
	}

	return true;
}

bool BrutalPlayer::isDoubledPawn(const BoardPosition & bp, const Board & board)
{
	return false;
}

// Positional Bonuses
int BrutalPlayer::m_bishop[64] = {
    -5,-5,-5,-5,-5,-5,-5,-5,
    -5,10,5,10,10,5,10,-5,
    -5,5,3,12,12,3,5,-5,
    -5,3,12,3,3,12,3,-5,
    -5,3,12,3,3,12,3,-5,
    -5,5,3,12,12,3,5,-5,
    -5,10,5,10,10,5,10,-5,
    -5,-5,-5,-5,-5,-5,-5,-5 };

int BrutalPlayer::m_knight[64] = {
    -10,-5,-5,-5,-5,-5,-5,-10,
    -5,0,0,3,3,0,0,-5,
    -5,0,5,5,5,5,0,-5,
    -5,0,5,10,10,5,0,-5,
    -5,0,5,10,10,5,0,-5,
    -5,0,5,5,5,5,0,-5,
    -5,0,0,3,3,0,0,-5,
    -10,-5,-5,-5,-5,-5,-5,-10 };

int BrutalPlayer::m_wpawn[64] = {
    0,0,0,0,0,0,0,0,
    0,0,0,-5,-5,0,0,0,
    1,2,3,4,4,3,2,1,
    2,4,6,8,8,6,4,2,
    3,6,9,12,12,9,6,3,
    4,8,12,16,16,12,8,4,
    5,10,15,20,20,15,10,5,
    0,0,0,0,0,0,0,0 };

int BrutalPlayer::m_bpawn[64] = {
    0,0,0,0,0,0,0,0,
    5,10,15,20,20,15,10,5,
    4,8,12,16,16,12,8,4,
    3,6,9,12,12,9,6,3,
    2,4,6,8,8,6,4,2,
    1,2,3,4,4,3,2,1,
    0,0,0,-5,-5,0,0,0,
    0,0,0,0,0,0,0,0 };

int BrutalPlayer::m_wking[64] = {
    2,10,4,0,0,7,10,2,
    -3,-3,-5,-5,-5,-5,-3,-3,
    -5,-5,-8,-8,-8,-8,-5,-5,
    -8,-8,-13,-13,-13,-13,-8,-8,
    -13,-13,-21,-21,-21,-21,-13,-13,
    -21,-21,-34,-34,-34,-34,-21,-21,
    -34,-34,-55,-55,-55,-55,-34,-34,
    -55,-55,-89,-89,-89,-89,-55,-55};

int BrutalPlayer::m_bking[64] = {
    -55,-55,-89,-89,-89,-89,-55,-55,
    -34,-34,-55,-55,-55,-55,-34,-34,
    -21,-21,-34,-34,-34,-34,-21,-21,
    -13,-13,-21,-21,-21,-21,-13,-13,
    -8,-8,-13,-13,-13,-13,-8,-8,
    -5,-5,-8,-8,-8,-8,-5,-5,
    -3,-3,-5,-5,-5,-5,-3,-3,
    2,10,4,0,0,7,10,2};

int BrutalPlayer::m_end_king[64] = {
    -5,-3,-1,0,0,-1,-3,-5,
    -3,5,5,5,5,5,5,-3,
    -1,5,10,10,10,10,5,-1,
    0,5,10,15,15,10,5,0,
    0,5,10,15,15,10,5,0,
    -1,5,10,10,10,10,5,-1,
    -3,5,5,5,5,5,5,-3,
    -5,-3,-1,0,0,-1,-3,-5};

// end of file brutalplayer.cpp

