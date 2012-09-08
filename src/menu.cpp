/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : menu.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "fontloader.h"
#include "menu.h"
#include "menuitem.h"
#include "SDL_opengl.h"

#include <string>
#include <iostream>

using namespace std;

// Pushes a matrix to let us draw in x-y coordinates
void push2D()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Pops the matrix allowing us to draw in x-y coordinates so that we may
// once again draw in x-y-z coordinates.
void pop2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// Deletes all of the MenuItem pointers in the Menu.
Menu::~Menu()
	{;}// for_each(m_item_set.begin(); m_item_set.end(); Delete()); }

void Menu::addMenuItem(const string & opSet, MenuItem * menu_item_ptr)
{
	m_item_sets[opSet].push_back(menu_item_ptr);
	menu_item_ptr->setParent(this);
}

// Draws this Menu
void Menu::draw()
{
	push2D();

	glDisable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	if(m_state == MENUIN || m_state == MENUOUT) {
		double d = m_scaletimer.value();
		glScaled(d, d, 1.0);
		draw(m_alphatimer.value());
		m_alphatimer++;
		m_scaletimer++;
		if(m_alphatimer.done())
			m_state = (m_state == MENUIN) ? ACTIVE : INACTIVE;
	} else {
		draw(1.0);
	}

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	pop2D();
}

// Draws this menu with respect to the specified alpha value.
void Menu::draw(const float a)
{
	glColor4f(1.0, 1.0, 1.0, 0.75*a);
	glBegin(GL_QUADS);
		glVertex2f(0.1, 0.2);
		glVertex2f(0.9, 0.2);
		glVertex2f(0.9, 0.9);
		glVertex2f(0.1, 0.9);
	glEnd();

	glTranslated(0.13, 0.83, 0.0);
	if(m_state == SUBMENUIN || m_state == SUBMENUOUT) {
		drawHeader(m_last_set, 1.0 - a*m_alphatimer.value());
		drawHeader(m_curr_set, a*m_alphatimer.value());

		if(m_state == SUBMENUIN) {
			glTranslated(-m_slidetimer.value(), 0, 0);
			drawSet(m_last_set, 1.0 - a*m_alphatimer.value());
			glTranslated(m_slidetimer.value(), 0, 0);

			glTranslated(m_slidetimer.end() - m_slidetimer.value(), 0, 0);
			drawSet(m_curr_set, a*m_alphatimer.value());
			glTranslated(-(m_slidetimer.end() - m_slidetimer.value()), 0, 0);
		} else if(m_state == SUBMENUOUT) {
			glTranslated(m_slidetimer.value(), 0, 0);
			drawSet(m_last_set, 1.0 - a*m_alphatimer.value());
			glTranslated(-m_slidetimer.value(), 0, 0);

			glTranslated(-(m_slidetimer.end() - m_slidetimer.value()), 0, 0);
			drawSet(m_curr_set, a*m_alphatimer.value());
			glTranslated(m_slidetimer.end() - m_slidetimer.value(), 0, 0);
		}

		m_slidetimer++;
		m_alphatimer++;
		if(m_alphatimer.done())
			m_state = ACTIVE;
	} else {
		drawHeader(m_curr_set, a);
		drawSet(m_curr_set, a);
	}

}

// Draws the header
void Menu::drawHeader(ItemSet_t::const_iterator set, float a)
{
	glColor4f(0.0, 0.0, 0.0, 0.75*a);
	glScaled(1/800.0, 1/800.0, 1/800.0);
    FontLoader::print(0.0, 0.0, m_header.c_str());
	glScaled(800.0, 800.0, 800.0);
}

