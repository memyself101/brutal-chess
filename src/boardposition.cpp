/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : boardposition.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "boardposition.h"
#include "board.h"
#include <iostream>

BoardPosition::BoardPosition(const char & file, const int & rank)
: m_file0(file-'a'), m_rank0(rank-1)
{}


bool BoardPosition::isValid() const
{
	return (m_file0 >= 0) && (m_rank0 >= 0) && (m_file0 < Board::BOARDSIZE) &&
				(m_rank0 < Board::BOARDSIZE);
}

const BoardPosition BoardPosition::N() const
{
	return BoardPosition(m_file0, m_rank0 + 1);
}

const BoardPosition BoardPosition::NE() const
{
	return BoardPosition(m_file0 + 1, m_rank0 + 1);
}

const BoardPosition BoardPosition::E() const
{
	return BoardPosition(m_file0 + 1, m_rank0);
}

const BoardPosition BoardPosition::SE() const
{
	return BoardPosition(m_file0 + 1, m_rank0 - 1);
}

const BoardPosition BoardPosition::S() const
{
	return BoardPosition(m_file0, m_rank0 - 1);
}

const BoardPosition BoardPosition::SW() const
{
	return BoardPosition(m_file0 - 1, m_rank0 - 1);
}

const BoardPosition BoardPosition::W() const
{
	return BoardPosition(m_file0 - 1, m_rank0);
}

const BoardPosition BoardPosition::NW() const
{
	return BoardPosition(m_file0 - 1, m_rank0 + 1);
}

const bool BoardPosition::outN() const
{
	return (m_rank0 >= Board::BOARDSIZE);
}

const bool BoardPosition::outNE() const
{
	return (m_rank0 >= Board::BOARDSIZE || m_file0 >= Board::BOARDSIZE);
}

const bool BoardPosition::outE() const
{
	return (m_file0 >= Board::BOARDSIZE);
}

const bool BoardPosition::outSE() const
{
	return (m_rank0 < 0 || m_file0 >= Board::BOARDSIZE);
}

const bool BoardPosition::outS() const
{
	return (m_rank0 < 0);
}

const bool BoardPosition::outSW() const
{
	return (m_rank0 < 0 || m_file0 < 0);
}

const bool BoardPosition::outW() const
{
	return (m_file0 < 0);
}

const bool BoardPosition::outNW() const
{
	return (m_rank0 >= Board::BOARDSIZE || m_file0 < 0);
}

void BoardPosition::moveN()
{
	m_rank0++; 
}

void BoardPosition::moveNE()
{
	m_rank0++;
	m_file0++;
}

void BoardPosition::moveE()
{
	m_file0++; 
}

void BoardPosition::moveSE()
{
	m_rank0--;
	m_file0++;
}

void BoardPosition::moveS()
{
	m_rank0--; 
}

void BoardPosition::moveSW()
{
	m_rank0--;
	m_file0--;
}

void BoardPosition::moveW()
{ 
	m_file0--; 
}
	
void BoardPosition::moveNW()
{
	m_rank0++;
	m_file0--;
}	

bool operator== (const BoardPosition& bp1, const BoardPosition& bp2)
{
	return (bp1.m_file0 == bp2.m_file0) && (bp1.m_rank0 == bp2.m_rank0);
}

bool operator!= (const BoardPosition& bp1, const BoardPosition& bp2)
{
	return (bp1.m_file0 != bp2.m_file0) || (bp1.m_rank0 != bp2.m_rank0);
}

std::ostream& operator<< (std::ostream& os, const BoardPosition & bp)
{
	os << (char)(bp.m_file0+'a') << bp.m_rank0+1;
	return os;	
}

// End of file boardposition.cpp
