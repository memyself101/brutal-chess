/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : gamecore.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef GAMECORE_H
#define GAMECORE_H

#include "boardtheme.h"
#include "chessgame.h"
#include "menu.h"
#include "objfile.h"
#include "pieceset.h"
#include "SDL.h"
#include "SDL_thread.h"
#include "texture.h"

class ChoicesItem;
class Options;
class ToggleItem;

/**
 * Handles events, drawing, and setup for the game
 */
class GameCore {
 public:
	/**
	 * Gets the instance of GameCore, Singleton pattern, there should
	 * only be once instance of the GameCore.
	 */
	static GameCore * get_Instance();

	/**
	 * Cleans up after the GameCore by freeing the BoardTheme and PieceSet.
	 */
	void destroy();
	
	/**
	 * This needs to be called before all other methods can work, I 
	 * wish there was a better way to do this.
	 */
	void init(const ChessGame & cg, BoardTheme * bt, PieceSet * ps);
	
	/**
	 * Load all non-OpenGL assets
	 */
	bool load();

	/**
	 * Load all OpenGL assets
	 */
	bool loadGL();

	void unloadGL();
	
	/**
	 * Handles all the drawing of the current state of the game
	 */
	void draw();

	/**
	 * Handle an event from SDL
	 */
	bool handleEvent(SDL_Event& e);

	void setLoadThread(SDL_Thread * loadthread) { m_loadthread = loadthread; }

	/**
	 * Set the resolution
	 */
	void setResolution(std::string resolution);

	/**
	 * Set the fullscreen state
	 */
	void setFullscreen(bool fullscreen);

    void setIsWaitingForPromotion(bool waiting)
        { m_isWaitingForPromotion = waiting; }

    bool isWaitingForPromotion() const
        { return m_isWaitingForPromotion; }

	Piece::Type getPromotionSelection(const BoardPosition & bp);
		 
	/**
	 * Returns the current game
	 */
	const ChessGame & game()
		{ return m_game; }
	
	void toBoardSpace();
 private:

	static GameCore * m_instance;
	
	GameCore() :
		m_loaded(false),
		m_preloaded(false),
		m_glloaded(false),
		m_mousex(0),
		m_mousey(0),
		m_mousepos(),
		m_thinkthread(0),
		m_rotate(false),
		m_rotatex(0),
		m_rotatey(0) {}

	bool preload();
	void drawLoadingScreen();

	void buildMenu();

	void projectShadows();
	void drawReflections();
	
	void updateMouseBoardPos();

	SDL_Thread * m_thinkthread;
	SDL_Thread * m_loadthread;

	void spawnThinkThread();
	
	BoardTheme * m_theme; 
	PieceSet * m_set;
	ChessGame m_game;

	Menu m_menu;
    
	ChoicesItem * m_whiteplayerchoices;
	std::string m_suggestedwhiteplayer;
	ChoicesItem * m_whitebrutalplychoices;
	ChoicesItem * m_blackplayerchoices;
	std::string m_suggestedblackplayer;
	ChoicesItem * m_blackbrutalplychoices;

	ChoicesItem * m_resolutionchoices;
	ToggleItem * m_fullscreentoggle;

	int m_mousex, m_mousey;
	double m_rotatey, m_rotatex;
	float m_mouseboardx, m_mouseboardy;
	BoardPosition m_mousepos;
	BoardPosition m_firstclick;
	BoardPosition m_secondclick;
	SDL_Cursor* m_blankcur;
	SDL_Cursor* m_defaultcur;

	bool m_isWaitingForPromotion;	
	bool m_loaded, m_glloaded, m_preloaded;
	bool m_rotate;

    Options* m_options;
    Timer   m_endgametimer;

	ObjFile	m_loadpawn;
	Texture m_logotexture;
};

#endif // GAMECORE_H

// End of file gamecore.h
