/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : menuitem.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <vector>

#include "timer.h"

class Menu;

/**
 * Defines the interface for using a MenuItem 
 */
class MenuItem {
 public:
	
	/** Constructs a new MenuItem */
	MenuItem(const std::string & label, const float height = 0.1f, const float spacing = 0.08f) : 
		m_label(label),
        m_is_collapsed(false),
		m_is_hovered(false),
		m_height(height),
        m_spacing(spacing),
        m_state(STILL) {}

 	/** Draws the MenuItem */
	virtual void draw(float a);

	/**
	 * Used to define the behavior for a MenuItem when it is selected with
	 * the keyboard or mouse.
	 */
	virtual void onSelect();

    /** Sets whether the MenuItem should be collapsed (not drawn) or drawn) */
    void setCollapsed(bool collapsed)
        { m_is_collapsed = collapsed; }

    bool isCollapsed() const
        { return m_is_collapsed; }

	/** Sets the MenuItem's parent menu.  */
	void setParent(Menu * parent)
		{ m_parent = parent; }

	/** Tells the menu item that it is being hovered over. */
	virtual void hoverIn();

	/** Tells the MenuItem that it is no longer being hovered over. */
	virtual void hoverOut();

    /** Returns the height of the MenuItem */
	float height()
		{ return m_height; }

    /** Returns the spacing for the MenuItem */
    float spacing()
        { return m_spacing; }

    /** Keeps track of the current state of the MenuItem */
    enum State {
        HOVERIN,
        HOVEROUT,
        SELECTANIM,
        STILL,
    };

 protected:

	/**
	 *	Draws something to indicate that the MenuItem is the one selected.
	 */
	void drawHovered(const float a);

	void sendUserEvent(int eventcode);

	Menu *          m_parent;
	std::string     m_label;
    bool            m_is_collapsed;
	bool            m_is_hovered;
	float           m_height;
    float           m_spacing;
    State           m_state;
    Timer           m_alphatimer;
    Timer           m_scaletimer;
    Timer           m_slidetimer;
};

class ActionItem : public MenuItem {
 public:
	ActionItem(const std::string & label, int eventCode) :
		MenuItem(label),
		m_eventCode(eventCode) {}

	void onSelect();

private:
	int m_eventCode;
};

/**
 * A MenuItem used to control features that can be toggled on and off.
 */
class ToggleItem : public MenuItem {
 public:

	/** 
	 * Constructs a ToggleItem with the specified event code.  All 
	 * ToggleItems default to true.
	 */
	ToggleItem(const std::string & label, int eventCode, bool on = true) :
		MenuItem(label),
		m_eventCode(eventCode), 
		m_is_on(on) {}

	void setOn(bool on) { m_is_on = on; }

	void draw(float a);

	void onSelect();

 private:
	int m_eventCode;
 	bool m_is_on;
};

/**
 * A MenuItem that allows you to choose between a couple different choices.
 */
class ChoicesItem : public MenuItem {
 public:
	
	/** 
	 * Constructs an empty useless ChoicesItem.  Choices must be
	 * added via the addChoice function for objects of this sort
	 * to actually be useful.
	 */
	ChoicesItem(const std::string & label) :
		MenuItem(label) {}

	void draw(float a);

	void onSelect();

	/**
	 * Adds a choice with the specified name and event code to
	 * this choices item.
	 */
	void addChoice(const std::string & choiceName, int eventCode);	

    std::string getCurrentChoice() const;

	void setChoice(const std::string & choiceName);

 private:
	std::vector<std::pair<std::string, int> > m_choices;
	std::vector<std::pair<std::string, int> >::const_iterator m_currChoice;
};

/**
 * 
 */
class ChangeMenuItem : public MenuItem {
 public:
	ChangeMenuItem(const std::string & label, const std::string & menuname) :
		MenuItem(label),
		m_menuname(menuname) {}

	void onSelect();
 private:
	std::string m_menuname;
};

/**
 * Used to separate different sets of MenuItems.
 */
class SeparatorItem : public MenuItem {
 public:

	SeparatorItem() :
		MenuItem("", 0.01f, 0.01f) {}

	/** Draws the MenuItem on the Menu at the specified x-y coordintes */
	virtual void draw(float a);

	/** Just a no-op, a separator doesn't do anything on mouseover. */
	virtual void onSelect()	{}
};

#endif // MENUITEM_H

// End of file menuitem.h

