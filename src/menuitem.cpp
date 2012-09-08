/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : menuitem.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "fontloader.h"
#include "menu.h"
#include "menuitem.h"
#include "SDL.h"

#include <iostream>
#include <string>

using namespace std;

void drawText(const string & text)
{
	glScaled(1/1000.0, 1/1000.0, 1/1000.0);
	FontLoader::print(0, 0, text.c_str());	
	glScaled(1000, 1000, 1000);
}

void MenuItem::draw(float a)
{
    // If the MenuItem isn't hovered, only draw the text.
    if (!m_is_hovered) {
        drawText(m_label);
        return;
    }

    // Otherwise, see if its fading in our out and adjust appropriately.
    double dist;
    double scale;
    if (m_state == HOVERIN || m_state == HOVEROUT || m_state == SELECTANIM) {
        dist = m_slidetimer.value();
        scale = m_scaletimer.value();
        // If we're running the select animation, we need to scale.
		if (m_state == SELECTANIM) {
			drawHovered(a);
            glScaled(scale, scale, 0.0);
		}
        // If we're hovering out, we need to slide left.
        //if (m_state == HOVEROUT)
        //    glTranslated(dist, 0.0, 0.0);
        drawHovered(a*m_alphatimer.value());
        m_alphatimer++;
        m_slidetimer++;
        m_scaletimer++;
        // If the timer is done, we reset the state to STILL, also
        // if we are hovering out, we need to translate back to where
        // we slid to (since we change the state here), and un-set
        // m_is_hovered.
        if (m_alphatimer.done()) {
            if (m_state == HOVEROUT) {
                m_is_hovered = false;
                //glTranslated(-dist, 0.0, 0.0);
            }
            else if (m_state == SELECTANIM) {
                glScaled(1/scale, 1/scale, 0.0);
                hoverIn();
            }
            m_state = STILL;
        }
    }
    else {
        // If we aren't hovering in or out, just draw at the current
        // alpha of the options set.
        drawHovered(a);
    }
    // If we're hovering out then we've slid to the left some distance,
    // we need to slide that distance back to the right.
    //if (m_state == HOVEROUT)
    //    glTranslated(-dist, 0.0, 0.0);
    if (m_state == SELECTANIM)
        glScaled(1/scale, 1/scale, 0.0);
    glColor4f(0.0, 0.0, 0.0, 0.5*a);
    drawText(m_label);
}

void MenuItem::onSelect()
{
    m_state = SELECTANIM;
    m_scaletimer = Timer(Timer::LOGARITHMIC);
    m_scaletimer.setRange(1.0, 1.5);
    m_scaletimer.setDuration(0.4);
    m_scaletimer.start();

    m_alphatimer = Timer(Timer::LOGARITHMIC);
    m_alphatimer.setRange(1.0, 0.0);
    m_alphatimer.setDuration(0.4);
    m_alphatimer.start();
}

void MenuItem::hoverIn()
{
    m_state = HOVERIN;
    m_alphatimer = Timer(Timer::LOGARITHMIC);
    m_alphatimer.setRange(0.0, 1.0);
    m_alphatimer.setDuration(0.2);
    m_alphatimer.start();
    m_is_hovered = true;
}

void MenuItem::hoverOut()
{
    m_state = HOVEROUT;
    m_alphatimer = Timer(Timer::LOGARITHMIC);
    m_alphatimer.setRange(1.0, 0.0);
    m_alphatimer.setDuration(0.3);
    m_alphatimer.start();

    m_slidetimer = Timer(Timer::LOGARITHMIC);
    m_slidetimer.setRange(0.0, -0.015);
    m_slidetimer.setDuration(0.3);
    m_slidetimer.start();
}

