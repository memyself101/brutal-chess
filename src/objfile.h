/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _OBJFILE_H
#define _OBJFILE_H

#include "vector.h"
#include <string>
#include <vector>
#include "SDL_opengl.h"

using namespace std;

class ObjFile
{
	public:
		ObjFile();
		ObjFile( string filename );
		bool load( string filename );	

		void build();
		void unbuild();
		void draw();
		void findNorms();

		void enableTexture();
		void disableTexture();

		Vector scale();
		void setScale( Vector newscale );

		void setRecenter( bool newcentx, bool newcenty, bool newcentz );

	protected:
		bool loaded;
		bool usetexture;

		bool centx, centy, centz;
		
		bool built;
		GLuint displist;
		
		unsigned int numvertices, numfaces, numnormals, numtextcoords;

		Vector scl;
		
		// Storage for all vertices that are used to make faces
		struct Point {
			double x, y, z;
			void glVertex() { glVertex3d( x, y, z ); }
			void glTexCoord() { glTexCoord3d( x, 1-y, z ); }
		};
		vector< Point > vertices;

		vector< Point > textcoords;
		vector< vector< Point > > facevertextextcoords;

		// Storage for different types of normals
		vector< Vector > facenormals;
		vector< vector< Vector > > facevertexnormals;
		vector< Vector > vertexnormals;
		vector< Vector > normals;
		
		// List of faces that use a specific vertex
		vector< vector< int > > vertexuse;
		// List of vertices that a face uses
		vector< vector< int > > faceuse;
	
};
#endif