// Draws the current ItemSet
void Menu::drawSet(ItemSet_t::const_iterator set, float a)
{
	glPushMatrix();
	glTranslated(0.075, 0.0, 0.0);
	vector<MenuItem*>::const_iterator mii;
	for (mii = (*set).second.begin(); mii != (*set).second.end(); mii++) {
        if ((*mii)->isCollapsed()) {
            continue;
        }
		glTranslated(0.0, -0.08, 0.0);	
		glColor4f(0.0, 0.0, 0.0, 0.5*a);
		(*mii)->draw(1.0*a);		
	}
	glPopMatrix();
}

/** Handles events when the Menu is enabled */
bool Menu::handleEvent(SDL_Event& e)
{
	if(m_state != INACTIVE && e.type == SDL_USEREVENT) {
		if(e.user.code == eBACK) {
			if (m_sethistory.size() == 1)
				deactivate();
			else
				popOptionsSet();
			return true;
		}
	}
	if(m_state != ACTIVE)
		return false;
	if(e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				// If we're in a sub-menu just go back.
				if (m_sethistory.size() != 1 && m_state == ACTIVE) {
					SDL_Event backevent;
					backevent.type = SDL_USEREVENT;
					backevent.user.code = eBACK;
					SDL_PushEvent(&backevent);
					return true;
				}
			}
			else if (e.key.keysym.sym == SDLK_RETURN) {
				// Don't act on the keypress unless the menu is active.
				if (m_state != ACTIVE) {
					return false;
				}
				(*m_curr_menuitem)->onSelect();
				return true;
			}
			else if (e.key.keysym.sym == SDLK_DOWN) {
                if (m_curr_menuitem != (*m_curr_set).second.end())
				    (*m_curr_menuitem)->hoverOut();
				nextMenuItem();
				if (dynamic_cast<SeparatorItem*>(*m_curr_menuitem))
					nextMenuItem();
				(*m_curr_menuitem)->hoverIn();
				return true;
			}
			else if (e.key.keysym.sym == SDLK_UP) {
                if (m_curr_menuitem != (*m_curr_set).second.end())
				    (*m_curr_menuitem)->hoverOut();
				prevMenuItem();
				if (dynamic_cast<SeparatorItem*>(*m_curr_menuitem))
					prevMenuItem();
				(*m_curr_menuitem)->hoverIn();
				return true;
			}
	}
	else if (e.type == SDL_MOUSEMOTION
					&& e.motion.state == 0) {
	    vector<MenuItem*>::const_iterator mii = getMousePointedMenuItem(e);
        if (mii == m_curr_menuitem || mii == (*m_curr_set).second.end())
            return true;
        (*m_curr_menuitem)->hoverOut();
        m_curr_menuitem = mii;
        (*m_curr_menuitem)->hoverIn();
		return true;
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        vector<MenuItem*>::const_iterator mii = getMousePointedMenuItem(e);
        if (mii != (*m_curr_set).second.end()) {
		    (*m_curr_menuitem)->onSelect();	
        }
		return true;
	}
	return false;
}

// Gets the MenuItem currently pointed to by the mouse.  If no MenuItem
// is pointed to (or we're pointing to a separator), returns iterator 
// to the end of the current options set.
vector<MenuItem*>::const_iterator Menu::getMousePointedMenuItem(const SDL_Event & e)
{
    // Get the mouse position
	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	float mousex = e.motion.x/viewport[2];
	
    // Just return if we are off to the sides of the menu.
	if (mousex < m_xoffset + 0.075 || mousex > m_xoffset + m_width + 0.075)
        return (*m_curr_set).second.end();

	float mousey = 1.0 - e.motion.y/viewport[3];
        
    // Figure out which menu item we're pointing at
    vector<MenuItem*>::const_iterator mii;
    float accumulatedOffset = 1.0 - (m_yoffset - 0.03f);
    for (mii = (*m_curr_set).second.begin(); mii != (*m_curr_set).second.end(); ++mii) {

        // If the menu item is "collapsed", we shouldn't be able to click it.
        if ((*mii)->isCollapsed()) {
            continue;
        }

        accumulatedOffset += (*mii)->spacing();
        float bottom = 1.0 - accumulatedOffset;
        if (mousey > bottom && mousey < bottom + (*mii)->height()) {
            if (!dynamic_cast<SeparatorItem*>(*mii))
                return mii;
        }
    }
    return (*m_curr_set).second.end();
}

