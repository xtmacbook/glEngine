#pragma once
#ifndef __OpenGL__EngineLoad__
#define __OpenGL__EngineLoad__

#include "decl.h"
#include "IRenderNode.h"
#include "texture.h"
#include <vector>
#include "mesh.h"


namespace IO
{
	struct LModelInfo
	{
		std::vector<Mesh_SP > meshs_;
		std::vector<Texture_Sp> textures_;
	};

	struct ImageOption
	{
		bool clamped_ = false;
	};

	class LIBENIGHT_EXPORT EngineLoad
	{
	public:
		enum ModelLoadType
		{
			ML_ASSIMP,
			ML_NONEPLUG
		};
		static bool loadNode(const char*file, LModelInfo&, ModelLoadType = ML_ASSIMP);
		static base::SmartPointer<base::Image> loadImage(const char*file, const ImageOption&option = {true});
	};
};

#endif
