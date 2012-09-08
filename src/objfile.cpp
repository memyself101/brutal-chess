// objfile.h
// ObjFile
// by Joe Flint
// .obj file loader
// Loads an obj file

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "objfile.h"
#include <fstream>
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// For DBL_MIN and DBL_MAX
#include <float.h>

using namespace std;

ObjFile::ObjFile()
{
	loaded = false;
	usetexture = false;
	built = false;
	centx = centy = centz = true;
	numfaces = numvertices = numnormals = numtextcoords = 0;
	vertices.resize( 1 );
	facenormals.resize( 1 );
	facevertexnormals.resize( 1 );
	vertexnormals.resize( 1 );
	vertexuse.resize( 1 );
	faceuse.resize( 1 );
	facevertextextcoords.resize( 1 );
}

ObjFile::ObjFile( string filename )
{
	// This function can't be defined in the base class as it calls a pure 
	// virtual member function.
	loaded = false;
	built = false;
	centx = centy = centz = true;
	load( filename );
}

bool ObjFile::load( string filename )
{
	if( loaded )
		return false;

	ifstream file( filename.c_str() );
	if( !file )
		return false;
	
	string buffer;
	string part;
	double x, y, z;

	Point min, max;
	min.x = min.y = min.z = DBL_MAX;
	max.x = max.y = max.z = -DBL_MAX;

	int slash;
	int temp;
	
	while( !file.eof() ) {
		file >> buffer;
		
		if( buffer == "v" ) {
			Point p;
			file >> p.x >> p.y >> p.z;
			
			numvertices++;

			if( numvertices > vertices.size() ) {
				vertices.resize( vertices.size()*2 );
				vertexnormals.resize( vertices.size()*2 );
				vertexuse.resize( vertices.size() * 2 );
			}

			vertices.at( numvertices - 1 ) = p;
			

		} else if( buffer == "vt" && usetexture ) {
			Point p;
			double junk;
			file >> p.x >> p.y >> p.z;
			textcoords.push_back( p );
			numtextcoords++;

		} else if( buffer == "vn" ) {
			file >> x >> y >> z;
			normals.push_back( Vector( x, y, z ) );
			numnormals++;

		} else if( buffer == "f" ) {
			numfaces++;
			if( numfaces > faceuse.size() ) {
				faceuse.resize( faceuse.size() * 2 );
				facenormals.resize( faceuse.size() * 2 );
				facevertexnormals.resize( faceuse.size() * 2 );
				if( usetexture )
					facevertextextcoords.resize( faceuse.size() * 2 );
			}
			
			getline( file, buffer );
			
			istringstream bufferinput( buffer );

			while( bufferinput >> part ) {
				
				// Replace double slash so we know there is no texture vertex
				slash = (int)part.find("//");
				if( slash != string::npos )
					part.replace( slash, 2, " 0 " );
				else {
					// With a double slash, there won't be any others
					
					// Replace any remaining slashes with spaces
					slash = (int)part.find("/");
					while( slash != string::npos ) {
						part.replace( slash, 1, " " );
						slash = (int)part.find("/");
					}
				}
				
				// Now, we can get the values
				istringstream partinput( part );
				
				vector< int > partdata( 3, 0 );
				
				for( int i = 0; i < 3 && partinput >> temp; i++ )
					partdata.at( i ) = temp;
					
				// Deal with negative references
				if( partdata.at( 0 ) < 0 )
					partdata.at( 0 ) += numvertices + 1;
				
				if( partdata.at( 0 ) > (int)numvertices )
					// Tried to access an illegal vertex
					return false;	

				x = vertices.at( partdata.at( 0 ) - 1 ).x;
				y = vertices.at( partdata.at( 0 ) - 1 ).y;
				z = vertices.at( partdata.at( 0 ) - 1 ).z;
				// Check for the min and max coord to do a bounding box
				if( x > max.x )
					max.x = x;
				if( y > max.y )
					max.y = y;
				if( z > max.z )
					max.z = z;
				
				if( x < min.x )
					min.x = x;
				if( y < min.y )
					min.y = y;
				if( z < min.z )
					min.z = z;
			
				vertexuse.at( partdata.at( 0 ) - 1 ).push_back( numfaces - 1 );
				faceuse.at( numfaces - 1 ).push_back( partdata.at( 0 ) - 1);

				// Texture coord was specified
				if( partdata.at( 1 ) != 0 && usetexture ) {
					// Deal with negative references
					if( partdata.at( 1 ) < 0 )
						partdata.at( 1 ) += numtextcoords + 1;
					
					if( partdata.at( 1 ) > (int)numtextcoords )
						// Illegal index
						return false;

					facevertextextcoords.at( numfaces - 1 ).push_back( 
						textcoords.at( partdata.at( 1 ) - 1 ) );
				}
				
				// Normal was specified
				if( partdata.at( 2 ) != 0 ) {
					// Deal with negative references
					if( partdata.at( 2 ) < 0 )
						partdata.at( 2 ) += numnormals + 1;
					
					if( partdata.at( 2 ) > (int)numnormals )
						// Tried to access an illegal normal
						return false;
					
					facevertexnormals.at( numfaces - 1 ).push_back( 
							normals.at( partdata.at( 2 ) - 1 ) );
				}

				// Currently only allow triangles
//				if( faceuse.at( numfaces - 1 ).size() > 3 )
//					return false;
				
				if( faceuse.at( numfaces - 1 ).size() >= 3 ) {
					// Calculate face normal
					Vector v1, v2;
					Point p1, p2, p3;

					p1 = vertices.at( faceuse.at( numfaces - 1 ).at( 0 ) );
					p2 = vertices.at( faceuse.at( numfaces - 1 ).at( 1 ) );
					p3 = vertices.at( faceuse.at( numfaces - 1 ).at( 2 ) );
					
					v1.x =  p1.x - p2.x;
					v1.y =  p1.y - p2.y;
					v1.z =  p1.z - p2.z;

					v2.x =  p3.x - p2.x;
					v2.y =  p3.y - p2.y;
					v2.z =  p3.z - p2.z;
					
					facenormals.at( numfaces -1 ) = v2.cross( v1 );
				}
			}
			
		} else {
			getline( file, buffer );
		}
	}

	// Recenter the object around (0,0,0)
	// Max + Min / 2 is the midpoint of the object
	Point mid;
	mid.x = (max.x + min.x) / 2;
	mid.y = (max.y + min.y) / 2;
	mid.z = (max.z + min.z) / 2;
	for( int i = 0; i < (int)numvertices; i++ ) {
		if( centx )
			vertices.at( i ).x -= mid.x;
		else
			vertices.at( i ).x -= min.x;

		if( centy ) {
			vertices.at( i ).y -= mid.y;
		} else
			vertices.at( i ).y -= min.y;

		if( centz )
			vertices.at( i ).z -= mid.z;
		else
			vertices.at( i ).z -= min.z;
	}	
	
	// Calculate the length of each side of the bounding box
	Point length = max;
	length.x -= min.x;
	length.y -= min.y;
	length.z -= min.z;
	
	// Temp var to store the longest side
	double maxvert = length.x;
	
	// Determine which side of the bounding box is the largest
	if( length.y > maxvert )
		maxvert = length.y;
	
	if( length.z > maxvert )
		maxvert = length.z;
	
	// Now rescale the object so it's bounding box is 1.0 units at it's largest
	scl = Vector( 1.0 / maxvert, 1.0 / maxvert, 1.0 / maxvert );
	
	loaded = true;

	return true;
}

