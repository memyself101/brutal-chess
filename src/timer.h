// timer.h
// Timer
// Simple timer class
// by Joe Flint

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

/** Simple timer class
 *  Iterates a value over a range for a specified time period
 */
class Timer
{
	public:
		enum Type { LINEAR, LOGARITHMIC };
		
		Timer( Type t = LINEAR );

		void setType( Type t );
		
		void setRange( double s, double e );
		double startv() const;
		double end() const;

		void setProgress( double value );
		
		void setDuration( double dursecs );
		double duration() const;
		double timeElapsed() const;
		double timeLeft() const;
		
		void start();
		bool started();	
		void stop();
		bool done();
		void resetDone();
		void update();
		void operator++(int);
		
		double change() const;
		double value() const;

	protected:
		void calculateStep();

		Type type;
	
		bool starton, doneon;
		double startval, endval, step, val, lastval;
		double dur;
		int startTicks, lastTicks, endTicks;
		
};
#endif
