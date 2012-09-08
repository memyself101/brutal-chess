#ifndef Q3CHARMODEL_H
#define Q3CHARMODEL_H

#include <string>
#include <map>
#include "md3model.h"

/**
 * Loads and collects all the meshes, animations, and textures for a Quake 3 
 * character model.
 */
class Q3CharModel {
 private:
	struct q3AnimParams_t {
		int first_frame;
		int num_frames;
		int loop_frames;
		int ticks_per_frame;
	};

	typedef std::map<std::string, q3AnimParams_t> str_to_param_map;	

	str_to_param_map m_lower_anim, m_upper_anim;
	
	str_to_param_map::const_iterator m_lower_pi, m_upper_pi;
	
 public:
	Q3CharModel() :m_lowerframe(0), m_upperframe(0) {}
	Q3CharModel(const std::string& modeldir, 
					const std::string skin = "default") 
		:m_lowerframe(0), m_upperframe(0)
		{ load(modeldir, skin); }

	bool load(const std::string& modeldir, const std::string& skin = "default");

	bool loadGL();
	
	struct Q3AnimState {
		public:
			int currFrame;
			int startTicks;
			double frameProgress;
			str_to_param_map::iterator anim;
	};

	void draw(Q3AnimState & upperstate, Q3AnimState & lowerstate);

	void updateAnim(Q3AnimState & as);

	Q3AnimState getLowerAnimation(const std::string& s);
	Q3AnimState getUpperAnimation(const std::string& s);
	
	std::string get_lower_animation()
		{ return m_lower_pi->first; }
	std::string get_upper_animation()
		{ return m_upper_pi->first; }
	
 private:
	int lastTicks;
	int lastUpperTicks;

	float m_center[3];

	std::string m_modeldir;
	std::string m_skin;

	MD3Model m_lower, m_upper, m_head;

	int m_upperframe, m_lowerframe;

	
	void loadAnimParams();
	void readParamsFromStream(std::istream & is, q3AnimParams_t & params);
	int nextFrame(Q3AnimState as);
};

#endif
