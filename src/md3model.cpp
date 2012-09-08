/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : md3model.cpp
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#include "md3model.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#ifndef WIN32
	#include <libgen.h>
#endif

#include "SDL_opengl.h"
#include "SDL.h"

using namespace std;

bool MD3Model::load(const string& filename, const string& skin)
{
	float pi = 3.1415;
	m_filename = filename;
	m_skin = skin;
	ifstream infile(m_filename.c_str(), ios::in | ios::binary);
	if (!infile.is_open()) {
		cout << "Couldn't open file " << m_filename << endl;
		return false;
	}
		
	infile.read((char*)&m_header, sizeof(m_header));

	md3Frame_t	currFrame;
	infile.seekg(m_header.frame_start, ios_base::beg);
	for (int i = 0; i < m_header.frame_num; ++i) {
		infile.read((char*)&currFrame.header, sizeof(md3FrameHeader_t));
		m_frames.push_back(currFrame);
	}

	md3Tag_t currTag;
	infile.seekg(m_header.tag_start, ios_base::beg);
	for (int i = 0; i < m_header.frame_num; ++i) {
		for (int j = 0; j < m_header.tag_num; ++j) {
			infile.read((char*)&currTag, sizeof(md3Tag_t));
			if((currTag.origin[0] >= -0.001 && currTag.origin[0] <= 0.001) &&
			   (currTag.origin[1] >= -0.001 && currTag.origin[1] <= 0.001) &&
			   (currTag.origin[2] >= -0.001 && currTag.origin[2] <= 0.001))
				m_tagname = currTag.name;
			m_frames[i].tags.push_back(currTag);
		}
	}

	md3Mesh_t currMesh;
	infile.seekg(m_header.mesh_start, ios_base::beg);
	for(int i = 0; i < m_header.mesh_num; i++) {
		int headerstart = infile.tellg();
		currMesh.skins.clear();
		currMesh.triangles.clear();
		currMesh.texcoords.clear();
		currMesh.vertices.clear();
		infile.read((char*)&currMesh.header, sizeof(md3MeshHeader_t));

		md3Skin_t currSkin;
		for(int i = 0; i < currMesh.header.skin_num; ++i) {
			infile.read((char*)&currSkin, sizeof(md3Skin_t));
			currMesh.skins.push_back(currSkin);
		}

		infile.seekg(headerstart + currMesh.header.triangle_start, ios_base::beg);
		md3Triangle_t currTriangle;
		for(int i = 0; i < currMesh.header.triangle_num; ++i) {
			infile.read((char*)&currTriangle, sizeof(md3Triangle_t));
			currMesh.triangles.push_back(currTriangle);
		}
	
		infile.seekg(headerstart + currMesh.header.texcoord_start, ios_base::beg);
		md3TexCoord_t currTexCoord;
		for(int i = 0; i < currMesh.header.vertex_num; ++i) {
			infile.read((char*)&currTexCoord, sizeof(md3TexCoord_t));
			currMesh.texcoords.push_back(currTexCoord);
		}

		infile.seekg(headerstart + currMesh.header.vertex_start, ios_base::beg);
		md3Vertex_t currVertex;
		md3Normal_t currNormal;
		currMesh.normals.clear();
		for(int i = 0; i < currMesh.header.vertex_num * currMesh.header.frame_num; ++i) {
			infile.read((char*)&currVertex, sizeof(md3Vertex_t));
			currNormal.x = cos(2*pi*currVertex.normals[1]/255.0)*sin(2*pi*currVertex.normals[0]/255.0);
			currNormal.y = sin(2*pi*currVertex.normals[1]/255.0)*sin(2*pi*currVertex.normals[0]/255.0);
			currNormal.z = cos(2*pi*currVertex.normals[0]/255.0);
			currMesh.vertices.push_back(currVertex);
			currMesh.normals.push_back(currNormal);
		}
		infile.seekg(headerstart + currMesh.header.mesh_size, ios_base::beg);
		m_meshes.push_back(currMesh);
	}

	string skinfile = m_filename.substr(0, m_filename.length() - 4);
	skinfile += "_" + m_skin + ".skin";
	
	ifstream skininfile(skinfile.c_str(), ios::in);
	string in;
	string tok;
	vector<string> toks;
	while((skininfile >> in)) {
		while(in.find(',') != string::npos)
			in.replace(in.find(','), 1, " ");
		istringstream linein(in);

		toks.clear();
		while((linein >> tok)) {
			if(tok != "")
				toks.push_back(tok);
		}
		if(toks.size() == 2) {
			for(int i = 0; i < m_header.mesh_num; i++) {
				char buf[1024];
				char* bufptr = buf;
				if(string(m_meshes[i].header.name) == toks[0]) {
					strncpy(bufptr, m_filename.c_str(), 1024);
#ifndef WIN32
					bufptr = dirname(bufptr);
#endif
					string texfile = bufptr;
					strncpy(buf, toks[1].c_str(), 1024);
#ifndef WIN32
					bufptr = basename(bufptr);
#endif
					texfile += "/";
					texfile += bufptr;
					if(m_textures.find(toks[1]) == m_textures.end()) {
						cout << "'" << toks[1] << "'" << endl;
						m_textures[toks[1]] = Texture();
						if(!m_textures[toks[1]].load(texfile))
							cout << "Failed to load texture: " << texfile << endl;
					}
					m_meshes[i].tex = toks[1];
				}
			}
		}
	}
	
	return true;
}

