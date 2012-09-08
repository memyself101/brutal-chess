// textureloader.cpp
// TextureLoader
// by Joe Flint
// OpenGL Texture loading with the help of SDL_Image

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "texture.h"
#include "SDL_image.h"

using namespace std;

// Copy Constructor
Texture::Texture(const Texture & tex)
{
	if(tex.m_loaded) {
		m_img = tex.m_img;
		tex.m_img->refcount++;
	}
	m_texture = tex.m_texture;
	m_loaded = tex.m_loaded;
	m_glloaded = tex.m_glloaded;
}

// Assignment operator
Texture& Texture::operator= (const Texture & tex)
{
	Texture temp(tex);
	temp.swap(*this);
	return *this;
}

bool Texture::load( const string & filename )
{
	// Don't try to load if already loaded
	// Can't just call unload because it isn't thread safe
	if(m_loaded) {
		return false;
	}
	
	// Name is ok, now try to load the image data from the file
	m_img = IMG_Load( filename.c_str() );

	if( m_img == NULL ) {
		// Image file failed to load for some reason
		return false;
	}

	// Make sure the size is a power of two
	if( nearestPow2( m_img->w ) != m_img->w 
			|| nearestPow2( m_img->h ) != m_img->h ) {
		SDL_FreeSurface( m_img );
		return false;
	}

	m_loaded = true;
	return true;
}

void Texture::unload()
{
	if(!m_loaded)
		return;

	unloadGL();
	
	// Free the image data
	SDL_FreeSurface( m_img );

	m_loaded = false;
}

void Texture::use() const
{
	if(!m_loaded || !m_glloaded)
		return;
	glBindTexture( GL_TEXTURE_2D, m_texture );
}

void Texture::loadGL()
{
	if(!m_loaded || m_glloaded)
		return;

	// Make sure texturing isn't erroneously disabled
	glPushAttrib(GL_ENABLE_BIT);
	
	// Load the image data into an OpenGL texture
	glEnable(GL_TEXTURE_2D);
	m_texture = loadSurface(m_img);

	glPopAttrib();
	m_glloaded = true;
}

void Texture::unloadGL()
{
	// Make sure the OpenGL data is actually loaded
	if(!m_glloaded)
		return;
	glDeleteTextures( 1, &m_texture );
	m_glloaded = false;
}

GLuint Texture::loadSurface( SDL_Surface * surf )
{
	GLuint newtext;
	// Create a new texture handle
	glGenTextures( 1, &newtext );
	// Bind to the new handle
	glBindTexture( GL_TEXTURE_2D, newtext );
	// Load the actual data
	gluBuild2DMipmaps( GL_TEXTURE_2D, surf->format->BytesPerPixel, surf->w, 
				  surf->h, getFormat(surf), GL_UNSIGNED_BYTE,
				  surf->pixels );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	return newtext;
}

GLenum Texture::getFormat( SDL_Surface * surf )
{
	SDL_PixelFormat * f = surf->format;
	int r, g, b, a;
	r = f->Rmask;
	g = f->Gmask;
	b = f->Bmask;
	a = f->Amask;
	// TODO This might be endian dependent, should look into it
	if(a == 0) {
		// No alpha channel present
		if( b < g && g < r )
			return GL_BGR;
		else
			return GL_RGB;
	} else {
		// Alpha channel present
		if( b < g && g < r )
			return GL_BGRA;
		else
			return GL_RGBA;
	}
}

unsigned int Texture::nearestPow2( unsigned int n )
{
	unsigned int pow2;
	for( pow2 = 2; pow2 < n; pow2 *= 2 )  {}
	return pow2;
}

void Texture::swap(Texture & tex)
{
	std::swap(m_img, tex.m_img);
	std::swap(m_texture, tex.m_texture);
	std::swap(m_loaded, tex.m_loaded);
	std::swap(m_glloaded, tex.m_glloaded);
}