// Sets the desired options set.
void Menu::setOptionsSet(const string & setName)
{
	m_curr_set = m_item_sets.find(setName);
	m_curr_menuitem = (*m_curr_set).second.begin();
	(*m_curr_menuitem)->hoverIn();
    m_header = setName;
}

// Pushes an option set on to the set history stack so we know where we
// came from.
void Menu::pushOptionsSet(const string & setName)
{
	m_last_set = m_curr_set;

	if(m_state == ACTIVE) {
        if (m_curr_menuitem != (*m_curr_set).second.end())
            (*m_curr_menuitem)->hoverOut();
		m_state = SUBMENUIN;
		m_alphatimer = Timer(Timer::LOGARITHMIC);
		m_alphatimer.setRange(0.0, 1.0);
		m_alphatimer.setDuration(0.2);
		m_alphatimer.start();

		m_slidetimer = Timer(Timer::LOGARITHMIC);
		m_slidetimer.setRange(0.0, 0.02);
		m_slidetimer.setDuration(0.2);
		m_slidetimer.start();
	}

	setOptionsSet(setName);
	m_sethistory.push(setName);
	

}

// Pops an option set off the set history stack.
void Menu::popOptionsSet()
{
	m_last_set = m_curr_set;
    if (m_curr_menuitem != (*m_curr_set).second.end())
	    (*m_curr_menuitem)->hoverOut();

	if(m_state == ACTIVE) {
		m_state = SUBMENUOUT;
		m_alphatimer = Timer(Timer::LOGARITHMIC);
		m_alphatimer.setRange(0.0, 1.0);
		m_alphatimer.setDuration(0.2);
		m_alphatimer.start();

		m_slidetimer = Timer(Timer::LOGARITHMIC);
		m_slidetimer.setRange(0.0, 0.02);
		m_slidetimer.setDuration(0.2);
		m_slidetimer.start();
	}

	m_sethistory.pop();
	setOptionsSet(m_sethistory.top());
}

// Sets the Menu's state to MENUIN and starts the entry animation.
void Menu::activate()
{
	m_state = MENUIN;
	m_alphatimer = Timer(Timer::LOGARITHMIC);
	m_alphatimer.setRange(0.0, 1.0);
	m_alphatimer.setDuration(0.2);
	m_alphatimer.start();

	m_scaletimer = Timer(Timer::LOGARITHMIC);
	m_scaletimer.setRange(0.9, 1.0);
	m_scaletimer.setDuration(0.2);
	m_scaletimer.start();
}

// Sets the Menu's state to MENUOUT and starts the exit animation.
void Menu::deactivate()
{
	m_state = MENUOUT;
	m_alphatimer = Timer(Timer::LOGARITHMIC);
	m_alphatimer.setRange(1.0, 0.0);
	m_alphatimer.setDuration(0.20);
	m_alphatimer.start();

	m_scaletimer = Timer(Timer::LOGARITHMIC);
	m_scaletimer.setRange(1.0, 0.9);
	m_scaletimer.setDuration(0.20);
	m_scaletimer.start();

}

// Iterates to the next menu item, wraps around to the beginning
// when it reaches the end.
void Menu::nextMenuItem()
{
	m_curr_menuitem++;
	if (m_curr_menuitem == (*m_curr_set).second.end())
		m_curr_menuitem = (*m_curr_set).second.begin();
}

// Iterates backwards to the previous menu item, wraps around to
// the end when it reaches the beginning.
void Menu::prevMenuItem()
{
	if (m_curr_menuitem == (*m_curr_set).second.begin())
		m_curr_menuitem = (*m_curr_set).second.end();
	m_curr_menuitem--;
}

// End of file menu.cpp

