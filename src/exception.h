/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : exception.h
 * Authors : Mike Cook, Joe Flint
 **************************************************************************/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>

class Exception : public std::exception {
 public:
	Exception(const char * msg) : m_msg(msg) {}
	
	virtual const char * what()
		{ return m_msg; }
		
 private:
	const char * m_msg;
};

#endif // EXCEPTION_H

// End of file exception.h