// Draw the splash behind a hovered MenuItem.
void MenuItem::drawHovered(const float a)
{
    const float width       =  0.645;
    const float height      =  0.046; 
    const float xoffset     = -0.005;
    const float yoffset     = -0.01;
    const float fletchwidth =  0.005;
    
	glEnable(GL_BLEND);
    
    // Draw the shaded splash background.
    glTranslated(width/2.0 + xoffset, height/2.0 + yoffset, 0.0);
    glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 0.0*a);
        glVertex2f( width/2.0, -height/2.0);
        glVertex2f( width/2.0,  height/2.0);
		glColor4f(0.0, 0.0, 0.0, 0.2*a);
        glVertex2f(-width/2.0,  height/2.0);
        glVertex2f(-width/2.0, -height/2.0);
    glEnd();

    // Draw the fletch at the left of the MenuItem.
    glTranslated(-width/2.0, -height/2.0, 0.0);
    glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 0.5*a);
        glVertex2f(-fletchwidth,   0.0);
        glVertex2f(         0.0,   0.0);
        glVertex2f(         0.0, height);
        glVertex2f(-fletchwidth, height);
    glEnd();
    glTranslated(-xoffset, -yoffset, 0.0);
}

// Send the event wit the specified code to the event handler.
void MenuItem::sendUserEvent(int eventCode)
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = eventCode;
	SDL_PushEvent(&event);
}

// ******** ActionItem ********
void ActionItem::onSelect()
{
    MenuItem::onSelect();
	sendUserEvent(m_eventCode);
}

// ******** ToggleItem ********         
void ToggleItem::draw(float a)
{
	MenuItem::draw(a);
	glTranslated(0.4, 0.0, 0.0);

	if (!m_is_on)
		glColor4f(0.0, 0.0, 0.0, 0.15*a);
	drawText("on");

	glTranslated(0.1, 0.0, 0.0);

	glColor4f(0.0, 0.0, 0.0, 0.5*a);	
	if (m_is_on)
		glColor4f(0.0, 0.0, 0.0, 0.15*a);
	drawText("off");
	
	glTranslated(-0.5, 0.0, 0.0);
}

void ToggleItem::onSelect()
{
    MenuItem::onSelect();
	m_is_on = !m_is_on;
	sendUserEvent(m_eventCode);
}

// ******** ChoicesItem ********
void ChoicesItem::draw(float a)
{
	MenuItem::draw(a);
	glTranslated(0.4, 0.0, 0.0);
	drawText((*m_currChoice).first);
	glTranslated(-0.4, 0.0, 0.0);
}

void ChoicesItem::onSelect()
{
    MenuItem::onSelect();
    ++m_currChoice;
	if (m_currChoice == m_choices.end())
		m_currChoice = m_choices.begin();
	sendUserEvent((*m_currChoice).second);
}

void ChoicesItem::addChoice(const string & choiceName, int eventCode)
{
	m_choices.push_back(make_pair(choiceName, eventCode));
	// Set the iterator to point to the first choice that we add.
	m_currChoice = m_choices.begin();
}

string ChoicesItem::getCurrentChoice() const
{
    return m_currChoice->first;
}

void ChoicesItem::setChoice(const std::string & choiceName)
{
	std::vector<std::pair<std::string, int> >::const_iterator ci;
	for(ci = m_choices.begin(); ci != m_choices.end(); ci++) {
		if( ci->first == choiceName ) {
			m_currChoice = ci;
            this->sendUserEvent(ci->second);
            return;
		}
	}
}

// ******** ChangeMenuItem ********
void ChangeMenuItem::onSelect()
{
    MenuItem::onSelect();
	m_parent->pushOptionsSet(m_menuname);
}

// ******** SeparatorItem ********
void SeparatorItem::draw(float a)
{
	glBegin(GL_QUADS);
		glColor4f(0.0, 0.0, 0.0, 0.0*a);
		glVertex2f(0.0, 0.04);
		glVertex2f(0.6, 0.04);
		glVertex2f(0.6, 0.055);
		glColor4f(0.0, 0.0, 0.0, 0.3*a);
		glVertex2f(0.0, 0.055);
	glEnd();

	glTranslated(0.0, 0.07, 0.0);
}

// End of file menuitem.cpp

