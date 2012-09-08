#include <SDL.h>
#include <iostream>
#include <math.h>
#include "timer.h"

using namespace std;

Timer::Timer( Type t )
:type(t)
{
	starton = false;
	doneon = false;
}

void Timer::setType( Type t )
{
	type = t;
}

/** Set the value range.
 *
 *  @param s Start value of the range
 *  @param e End value of the range
 */
void Timer::setRange( double s, double e )
{
	// Parameters can not be modified while the timer is running
	if( starton )
		return;
	
	startval = s;
	endval = e;
}

/** Returns the start value of the range.
 *
 */
double Timer::startv() const
{
	return startval;
}

/** Returns the end value of the range.
 *
 */
double Timer::end() const
{
	return endval;
}

/** Sets the progress partway through the timer
 *
 */
void Timer::setProgress( double progval )
{
	if( starton )
		return;

	dur *= ( endval - progval ) / ( endval - startval );
	startval = progval;
}

/** Set the duration of the timer.
 *
 *  @param dursecs The amount of time the timer should run, in seconds
 */
void Timer::setDuration( double dursecs )
{
	// Parameters can not be modified while the timer is running
	if( starton )
		return;
	
	dur = dursecs;
}

/** Returns the duration of the timer in seconds.
  *
  */
double Timer::duration() const
{
	return dur;
}

/** Returns how much time has elapsed.
  * Returns the number of seconds the timer has been running.
  */
double Timer::timeElapsed() const
{
	return ( lastTicks - startTicks ) / 1000.0;
}

/** Returns how much time is left.
  * Returns the number of seconds the timer has until completion.
  */
double Timer::timeLeft() const
{
	return ( endTicks - lastTicks ) / 1000.0;
}

/** Start the timer.
  *
  */
void Timer::start()
{
	// Can't start something twice
	if( starton )
		return;
	
	// Store the starting time
	startTicks = SDL_GetTicks();
	lastTicks = startTicks;
	endTicks = int( dur * 1000 ) + startTicks;
	
	// Initialize the current and previous values
	val = startval;
	lastval = startval;
	
	// See how much the value changes by for each tick
	calculateStep();
	
	starton = true;
	doneon = false;
}

bool Timer::started()
{
	return starton;
}

void Timer::stop()
{
	starton = false;
}

bool Timer::done()
{
	return doneon;
}

void Timer::resetDone()
{
	doneon = false;
}

/** Update the timer.
 *  Modifies the value to reflect the time elapsed.
 */
void Timer::update()
{
	// Timer must be running to update
	if( !starton )
		return;
	
	lastval = val;
	
	int ticksChange = SDL_GetTicks() - lastTicks;
	
	lastTicks += ticksChange;

	// As long as this isn't the end of the timer
	if( lastTicks < endTicks ) {
	 	val += step * ticksChange;
		if( type == LOGARITHMIC ) {
			val = ((endval - startval)/log(11.0))*log(10*double(lastTicks - startTicks)/
					(endTicks - startTicks) + 1.0) + startval;
		}
		
	} else {
	// If this is the end, just update to the end value
		val = endval;
		lastval = endval;
		starton = false;
		doneon = true;
	}
	
	
}

/** Update the timer.
 *
 */
void Timer::operator++(int)
{
	update();
}

/** Returns the change in the value since the last update.
 *
 */
double Timer::change() const
{
	// No change if the timer isn't running
	if( !starton )
		return 0;
	
	return val - lastval;
}

/** Returns the current value within the range.
  * If the timer isn't running, returns the start of the range.
  */
double Timer::value() const
{
	// Still at the start if not running
	if( !starton )
		return startval;
	
	return val;
}

void Timer::calculateStep()
{
	step = endval - startval;
	
	// Prevent divide by zero
	if( dur != 0 )
		step /= 1000 * dur;
	else
		step = 0;
}
