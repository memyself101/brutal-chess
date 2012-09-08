/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : menu.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef MENU_H
#define MENU_H

#include <map>
#include <stack>
#include <string>
#include <vector>

#include "SDL.h"
#include "timer.h"

class MenuItem;

/**
 * Represents a titled Menu composed of many MenuItems.
 */
class Menu {
 public:

    /** Constructs an inactive Menu with the specified offsets. */
	Menu() :
		m_state(INACTIVE),
        m_width(0.6),
		m_xoffset(0.13),
		m_yoffset(0.83) {}

	/** Deletes all of the MenuItem pointers in the Menu */
	~Menu();

	/** Adds a MenuItem to the Menu. */
	void addMenuItem(const std::string & opSet, MenuItem * menu_item_ptr);

    /** Handles events when the Menu is enabled */
	bool handleEvent(SDL_Event& e);

	/** Renders the Menu to the screen. */
	void draw();

	/** Sets the desired options set */
	void setOptionsSet(const std::string & setName);

    /*
     * Pushes an option set on to the set history stack so we know where
     * we came from.
     */
    void pushOptionsSet(const std::string & setName);
	
    /** Returns true if the Menu is in the ACTIVE state. */
	bool isActive()
		{ return m_state != Menu::INACTIVE; }

	/** Sets the Menu's state to MENUIN and starts the entry animation. */
	void activate();

	/** Sets the Menu's state to MENUOUT and starts the exit animation. */
	void deactivate();

    /** Sets the header message that should be drawn for the current itemset. */
    void setHeader(const std::string& header) 
        { m_header = header; }

    /** Event codes thrown by MenuItems */
	enum MENU_EVENT_CODES {
		eAPPLYRES = 1,
		eBACK,
        eBBRUTALPLYCHANGED,
        eBLACKPLAYERCHANGED,
        eFULLSCREENTOG,
        eHISTORYARROWSTOG,
		eQUIT,
        eREFLECTTOG,
		eSHADOWTOG,
        eSTARTNEWGAME,
        eWBRUTALPLYCHANGED,
        eWHITEPLAYERCHANGED,
        e640X480,
		e800X600,
		e1024X768,
		e1280X1024,
		e1400X1050,
		e1600X1200,
	};

	/** For keeping track of the Menu's current state */
	enum State { 
		INACTIVE, 
		ACTIVE, 
		MENUIN, 
		MENUOUT,
		SUBMENUIN,
		SUBMENUOUT
	};

 private:
    
    // Draws this Menu with respect to the specified alpha value.
	void draw(const float a);

    // Iterates to the next menu item, goes to the first item when we reach the last.
	void nextMenuItem();

    // Iterates backwards through the menu items, goes to the last item went
    // we iterate back past the first.
	void prevMenuItem();

    // Gets the MenuItem currently pointed to by the mouse.  If no MenuItem
    // is pointed to, returns iterator to the end of the current options set.
    std::vector<MenuItem*>::const_iterator getMousePointedMenuItem(const SDL_Event & e);

    // Pops an option set off the set history stack.
    void popOptionsSet();

	typedef std::map<std::string, std::vector<MenuItem*> > ItemSet_t;
    std::string                 m_header;
	
    // A map of menu titles to MenuItem sets.
    ItemSet_t                   m_item_sets;

    // An iterator which points to the current MenuItem set.
	ItemSet_t::const_iterator   m_curr_set;

    // An iterator which points to the last MenuItem set.
	ItemSet_t::const_iterator   m_last_set;

    // Draws the header
	void drawHeader(ItemSet_t::const_iterator set, float a);
	
    // Draws the current ItemSet
    void drawSet(ItemSet_t::const_iterator set, float a);

    std::vector<MenuItem*>::const_iterator  m_curr_menuitem;
	std::stack<std::string>	                m_sethistory;

    // Offsets for tweaking the appearance of the menu.
	float m_xoffset;
	float m_yoffset;
	float m_width;

    // Current state of the Menu.
	State m_state;

    // Timers for animations.
	Timer m_alphatimer;
	Timer m_scaletimer;
	Timer m_slidetimer;
};

#endif // MENU_H

// End of file menu.h

