#include "meshLoad.h"
#include "mesh.h"
#include "texture.h"
#include "log.h"
#include "IGeometry.h"

#include <sstream>

#ifdef ENGINE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif // ENGINE_ASSIMP


#ifdef ENGINE_ASSIMP
void loadMaterialTextures(aiMaterial* mat, aiTextureType type, TEXTURE_TYPE t, Mesh_SP& m, const std::string& dir, std::vector<Texture_Sp>& ts)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		{   // if texture hasn't been loaded already, load it
			bool skip = false;
			for (unsigned int j = 0; j < ts.size(); j++)
			{
				if (std::strcmp(ts[j]->fileName_.c_str(), (dir + "/" + str.C_Str()).c_str()) == 0)
				{
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				ts.push_back(new Texture((dir + "/" + str.C_Str()).c_str()));
				{
					Mesh* lm = m;
					TMesh* tm = dynamic_cast<TMesh*>(lm);
					if (tm)
						tm->t_indices_.push_back(ts.size() - 1);
				}
				ts[ts.size() - 1]->setType(t);
			}
		}
	}
}

void processMesh(aiMesh* mesh, const aiScene* scene, Mesh_SP& m, std::vector<Texture_Sp>& textures, const std::string& dir)
{
	// data to fill
	m->rmode() = mesh->mPrimitiveTypes;
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		V3f point; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
					// positions
		m->RFVF() |= FVF_XYZ;
		point.x = mesh->mVertices[i].x;
		point.y = mesh->mVertices[i].y;
		point.z = mesh->mVertices[i].z;
		vertex.Position = point;
		// normals
		if (mesh->mNormals)
		{
			m->RFVF() |= FVF_NORMAL;
			point.x = mesh->mNormals[i].x;
			point.y = mesh->mNormals[i].y;
			point.z = mesh->mNormals[i].z;
			vertex.Normal = point;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			V2f vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			m->RFVF() |= FVF_TEXT0;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		if (mesh->mTangents)
		{
			// tangent
			m->RFVF() |= FVF_TANGENT;
			point.x = mesh->mTangents[i].x;
			point.y = mesh->mTangents[i].y;
			point.z = mesh->mTangents[i].z;
			vertex.Tangent = point;
		}
		if (mesh->mBitangents)
		{
			// bitangent
			m->RFVF() |= FVF_BITANGENT;
			point.x = mesh->mBitangents[i].x;
			point.y = mesh->mBitangents[i].y;
			point.z = mesh->mBitangents[i].z;
			vertex.Bitangent = point;
		}
		m->addVertex(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			m->addIndices(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	loadMaterialTextures(material, aiTextureType_DIFFUSE, T_DIFFUSE, m, dir, textures);
	loadMaterialTextures(material, aiTextureType_SPECULAR, T_SPECULAR, m, dir, textures);
	loadMaterialTextures(material, aiTextureType_HEIGHT, T_NORMAL, m, dir, textures);
	loadMaterialTextures(material, aiTextureType_AMBIENT, T_HEIGHT, m, dir, textures);

	m->computeBox();
}



void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh_SP >& meshs, std::vector<Texture_Sp>& textures, const std::string& dir)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		base::SmartPointer<Mesh> tmp(new TMesh);
		tmp->call_ = DRAW_ELEMENTS;
		meshs.push_back(tmp);
		processMesh(mesh, scene, meshs[meshs.size() - 1], textures, dir);

	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshs, textures, dir);
	}
}

#endif // ENGINE_ASSIMP


bool IO::NodeFile::assimpLoadModel(const char* path, LModelInfo& node)
{
#ifdef ENGINE_ASSIMP

	Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::stringstream err;
		err << "ERROR::ASSIMP:: " << importer.GetErrorString();
		Log::printMessage(err.str().c_str());
		return false;
	}
	std::string tp(path);
	std::string directory = tp.substr(0, tp.find_last_of('/'));
	// process ASSIMP's root node recursively

	processNode(scene->mRootNode, scene, node.meshs_, node.textures_, directory);

	return true;
#endif // ENGINE_ASSIMP
	return false;
}