void ObjFile::build()
{
	if( !loaded )
		return;
	
	if( built )
		return;
	
	displist = glGenLists( 1 );

	// 0 is an invalid display list index
	if( displist == 0 )
		return;
	
	glNewList( displist, GL_COMPILE );
	draw();
	glEndList();

	built = true;
}

void ObjFile::unbuild()
{
	if( !built )
		return;

	glDeleteLists( displist, 1 );
	built = false;
}

void ObjFile::draw()
{
	// Don't try to draw anything if nothing has been loaded yet
	if( !loaded )
	  return;

	if( built ) {
		glCallList( displist );
		return;
	}

	glPushMatrix();
	
	scl.glScale();

	glBegin( GL_TRIANGLES );
	for( int i = 0; i < (int)numfaces; i++ ) {
		for( int j = 0; j < (int)faceuse.at( i ).size(); j++ ) {
			if( usetexture )	
				( facevertextextcoords.at( i ).at( j ) ).glTexCoord();
			( facevertexnormals.at( i ).at( j ) *scl ).glNormal();
			( vertices.at( faceuse.at( i ).at( j ) ) ).glVertex();
		}
	}
	glEnd();
	
	glPopMatrix();
}

void ObjFile::findNorms()
{
	if( !loaded )
		return;

	for( int i = 0; i < (int)faceuse.size(); i++ ) {
		facevertexnormals.at( i ).resize( faceuse.at( i ).size() );
		for( int j = 0; j < (int)faceuse.at(i).size(); j++ ) {
			Vector avg( 0, 0, 0 );
			Vector eval;
			int numnorms = 0;
			int idx = faceuse.at(i).at(j);
			for( int k = 0; k < (int)vertexuse.at( idx ).size(); k++ ) {
				eval = facenormals.at( vertexuse.at( idx ).at( k ) );
				if( eval.dot( facenormals.at( i ) ) > 0 
					|| i == vertexuse.at( idx ).at( k ) ) {
					avg += eval;
					numnorms++;
				}
			}
			avg = avg / numnorms;
			avg.normalize();
			facevertexnormals.at( i ).at( j ) = avg;
		}
	}
}

void 
ObjFile::enableTexture()
{
	usetexture = true;

	return;
}

void 
ObjFile::disableTexture()
{
	usetexture = false;

	return;
}

Vector 
ObjFile::scale()
{
	return scl;
}

void 
ObjFile::setScale( Vector newscale )
{
	scl = newscale;

	return;
}

void 
ObjFile::setRecenter( bool newcentx, bool newcenty, bool newcentz )
{
	centx = newcentx;
	centy = newcenty;
	centz = newcentz;

	return;
}
