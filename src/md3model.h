/***************************************************************************
 * Brutal Chess
 * http://brutalchess.sf.net
 *
 * File : md3model.h 
 * Authors : Mike Cook, Joe Flint, Neil Pankey
 **************************************************************************/

#ifndef MD3MODEL_H
#define MD3MODEL_H

#include "texture.h"
#include "vector.h"

#include <map>
#include <string>
#include <vector>

/**
 * Represents and loads an MD3 Model.
 */
class MD3Model {

 public:
	
    /**
     * Construct a useless empty MD3Model.
     */
    MD3Model() :
        m_frame1(0), m_frame2(0), m_blend(0.0) {}
	
    /**
     * Loads and initializes the MD3Model from file.
     * @param filename - The file to load the model from.
     * @param skin - The skin to use.
     */
    MD3Model(const std::string& filename, const std::string& skin = "default")
		: m_frame1(0), m_frame2(0), m_blend(0.0)
		{ load(filename, skin); }

    /**
     * Loads the model from the specified file.
     * @param filename - The file to load the model from.
     * @param skin - The skin to use.
     */
    bool load(const std::string& filename, const std::string& skin = "default");
    
    /**
     * Loads the textures for the model.
     */
    bool loadGL();	

    /**
     * Sets the current frame of the animation.
     * @param frame1 - The frame to set the upper-body of the model to.
     * @param frame2 - The frame to set the lower-body of the model to.
     * @param blend - Alpha of the vertex to blend. 
     */
    void setFrame(int frame1, int frame2 = 0, double blend = 1.0);

    /**
     * Links the head, body, and legs of the model together. 
     */
	void link(MD3Model * m);

    /**
     * Draws the MD3Model.
     */
    void draw();

    // For debugging.
    void printInfo() const;
    void printHeader() const;
    void printFrames() const;
    void printMeshes() const;

    friend class Q3CharModel;

 private:

	int m_frame1, m_frame2;
	double m_blend;

	std::string                 m_tagname;
	std::vector<MD3Model*>      m_links;
	
	std::string     m_filename;
	std::string     m_skin;
	
	struct md3Header_t {
		char 	ID[4];
		int 	version;
		char 	name[68];
		int 	frame_num;
		int		tag_num;
		int		mesh_num;
		int		max_skin_num;
		int		frame_start;
		int		tag_start;
		int		mesh_start;
		int		file_size;
	};
	md3Header_t	m_header;
	
	struct md3Tag_t {
	   	char	name[64];   // Name of the tag
		float	origin[3];  // Relative Position of Tag
		float	axis[3][3]; // The direction the tag
                            // is facing relative to the
    };                      // rest of the model.
	
	struct md3FrameHeader_t {
		float	min_bound[3];
		float	max_bound[3];
		float	origin[3];
		float	bound_radius;
		char	name[16];
	};

	struct md3Frame_t {
		md3FrameHeader_t        header;
		std::vector<md3Tag_t>   tags;
	};
    std::vector<md3Frame_t>     m_frames;

	struct md3MeshHeader_t {
		char	id[4];
		char	name[68];
		int	frame_num;
		int	skin_num;
		int	vertex_num;
		int	triangle_num;
		int	triangle_start;
		int	skin_start;
		int	texcoord_start;
		int	vertex_start;
		int	mesh_size;
	};
	
	struct md3Skin_t {
		char name[68];
	};

	struct md3Triangle_t {
		int vertices[3];
	};

	struct md3TexCoord_t {
		float coords[2];
	};

	struct md3Vertex_t {
		signed short coords[3];
		unsigned char normals[2];
	};

	struct md3Normal_t {
		float x,y,z;
	};

	struct md3Mesh_t {
		md3MeshHeader_t	            header;
		std::vector<md3Skin_t>	    skins;
		std::vector<md3Triangle_t>  triangles;
		std::vector<md3TexCoord_t>  texcoords;
		std::vector<md3Vertex_t>    vertices;
		std::vector<md3Normal_t>    normals;
		std::string                 tex;
	};
	std::vector<md3Mesh_t> m_meshes;

	std::map<std::string, Texture> m_textures;
	
	md3Vertex_t blendVertex(md3Vertex_t & v1, md3Vertex_t & v2, double b);
};

#endif

// End of file md3model.h

