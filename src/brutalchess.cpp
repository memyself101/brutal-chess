/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : brutalchess.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/
 
#ifdef WIN32
	#include <windows.h>
#endif

#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_thread.h"

#include "boardtheme.h"
#include "chessgame.h"
#include "chessplayer.h"
#include "config.h"
#include "fontloader.h"
#include "gamecore.h"
#include "pieceset.h"
#include "options.h"
#include "utils.h"

using namespace std;

const char * const BRUTAL_CHESS_VERSION = "Brutal Chess 0.5.2";

/**
 * Resizes the game window, handles updating all OpenGL stuff
 * @param width - new width of the window
 * @param height - new height of the window
 */
bool resizeWindow(int width, int height)
{
	if (height == 0) { height = 1; }	// Protect against devide by zero
	
	GLfloat aspectRatio;
	aspectRatio = (float)width/(float)height;
	
	glViewport(0, 0, width, height);	// Set up our viewport.
	glMatrixMode(GL_PROJECTION);		// Change to the projection matrix and set our viewing volume.
	glLoadIdentity();	
	gluPerspective(30.0f, aspectRatio, 0.1f, 151.0f);	// Sets our persepective.
	glMatrixMode(GL_MODELVIEW);	// Change to the modelview matrix and set out viewing volume.
	glLoadIdentity();
	
	return true;
}

/**
 * Initializes all the appropriate OpenGL settings
 */
bool initGL()
{
	glShadeModel(GL_SMOOTH);				// Enable smooth shading. 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);			// Set the background color to black. 
	glClearDepth(1.0f);					// Depth buffer setup. 
	glEnable(GL_DEPTH_TEST);				// Enables Depth Testing 
	glDepthFunc(GL_LEQUAL);					// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations 
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable( GL_COLOR_MATERIAL );
	GLfloat light_position[]       = { 3.0,   3.0,  5.0,  0.0 };
	const GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat specularmat[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularmat);
	glMaterialf(GL_FRONT, GL_SHININESS, 128);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);

	return true;
}

/*
 * Handles the drawing of the frame at the highest level. Calculates FPS at 
 * five second intervals.
 * @param core - Current state of the game.
 */
bool drawGLScene(GameCore * core)
{
	// These are to calculate our fps. 
	static int T0     = 0;
	static int Frames = 0;
	
	// Clear The Screen And The Depth Buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	core->draw();
	
	// Draw it to the screen. 
	SDL_GL_SwapBuffers();

	// Gather our frames per second. 
	Frames++;
	int t = SDL_GetTicks();
	if (t - T0 >= 5000) {
		float sec = (float)((t-T0)/1000.0);
		float fps = Frames / sec;
//		cout << Frames << " frames in "<< sec << " seconds = "<< fps << " FPS\n";
		T0 = t;
		Frames = 0;
	}
    
	return true;
}

/*
 * Needed for loading the GameCore due to limitations of SDL_Thread
 * @param gc - GameCore that needs to be loaded
 */
int callLoad(void * gc)
{
	GameCore* core = (GameCore*)gc;
	core->load();
	return 0;
}

/*
 * Come on, its main.
 */
