#include "axes.h"
#include "mesh.h"
#include "engineLoad.h"
#include "resource.h"
#include "shader.h"
#include "renderNode.h"
#include "log.h"
#include "gls.h"

AexsGeometry::AexsGeometry():MeshGeometry()
{

}

void AexsGeometry::initGeometry()
{
	for (int i = 0; i < meshs_.size(); i++)
	{
		Mesh_SP ms = meshs_[i];
		ms->call_ = DRAW_ELEMENTS;
		ms->type_ = GL_UNSIGNED_SHORT;

		std::vector<Vertex>& vs = ms->RVertex(0);
		for (int j = 0; j < vs.size(); j++)
		{
			if (i == 0) //
			{
				vs[j].Normal.x = 0.45;
				vs[j].Normal.y = 0.0;
				vs[j].Normal.z = 0.0;
			}
			else if (i == 1)
			{
				vs[j].Normal.x = 0.0;
				vs[j].Normal.y = 0.45;
				vs[j].Normal.z = 0.0;
			}
			else if (i == 2)
			{
				vs[j].Normal.x = 0.0;
				vs[j].Normal.y = 0.0;
				vs[j].Normal.z = 0.45;
			}

			else if (i == 3)
			{
				vs[j].Normal.x = 0.9;
				vs[j].Normal.y = 0.0;
				vs[j].Normal.z = 0.0;
				vs[j].Position.x += 1.0;
			}
			else if (i == 4)
			{
				vs[j].Normal.x = 0.0;
				vs[j].Normal.y = 0.9;
				vs[j].Normal.z = 0.0;
				vs[j].Position.y += 1.0;
			}
			else if (i == 5)
			{
				vs[j].Normal.x = 0.0;
				vs[j].Normal.y = 0.0;
				vs[j].Normal.z = 0.9;
				vs[j].Position.z += 1.0;
			}
			else
			{ }
		}
		//此处用normal作为color
		ms->box().init();
		ms->computeBox();
		IRenderMeshObj_SP irmsp = createRenderMeshObj();
		setupMesh(ms, irmsp);
		mesh_obj_.push_back(irmsp);
	}
	computeBoundingBox(NULL);
}


IRenderNode_SP  createAxesRenderNode()
{
	IO::LModelInfo data;
	std::string modelPath = get_model_BasePath() + "axes.obj";
 
	IRenderNode_SP axesNode;
	if (IO::EngineLoad::loadNode(modelPath.c_str(), data))
	{
		if (!data.meshs_.empty())
		{
			axesNode = new RenderNode;
			base::SmartPointer<AexsGeometry> mg = new AexsGeometry();
			mg->meshs_ = data.meshs_;
			mg->initGeometry();
			if (axesNode)
				((RenderNode*)axesNode.addr())->setGeometry(mg);
		}
	}
	return axesNode;
}

Shader_SP createAxesRenderNodeShader()
{
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 normal;"

		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"out vec3 vcolor;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"   vcolor = normal;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"out vec4 color;"
		"in vec3 vcolor;"
		"void main()"
		"{"
		"color = vec4(vcolor, 1.0f);"
		"}";

	Shader_SP axesShader = new Shader;
	axesShader->loadShaderSouce(vertShder, fragShader, NULL);
	return axesShader;
}
 