bool MD3Model::loadGL()
{
	cout << "LoadGL called" << endl;
	for(map<string, Texture>::iterator ti = m_textures.begin(); ti != m_textures.end(); ti++) {
		cout << "'" << ti->first << "'" << endl;
		ti->second.loadGL();
	}
	return true;
}

void MD3Model::setFrame(int frame1, int frame2, double blend)
{
	m_frame1 = frame1;
	m_frame2 = frame2;
	m_blend = blend;
}

void MD3Model::link(MD3Model * m)
{
	m_links.push_back(m);
}

void MD3Model::draw()
{
	int f1 = m_frame1;
	int f2 = m_frame2;
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float pi = 3.1415;
	glEnable(GL_NORMALIZE);
	for(int j = 0; j < m_header.mesh_num; j++) {
		if(m_textures.find(m_meshes[j].tex) != m_textures.end()) {
			m_textures[m_meshes[j].tex].use();
		}
		glBegin(GL_TRIANGLES);
		for(int i = 0; i < m_meshes[j].triangles.size(); i++) {
			int v = m_meshes[j].header.vertex_num;
			md3Vertex_t v0 = blendVertex(m_meshes[j].vertices[v*f1+m_meshes[j].triangles[i].vertices[0]], 
											m_meshes[j].vertices[v*f2+m_meshes[j].triangles[i].vertices[0]], m_blend);
			md3Vertex_t v1 = blendVertex(m_meshes[j].vertices[v*f1+m_meshes[j].triangles[i].vertices[1]], 
											m_meshes[j].vertices[v*f2+m_meshes[j].triangles[i].vertices[1]], m_blend);
			md3Vertex_t v2 = blendVertex(m_meshes[j].vertices[v*f1+m_meshes[j].triangles[i].vertices[2]], 
											m_meshes[j].vertices[v*f2+m_meshes[j].triangles[i].vertices[2]], m_blend);
			md3Normal_t n0 = m_meshes[j].normals[v*f1+m_meshes[j].triangles[i].vertices[0]];
			md3Normal_t n1 = m_meshes[j].normals[v*f1+m_meshes[j].triangles[i].vertices[1]];
			md3Normal_t n2 = m_meshes[j].normals[v*f1+m_meshes[j].triangles[i].vertices[2]];
			md3TexCoord_t tv0 = m_meshes[j].texcoords[m_meshes[j].triangles[i].vertices[0]];
			md3TexCoord_t tv1 = m_meshes[j].texcoords[m_meshes[j].triangles[i].vertices[1]];
			md3TexCoord_t tv2 = m_meshes[j].texcoords[m_meshes[j].triangles[i].vertices[2]];
			glNormal3f(n2.x, n2.y, n2.z);
			glTexCoord2f(tv2.coords[0], tv2.coords[1]);
			glVertex3f(v2.coords[0]*1.0/64, v2.coords[1]*1.0/64, v2.coords[2]*1.0/64);
			glNormal3f(n1.x, n1.y, n1.z);
			glTexCoord2f(tv1.coords[0], tv1.coords[1]);
			glVertex3f(v1.coords[0]*1.0/64, v1.coords[1]*1.0/64, v1.coords[2]*1.0/64);
			glNormal3f(n0.x, n0.y, n0.z);
			glTexCoord2f(tv0.coords[0], tv0.coords[1]);
			glVertex3f(v0.coords[0]*1.0/64, v0.coords[1]*1.0/64, v0.coords[2]*1.0/64);
		}
		glEnd();
	}

	double b1 = (1 - m_blend);
	double b2 = m_blend;
	float mult[4][4] = { 0.0 };
	mult[3][3] = 1.0;
	for(int j = 0; j < m_header.tag_num; j++) {
		for(int l = 0; l < m_links.size(); l++) {
			if(string(m_links[l]->m_tagname) == string(m_frames[f1].tags[j].name)) {
				glPushMatrix();
				if(b2 < 0.1)
				glTranslatef(m_frames[f1].tags[j].origin[0],
								m_frames[f1].tags[j].origin[1],
								m_frames[f1].tags[j].origin[2]);
				else if(b2 > 0.9)
				glTranslatef(m_frames[f2].tags[j].origin[0],
								m_frames[f2].tags[j].origin[1],
								m_frames[f2].tags[j].origin[2]);
				else
				glTranslatef(b1*m_frames[f1].tags[j].origin[0]
								+ b2*m_frames[f2].tags[j].origin[0],
								b1*m_frames[f1].tags[j].origin[1]
								+ b2*m_frames[f2].tags[j].origin[1],
								b1*m_frames[f1].tags[j].origin[2]
								+ b2*m_frames[f2].tags[j].origin[2]);
				for(int r = 0; r < 3; r++) {
					for(int c = 0; c < 3; c++) {
						if(b2 < 0.1)
							mult[r][c] = m_frames[f1].tags[j].axis[r][c];
						else if(b2 > 0.9)
							mult[r][c] = m_frames[f2].tags[j].axis[r][c];
						else 
						mult[r][c] = b1*m_frames[f1].tags[j].axis[r][c] 
							+b2*m_frames[f2].tags[j].axis[r][c];
					}
				}
				glMultMatrixf((float*)mult);
				m_links[l]->draw();
				glPopMatrix();
			}
		}
	}
}

