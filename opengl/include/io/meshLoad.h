#pragma once
#ifndef __OpenGL__MeshLoad__
#define __OpenGL__MeshLoad__

#include "decl.h"
#include "type.h"
#include "mesh.h"
#include "texture.h"
#include "engineLoad.h"
#include  <vector>

namespace IO
{

	class NodeFile
    {
	public:

		static bool assimpLoadModel(const char*path, LModelInfo&);

		static bool loadOBJ(const char * path,
			std::vector < math::V3f > & out_vertices,
			std::vector < math::V2f > & out_uvs,
			std::vector < math::V3f > & out_normals);
        //static bool loadObjByAssimp(const char*fileName, std::vector<Model>&meshs, unsigned int steps);
    };
}
#endif