int main(int argc, char *argv[])
{
#ifdef WIN32
	// Seems this was removed in SDL 1.2.10
	freopen("stdout.txt", "w", stdout);
	freopen("stderr.txt", "w", stderr);
#endif

	// Options singleton
	Options* opts = Options::getInstance();

	// Parse command line options before doing any screen initialization
	parseCommandLine(argc, argv);
	
	// Screen Settings
	const int WINDOW_WIDTH = opts->getResolutionWidth();
	const int WINDOW_HEIGHT = opts->getResolutionHeight();
	const int WINDOW_BPP = 16;

	// Declares our SDL surface
	SDL_Surface *surface;
   
	// Flags to pass to SDL_SetVideoMode.
	int videoFlags;
	
	// Used to collect events. 
	SDL_Event event;
    
	// This holds some info about our display.
	const SDL_VideoInfo *videoInfo;
    
	// Whether or not the window is active.
	bool isActive = true;

	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Video initialization failed: " << SDL_GetError() << endl;
		Quit(1);
	}

	// Fetch the video info.
	videoInfo = SDL_GetVideoInfo();

	if (!videoInfo) {
		cerr << "Video query failed: " << SDL_GetError() << endl;
		Quit(1);
	}

	// The flags to pass to SDL_SetVideoMode.
	videoFlags  = SDL_OPENGL;          // Enable OpenGL in SDL.
	videoFlags |= SDL_GL_DOUBLEBUFFER; // Enable double buffering.
	videoFlags |= SDL_HWPALETTE;       // Store the palette in hardware.
	//videoFlags |= SDL_FULLSCREEN;       // Store the palette in hardware.
	//videoFlags |= SDL_RESIZABLE;       // Enable window resizing.

	// This checks to see if surfaces can be stored in memory.
	if (videoInfo->hw_available) {
		videoFlags |= SDL_HWSURFACE;
	} else {
		videoFlags |= SDL_SWSURFACE;
	}

	// This checks if hardware blits can be done.
	if (videoInfo->blit_hw) {
		videoFlags |= SDL_HWACCEL;
	}

	// Sets up OpenGL double buffering.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Need some a bit for the stencil buffer
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

	// Sets the string displayed as the title of the window.
	SDL_WM_SetCaption(BRUTAL_CHESS_VERSION, NULL);

	// Get a SDL surface.
	if(opts->fullscreen) {
		surface = SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_BPP,videoFlags | SDL_FULLSCREEN);
	} else {
		surface = SDL_SetVideoMode(WINDOW_WIDTH,WINDOW_HEIGHT,WINDOW_BPP,videoFlags);
	}

	// Verify there is a surface.
	if (!surface) {
		cerr << "Video mode set failed: " << SDL_GetError() << endl;
		Quit(1);
	}

	// Initialize all the static board functions
	Board::init();

	ChessPlayer* player1 = toPlayer(opts->player1);
	ChessPlayer* player2 = toPlayer(opts->player2);
	BoardTheme* boardTheme = toBoard(opts->board);
	PieceSet* pieceSet = toPieces(opts->pieces);

	// Initialize OpenGL.
	initGL();

	// Resize the initial window.
	resizeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Create a chess game object here.
	ChessGame game(player1, player2);

	// We delete game core after the player quits the game, the players are
	// deleted in the ChessGame destructor, and the BoardTheme and PieceSets
	// are deleted by the GameCore's destructor.
	GameCore * core = GameCore::get_Instance();
	core->init(game, boardTheme, pieceSet);

	// Call the core's load function in a new thread
	SDL_Thread *thread = SDL_CreateThread(callLoad, core);
	core->setLoadThread(thread);
	if(thread == NULL) {
		cerr << "Unable to create load thread: " << SDL_GetError() << endl;
		Quit( 1 );
	}	
	
	bool quit = false;
	int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
	string resolution = opts->getResolutionString();

    bool goodfullscreen = opts->fullscreen;
	int goodwidth = width, goodheight = height;
	string goodresolution = resolution;

	// Main event loop
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				quit = true;
			} else if(event.type == SDL_VIDEORESIZE) {
				core->unloadGL();
				SDL_Quit();
				SDL_Init(SDL_INIT_VIDEO);
				// Sets up OpenGL double buffering.
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
				// Need some a bit for the stencil buffer
				SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

				// Sets the string displayed as the title of the window.
				SDL_WM_SetCaption(BRUTAL_CHESS_VERSION, NULL);
				surface = SDL_SetVideoMode(event.resize.w, event.resize.h, 
                    WINDOW_BPP,videoFlags | (opts->fullscreen ? SDL_FULLSCREEN : 0));

				if(!surface) {
					cerr << "Could not get a surface after resize: ";
					cerr << SDL_GetError() << endl;
					cerr << "Using last known good resolution" << endl;

					// Reset the width and height to the last working ones
					width = goodwidth;
					height = goodheight;
                    opts->fullscreen = goodfullscreen;
					resolution = goodresolution;
					core->setResolution(resolution);
                    core->setFullscreen(opts->fullscreen);

					// Apply the new known good resolution
					SDL_Event e;
					e.type = SDL_USEREVENT;
					e.user.code = Menu::eAPPLYRES;
					SDL_PushEvent(&e);

					// Process the next event
					continue;
				}

				// This resolution works
				goodwidth = width;
				goodheight = height;
                goodfullscreen = opts->fullscreen;
				goodresolution = resolution;

				resizeWindow(event.resize.w, event.resize.h);
				initGL();
				core->loadGL();
				FontLoader::reload();
			} else if(event.type == SDL_KEYDOWN) {
				// empty for now	
			} else if(event.type == SDL_USEREVENT) {
				if(event.user.code == Menu::eFULLSCREENTOG) {
                    opts->fullscreen = !opts->fullscreen;
				} else if(event.user.code == Menu::eAPPLYRES) {
					SDL_Event e;
					e.type = SDL_VIDEORESIZE;
					e.resize.w = width;
					e.resize.h = height;
					SDL_PushEvent(&e);
				} else if(event.user.code == Menu::e640X480) {
					width = 640;
					height = 480;
					resolution = "640x480";
                    opts->resolution = r640X480;
				} else if(event.user.code == Menu::e800X600) {
					width = 800;
					height = 600;
					resolution = "800x600";
                    opts->resolution = r800X600;
				} else if(event.user.code == Menu::e1024X768) {
					width = 1024;
					height = 768;
					resolution = "1024x768";
                    opts->resolution = r1024X768;
				} else if(event.user.code == Menu::e1280X1024) {
					width = 1280;
					height = 1024;
					resolution = "1280x1024";
                    opts->resolution = r1280X1024;
				} else if(event.user.code == Menu::e1400X1050) {
					width = 1400;
					height = 1050;
					resolution = "1400x1050";
                    opts->resolution = r1400X1050;
				} else if(event.user.code == Menu::e1600X1200) {
					width = 1600;
					height = 1200;
					resolution = "1600x1200";
                    opts->resolution = r1600X1200;
				}
			}
			core->handleEvent(event);
		}
		drawGLScene(core);
		SDL_Delay(1);
			
	}
	
	core->destroy();
	
	// Clean ourselves up and exit.
	Quit(0);

	// Should never get here.
	cerr << "Shouldn't have reached this... End of brutalchess.cpp.\n";
	return 0;
}

// End of file brutalchess.cpp