void MD3Model::printInfo() const
{
	printHeader();
	printFrames();
	printMeshes();
	cout << "Found Tag: " << m_tagname << endl;
}

void MD3Model::printHeader() const
{
	cout << "\nMD3 Header:" << endl;
	cout << "ID:\t\t\t" << m_header.ID << endl;
	cout << "Version:\t\t" << m_header.version << endl;
	cout << "Filename:\t\t" << m_header.name << endl;
	cout << "Number of Frames:\t" << m_header.frame_num << endl;
	cout << "Number of Tags:\t\t" << m_header.tag_num << endl;
	cout << "Number of Meshes:\t" << m_header.mesh_num << endl;
	cout << "Max Num Skins:\t\t" << m_header.max_skin_num << endl;
	cout << "Start Frame Position:\t" << m_header.frame_start << endl;
	cout << "Start Tag Position:\t" << m_header.tag_start << endl;
	cout << "Start Mesh Position:\t" << m_header.mesh_start << endl;
	cout << "Size of File:\t\t" << m_header.file_size << endl;
}

void MD3Model::printFrames() const
{
	for (vector<md3Frame_t>::const_iterator fi = m_frames.begin(); fi != m_frames.end(); ++fi) {
		cout << "\nFrame:" << endl;
		cout << "Min Bound: (" << fi->header.min_bound[0] << "," << fi->header.min_bound[1] << "," << fi->header.min_bound[2] << ")" << endl;
		cout << "Max Bound: (" << fi->header.max_bound[0] << "," << fi->header.max_bound[1] << "," << fi->header.max_bound[2] << ")" << endl;
		cout << "Origin: (" << fi->header.origin[0] << "," << fi->header.origin[1] << "," << fi->header.origin[2] << ")" << endl;
		cout << "Bound Radius: " << fi->header.bound_radius << endl;
		cout << "Name: " << fi->header.name << endl;
		for (vector<md3Tag_t>::const_iterator ti = fi->tags.begin(); ti != fi->tags.end(); ++ti) {
			cout << "Tag name: " << ti->name << endl;
			cout << "Origin: (" << ti->origin[0] << "," << ti->origin[1] << "," << ti->origin[2] << ")" << endl;
			cout << "Axis: (" << ti->axis[0][0] << "," << ti->axis[0][1] << "," << ti->axis[0][2] << ")" << endl;
			cout << "Axis: (" << ti->axis[1][0] << "," << ti->axis[1][1] << "," << ti->axis[1][2] << ")" << endl;
			cout << "Axis: (" << ti->axis[2][0] << "," << ti->axis[2][1] << "," << ti->axis[2][2] << ")" << endl;
		}
	}
}

void MD3Model::printMeshes() const
{
	for (vector<md3Mesh_t>::const_iterator mi = m_meshes.begin(); mi != m_meshes.end(); ++mi) {
		cout << "\nMesh:" << endl;
		cout << "ID: " << mi->header.id << endl;
		cout << "Name: " << mi->header.name << endl;
		cout << "Number of Frames: " << mi->header.frame_num << endl;
		cout << "Number of Skins: " << mi->header.skin_num << endl;
		cout << "Number of Vertices: " << mi->header.vertex_num << endl;
		cout << "Number of Triangles: " << mi->header.triangle_num << endl;
		cout << "Triangles Start: " << mi->header.triangle_start << endl;
		cout << "Skins Start: " << mi->header.skin_start << endl;
		cout << "Texture Coords Start: " << mi->header.texcoord_start << endl;
		cout << "Vertices Start: " << mi->header.vertex_start << endl;
		cout << "Mesh Size: " << mi->header.mesh_size << endl;
		for (vector<md3Skin_t>::const_iterator si = mi->skins.begin(); si != mi->skins.end(); ++si) {
			cout << "Name: " << si->name << endl;
		}
	}
	
}

MD3Model::md3Vertex_t MD3Model::blendVertex(md3Vertex_t & v1, md3Vertex_t & v2, 
		double b)
{
	if(b < 0.1)
		return v1;
	else if(b > 0.9)
		return v2;
	double b1 = 1 - b;
	md3Vertex_t v;
	v.coords[0] = (signed short)(b1*v1.coords[0] + b*v2.coords[0]);
	v.coords[1] = (signed short)(b1*v1.coords[1] + b*v2.coords[1]);
	v.coords[2] = (signed short)(b1*v1.coords[2] + b*v2.coords[2]);
	return v;
}

// End of file md3model.cpp

