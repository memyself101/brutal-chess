/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : fontloader.h
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef _FONTLOADER_H_IS_INCLUDED_
#define _FONTLOADER_H_IS_INCLUDED_

//FreeType Headers
#include <ft2build.h>
#include FT_FREETYPE_H

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// Other Headers we'll need.
#include <vector>
#include <string>
#include <stdexcept>
#include <stdarg.h>
#include "SDL_opengl.h"

using namespace std;

// So MSVC doesn't spit out warnings regarding vectors of strings.
#pragma warning(disable: 4786) 

class FontLoader
/**************************************************************************
*	fontLoader Class
*
*	- Stores data for fonts, and handles printing to the screen.
**************************************************************************/
{	 
	public:

		// Load a font
		static bool loadFont( string name, string fontfile, 
				unsigned int pixheight );

		// Chose a font
		static bool setFont( string name );
		
		// Recreate display lists after context reset
		static void reload();
		
		//The flagship function of the library - this thing will print
		//out text at window coordinates x,y, using the font ft_font.
		//The current modelview matrix will also be applied to the text. 
		static void print(double x, double y, const char *fmt, ...) ;

		// Unload all of the fonts
		static void unload();

		// Free OpenGL textures and display list
		static void unloadGL();

		static float lastMatrix[16];

	private:
		struct font_data 
		//This holds all of the information related to any
		//freetype font that we want to create. 
		{
			// For user lookup
			string name;

			// Needed to reload
			string file;
			unsigned int height;
			
			float	h;			// Holds the height of the font.
			GLuint	*textures;	// Holds the texture id's 
			GLuint	list_base;	// Holds the first display list id

			//The init function will create a font of
			//of the height h from the file fname.
			bool init(const char *fname, unsigned int h);

			//Free all the resources assosiated with the font.
			void clean();
		};

		static vector< font_data > fonts;
		static unsigned int curfont;

};

#endif // _FONTLOADER_H_IS_INCLUDED_

// End of file fontloader.h
