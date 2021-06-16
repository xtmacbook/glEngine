
#include "scene.h"
#include "RenderNode.h"
#include "shader.h"
#include "engineLoad.h"
#include "resource.h"
#include "gLApplication.h"
#include "camera.h"
#include "windowManager.h"
#include "log.h"
#include "helpF.h"
#include <Inputmanager.h>
#include <callback.h>
#include <text.h>
#include "gls.h"
#include "dynamicMesh.h"
#include <glinter.h>
#include <gli.h>


class TestScene :public Scene
{
public:
protected:
	virtual bool					initSceneModels(const SceneInitInfo&); 
	virtual bool					update();
	virtual void					render(PassInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);

public:
};

V3f Normal(0.0f, 0.0f, 0.0f);
V3f Tangent(0.0f, 0.0f, 0.0f);
V3f Bitangent(0.0f, 0.0f, 0.0f);

const Vertex gQuad[] =							{ {V3f(0.0f,0.0f,0.0f),Normal, V2f(0.0f,0.0f), Tangent,Bitangent},
												  {V3f(0.1f,0.0f,0.0f),Normal, V2f(1.0f,0.0f) ,Tangent,Bitangent},
												  {V3f(0.0f,0.1f,0.0f),Normal, V2f(0.0f,1.0f) ,Tangent,Bitangent},
												  {V3f(0.1f,0.1f,0.0f),Normal, V2f(1.0f,1.0f) ,Tangent,Bitangent}

};

const unsigned int gIndex[] = { 0,1,2,1,3,2 };

GLuint gArrayTexture;
class LocalMeshGeometry :public MeshGeometry
{
public:

	virtual void setupOtherBufferAndVA(int idx);
};

void LocalMeshGeometry::setupOtherBufferAndVA(int idx)
{
	GLuint vDrawId[100];
	for (GLuint i(0); i < 100; i++)
	{
		vDrawId[i] = i;
	}

	GLuint drawIdVBo;
	glGenBuffers(1, &drawIdVBo);
	glBindBuffer(GL_ARRAY_BUFFER, drawIdVBo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vDrawId), vDrawId, GL_STATIC_DRAW);

	glEnableVertexAttribArray(idx);
	glVertexAttribIPointer(idx, 1, GL_UNSIGNED_INT, 0, (GLvoid*)0);
	glVertexAttribDivisor(idx, 1);
}


bool TestScene::initSceneModels(const SceneInitInfo&)
{
	Vertex vVertex[400];
	int index(0);
	
	float xOffset(-0.95f);
	float yOffset(-0.95f);

	for (unsigned int i(0); i != 10; ++i)
	{
		for (unsigned int j(0); j != 10; ++j)
		{
			for (unsigned int k(0); k != 4; ++k)
			{
				vVertex[index].Position.x = gQuad[k].Position.x + xOffset;
				vVertex[index].Position.y = gQuad[k].Position.y + yOffset;
				vVertex[index].Position.z = gQuad[k].Position.z;
				vVertex[index].TexCoords = gQuad[k].Tangent;
				index++;
			}
			xOffset += 0.2f;
		}
		yOffset += 0.2f;
		xOffset = -0.95f;
	}

	Mesh_SP mesh = new Mesh;
	mesh->RFVF() |= FVF_XYZ;
	mesh->RFVF() |= FVF_TEXT0;
	mesh->mode_ = GL_TRIANGLES;
	mesh->call_ = DRAW_ELEMENT_INDIRECT;
	mesh->type_ = GL_UNSIGNED_SHORT;
	
	for (int i = 0; i < 400; i++)
	{
		mesh->addVertex(vVertex[i]);
	}
	for (int i = 0; i < 6; i++)
		mesh->addIndices(gIndex[i]);

	DrawElementsIndirectCommand vDrawCommand[100];
	for (unsigned int i(0); i < 100; ++i)
	{
		vDrawCommand[i].count_ = 6;
		vDrawCommand[i].instanceCount_ = 1;
		vDrawCommand[i].firstIndex_ = 0;
		vDrawCommand[i].baseVertex_ = i * 4;
		vDrawCommand[i].baseInstance_ = i;
	}

	mesh->eIndirectCom_ = vDrawCommand;
	mesh->numIndirect_ = 100;

	LocalMeshGeometry* mg = new LocalMeshGeometry;
	mg->addMesh(mesh);
	mg->initGeometry();

	RenderNode_SP rn = new RenderNode;
	if (rn) rn->setGeometry(mg);
	addRenderNode(rn);
	return true;
}

unsigned int vao_;
unsigned int pos_vbo_;
unsigned int color_vbo_;

bool TestScene::update()
{
	Scene::update();

	return true;
}

void TestScene::render(PassInfo&pi)
{
	Shader* shader = shaders_[0];
	shader->turnOn();
	initUniformVal(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, gArrayTexture);
	getRenderNode(0)->render(shader, pi);

	shader->turnOff();
	CHECK_GL_ERROR;
}



bool TestScene::initTexture(const SceneInitInfo&)
{
	glGenTextures(1, &gArrayTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, gArrayTexture);

	//Create storage for the texture. (100 layers of 1x1 texels)
	glTexStorage3D(GL_TEXTURE_2D_ARRAY,
		1,                    //No mipmaps as textures are 1x1
		GL_RGB8,              //Internal format
		1, 1,                 //width,height
		100                   //Number of layers
	);

	for (unsigned int i(0); i != 100; ++i)
	{
		//Choose a random color for the i-essim image
		GLubyte color[3] = { rand() % 255,rand() % 255,rand() % 255 };

		//Specify i-essim image
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,                     //Mipmap number
			0, 0, i,                 //xoffset, yoffset, zoffset (start from zoffset (lay),and update 1 count lay)
			1, 1, 1,                 //width, height, depth
			GL_RGB,                //format
			GL_UNSIGNED_BYTE,      //type
			color);                //pointer to data
	}

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	return true;
}

bool TestScene::initShader(const SceneInitInfo&)
{
	const char gVertexShaderSource[] =
	{
		"#version 330 core\n"
		"layout (location = 0 ) in vec3 position;\n"
		"layout (location = 1 ) in vec2 texCoord;\n"
		"layout (location = 2 ) in uint drawid;\n"

		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"out vec2 uv;\n"
		"flat out uint drawID;\n"
		"void main(void)\n"
		"{\n"
		"  gl_Position = projection * view * model * vec4(position,1.0);\n"
		"  uv = texCoord;\n"
		"  drawID = drawid;\n"
		"}\n"
	};

	const char gFragmentShaderSource[] =
	{
		"#version 330 core\n"

		"out vec4 color;\n"
		"in vec2 uv;\n"

		"flat in uint drawID;\n"
		"uniform sampler2DArray textureArray;\n"
		
		"void main(void)\n"
		"{\n"
		" color = texture(textureArray, vec3(uv.x,uv.y,drawID) );\n"
		"}\n"
	};

	Shader* shader = new Shader;
	shader->loadShaderSouce(gVertexShaderSource, gFragmentShaderSource, NULL);
	shaders_.push_back(shader);

	shader->turnOn();
	GLuint local = shader->getVariable("textureArray");
	if (local)
		shader->setInt(local, 0);
	shader->turnOff();

	return true;
}

int main()
{
	TestScene * scene = new TestScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "multriDrawIndirect";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	dc.glVersion_.glMaxJor_ = 3.0;
	dc.glVersion_.glMinJor_ = 3.0;

	application.initialize(&wc, &dc);

	if (!GL_X::isSupportExtension("GL_ARB_multi_draw_indirect"))
		exit(0);

	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
