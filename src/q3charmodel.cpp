/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : q3charmodel.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "q3charmodel.h"

#include <iostream>
#include <fstream>
#include <string>

#include "SDL_opengl.h"
#include "SDL.h"

using namespace std;

bool Q3CharModel::load(const string& modeldir, const string& skin)
{
	m_modeldir = modeldir;
	m_skin = skin;

	m_lower.load(modeldir + "/lower.md3", skin);
	m_upper.load(modeldir + "/upper.md3", skin);
	m_head.load(modeldir + "/head.md3", skin);

	m_lower.link(&m_upper);
	m_upper.link(&m_head);

	cout << "Load aimation params" << endl;
	loadAnimParams();
	
	float min[3];
	float max[3];
	min[0] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.min_bound[0];
	min[1] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.min_bound[1];
	min[2] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.min_bound[2];
	max[0] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.max_bound[0];
	max[1] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.max_bound[1];
	max[2] = m_lower.m_frames[m_lower_anim["LEGS_WALK"].first_frame].header.max_bound[2];
	m_center[0] = -(max[0]-min[0])/2.0-min[0];
	m_center[1] = -min[2];
	m_center[2] = -(max[1]-min[1])/2.0-min[1];
	return true;
}

bool Q3CharModel::loadGL()
{
	m_lower.loadGL();
	m_upper.loadGL();
	m_head.loadGL();
	return true;
}

void Q3CharModel::draw(Q3AnimState & upperstate, Q3AnimState & lowerstate)
{
	glPushMatrix();
	glTranslatef(m_center[0], m_center[1], m_center[2]);
	glRotated( -90.0, 1.0, 0.0, 0.0);
	glRotated( -90.0, 0.0, 0.0, 1.0);
	m_lower.setFrame(lowerstate.currFrame, nextFrame(lowerstate),
			lowerstate.frameProgress);
	m_upper.setFrame(upperstate.currFrame, nextFrame(upperstate),
			upperstate.frameProgress);
	m_lower.draw();
	glPopMatrix();
}

void Q3CharModel::updateAnim(Q3AnimState & as)
{
	q3AnimParams_t ap = (*(as.anim)).second;
	// Check to see if we are ready to do to the next frame
	int tickdiff = SDL_GetTicks() - as.startTicks;
	if(tickdiff > ap.ticks_per_frame) {
		as.currFrame++;
		if(as.currFrame >= ap.first_frame + ap.num_frames - 1)
			as.currFrame = ap.first_frame;
		as.startTicks = SDL_GetTicks();
		as.frameProgress = 0.0;
	} else {
		as.frameProgress = double(tickdiff) / ap.ticks_per_frame;
	}
	if(ap.num_frames == 1)
		as.frameProgress = 0;
}

Q3CharModel::Q3AnimState Q3CharModel::getLowerAnimation(const string& anim_name)
{
	Q3AnimState ret;
	if (m_lower_anim.find(anim_name) == m_lower_anim.end()) {
		return ret;
	}
	str_to_param_map::const_iterator pi = m_upper_anim.find(anim_name);
	/*if (pi != m_upper_anim.end()) {
		m_upper_pi = pi;
		m_upperframe = m_upper_pi->second.first_frame;
	}*/
	ret.anim = m_lower_anim.find(anim_name);
	ret.currFrame = ret.anim->second.first_frame;
	return ret;
}

Q3CharModel::Q3AnimState Q3CharModel::getUpperAnimation(const string& anim_name)
{
	Q3AnimState ret;
	if (m_upper_anim.find(anim_name) == m_upper_anim.end()) {
		cout << "Failure" << endl;
		return ret;
	}
	str_to_param_map::const_iterator pi = m_lower_anim.find(anim_name);
	/*if (pi != m_lower_anim.end()) {
		m_lower_pi = pi;
		m_lowerframe = m_lower_pi->second.first_frame;
	}*/
	ret.anim = m_upper_anim.find(anim_name);
	ret.currFrame = ret.anim->second.first_frame;
	return ret;
}

void Q3CharModel::loadAnimParams()
{
	string filename = m_modeldir + "animation.cfg";
	ifstream animcfgfile(filename.c_str());
	if (!animcfgfile) {
		cerr << "Failed to load the animation.cfg file: " << filename << endl;
		return;
	}

	string currLine;

	// Load animation parameters for both models
	q3AnimParams_t params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEATH1"] = params;
	m_upper_anim["BOTH_DEATH1"] = params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEAD1"] = params;
	m_upper_anim["BOTH_DEAD1"] = params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEATH2"] = params;
	m_upper_anim["BOTH_DEATH2"] = params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEAD2"] = params;
	m_upper_anim["BOTH_DEAD2"] = params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEATH3"] = params;
	m_upper_anim["BOTH_DEATH3"] = params;
	readParamsFromStream(animcfgfile, params);
	m_lower_anim["BOTH_DEAD3"] = params;
	m_upper_anim["BOTH_DEAD3"] = params;
	

	// Load upper body animations
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_GESTURE"] = params;
	int upperstart = params.first_frame;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_ATTACK1"] = params;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_ATTACK2"] = params;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_DROP"] = params;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_RAISE"] = params;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_STAND1"] = params;
	readParamsFromStream(animcfgfile, params);
	m_upper_anim["TORSO_STAND2"] = params;
	int upperend = params.first_frame + params.num_frames;

	int d = upperend - upperstart;
	// Load lower body animations
	readParamsFromStream(animcfgfile, params);
	if(params.first_frame == upperstart)
		d = 0;
	params.first_frame -= d;
	m_lower_anim["LEGS_WALKCR"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_WALK"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_RUN"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_BACK"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_SWIM"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_JUMP"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_LAND"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_JUMPB"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_LANDB"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_IDLE"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_IDLECR"] = params;
	readParamsFromStream(animcfgfile, params);
	params.first_frame -= d;
	m_lower_anim["LEGS_TURN"] = params;

	m_lower_pi = m_lower_anim.begin();
	m_upper_pi = m_upper_anim.begin();
}

void Q3CharModel::readParamsFromStream(istream& is, q3AnimParams_t & params)
{
	string crap;
	while (!isdigit(is.peek()) && getline(is, crap)) 
		/*loop*/;

	is >> params.first_frame >> 
	       params.num_frames >>
	       params.loop_frames;
	float framespersecond;
	is >> framespersecond;
	params.ticks_per_frame = static_cast<int>(1.0/(framespersecond/1000.0));
	getline(is, crap);	// Get the rest of the line
}

int Q3CharModel::nextFrame(Q3AnimState as)
{
	q3AnimParams_t ap = (*(as.anim)).second;
	if(ap.num_frames == 1 )
		return ap.first_frame;
	if(as.currFrame + 1 > ap.first_frame + ap.num_frames)
		return ap.first_frame + ap.num_frames;
	else
		return as.currFrame + 1;
}

// End of file q3charmodel.cpp

