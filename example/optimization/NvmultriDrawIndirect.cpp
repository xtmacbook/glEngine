
#include <scene.h>
#include "resource.h"
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <glinter.h>
#include <gli.h>
#include <log.h>
#include <renderNode.h>
#include <engineLoad.h>
#include <texture.h>
#include <helpF.h>
#include <flashBuffer.h>
#include <vertex.h>
#include <log.h>

static const int  MAX_GRID_SIZE = 500;
static const int  DEFAULT_GRID_SIZE = 100;
static const int  MAX_INSTANCES = 20;
static const int  INSTANCE_SEPERATION = 10.0f;

V4f lightPositionEye(1.0f, 1.0f, 1.0f, 0.0f);

GLuint vao_;

class NVTestScene : public Scene
{
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual void					render(PassInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);

private:
	void							drawSkyBox(PassInfo&);
	void							drawSingleHouse(PassInfo&);

	void							setConstant();

	void							createMultiDrawParameters();
	void							setupMultiDrawParameters();
	void							setupMultiDrawIndirectData();
	void							setupMultipleModelData();
	int							    setupVertexAttribute(const int fvf);
	void							getGridPoints(unsigned int        j,
		std::vector<float>& Offsets);

	FlashBuffer<DrawElementsIndirectCommand>*							indirectBuffer_ = nullptr;

	unsigned int  gridSize_;

	GLuint                       m_SizeofIndexBuffer;
	GLuint                       m_SizeofVertexBuffer;
	GLuint                       m_OffsetofInstanceBuffer;

	Mesh_SP						 mesh_;
	std::vector<float>           m_Offsets;

};

bool NVTestScene::initSceneModels(const SceneInitInfo&)
{
	gridSize_ = DEFAULT_GRID_SIZE;

	IO::LModelInfo data;
	std::string modelPath = get_model_BasePath() + "house.obj";
	if (IO::EngineLoad::loadNode(modelPath.c_str(), data))
	{
		if (!data.meshs_.empty())
		{
			Mesh_SP mesh = data.meshs_[0];
			mesh_ = mesh;
			mesh_->RFVF() = FVF_NONE;
			mesh_->RFVF() |= FVF_XYZ;
			mesh_->RFVF() |= FVF_NORMAL;
			mesh_->RFVF() |= FVF_TEXT0;

			setConstant();
			glGenVertexArrays(1, &vao_);
			createMultiDrawParameters();
			setupMultiDrawParameters();
			setupMultiDrawIndirectData();
			CHECK_GL_ERROR;
		}
	}

	Mesh_SP mesh = new Mesh;
	mesh->call_ = DRAW_ARRAYS;
	mesh->rmode() = GL_TRIANGLE_STRIP;
	mesh->RFVF() |= FVF_XYZ;
	Vertex v;
	v.Position = V3f(-1.0f, -1.0f, -1.0f);
	mesh->addVertex(v);
	v.Position = V3f(1.0f, -1.0f, -1.0f);
	mesh->addVertex(v);
	v.Position = V3f(-1.0f, 1.0f, -1.0f);
	mesh->addVertex(v);
	v.Position = V3f(1.0f, 1.0f, -1.0f);
	mesh->addVertex(v);

	MeshGeometry_Sp mg = new MeshGeometry;
	mg->addMesh(mesh);
	mg->initGeometry();
	RenderNode_SP rn = new RenderNode;
	if (rn) rn->setGeometry(mg);
	addRenderNode(rn);

	return true;
}

void NVTestScene::render(PassInfo&ps)
{
	drawSkyBox(ps);
	drawSingleHouse(ps);
}

bool NVTestScene::initTexture(const SceneInitInfo&)
{
	std::string tx = get_texture_BasePath() + "sky_cube.dds";
	Texture* curTexObj = new Texture(tx.c_str());
	curTexObj->target_ = GL_TEXTURE_CUBE_MAP;
	if (curTexObj->loadData())
	{
		curTexObj->createObj();
		curTexObj->bind();
		curTexObj->mirrorRepeat();
		curTexObj->filterLinear();
		if (curTexObj->context(NULL))
		{
			curTexObj->unBind();
			texturesObj_.push_back(curTexObj);
		}
		else return false;
		CHECK_GL_ERROR;
	}

	tx = get_texture_BasePath() + "windmill_diffuse1.dds";
	curTexObj = new Texture(tx.c_str());
	if (curTexObj->loadData())
	{
		curTexObj->createObj();
		curTexObj->bind();
		curTexObj->mirrorRepeat();
		curTexObj->filterLinear();
		if (curTexObj->context(NULL))
		{
			curTexObj->unBind();
			texturesObj_.push_back(curTexObj);
		}
		else return false;
		CHECK_GL_ERROR;
	}
	return true;
}

bool NVTestScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);
	Shader * currentShader = shaders_[0];
	if (currentShader->loadShaders("skybox/skyboxcolor.vert", "skybox/skyboxcolor.frag", nullptr) == -1) return false;
	currentShader->turnOn();
	int eunit = currentShader->getVariable("skybox");
	glUniform1i(eunit, 0);

	const char sceneVertexSahder[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"

		"layout(location = 0) in vec4 a_vPosition;   // In BaseShader\n"
		"layout(location = 1) in vec3 a_vNormal;     // In BaseProjNormalShader\n"
		"layout(location = 2) in vec2 a_vTexCoord;   // In SceneColorShader\n"

		"out vec2 v_vTexcoord;\n"
		"out float v_fLightIntensity;\n"


		"uniform mat4            projection;      // In BaseProjectionShader	 \n"
		"uniform mat4            u_mNormalMat;          // In BaseProjNormalShader	 \n"
		"uniform mat4            view;       // In SceneColorShader		 \n"
		"uniform mat4            model;       // In SceneColorShader		\n "

		"uniform vec4            u_vLightPosition;      // In SceneColorShader		\n "
		"uniform float           u_fLightAmbient;      // In SceneColorShader		 \n"
		"uniform float           u_fLightDiffuse;      // In SceneColorShader		 \n"
		"uniform float           u_fLightSpecular;     // In SceneColorShader		 \n"
		"uniform float           u_fLightShininess;    // In SceneColorShader		 \n"
		"uniform vec2            u_vPosition;										 \n"


		"void main()\n"
			"{\n"
			"vec4 vPosEyeSpace = view * model * (a_vPosition + vec4(u_vPosition.x, 0.0f, u_vPosition.y, 0.0f));\n"

			"gl_Position = projection * vPosEyeSpace;\n"

			"vec3 normal = normalize(mat3(view * model) * a_vNormal);\n"

			"vec3 lDir;\n"

			"if (u_vLightPosition.w == 0.0)\n"
			"	lDir = normalize(u_vLightPosition.xyz);\n"
			"else\n"
			"	lDir = normalize(u_vLightPosition.xyz - vPosEyeSpace.xyz);\n"

		//here half vector = normalize (lightVector+eyeVector)
		//but the vPosEye if vertexposition in eye space (eye = 0,0,0)
		//hence vector from vertex to eye = [-vPosEye]

		"vec3 halfVector = normalize(lDir - normalize(vPosEyeSpace.xyz));\n"
		"v_fLightIntensity = u_fLightAmbient;\n"

		"float NdotL = max(dot(normal,lDir),0.0), NdotHV;\n"

		"if (NdotL > 0.0)\n"
		"{\n"
		"v_fLightIntensity += u_fLightDiffuse * NdotL;\n"
		"NdotHV = max(dot(normal, halfVector),0.0);\n"
		"v_fLightIntensity += u_fLightSpecular * pow(NdotHV, u_fLightShininess);\n"
		"}\n"
		"v_vTexcoord = a_vTexCoord;\n"
		"}"

	};
	const char scenescenecolormdiV[] =
	{
		"#version 330 core\n"

		"precision mediump float;\n"

		// INPUT
		"layout(location = 0) in vec3 a_vPosition;   // In BaseShader\n"
		"layout(location = 1) in vec3 a_vNormal;     // In BaseProjNormalShader\n"
		"layout(location = 2) in vec2 a_vTexCoord;   // In SceneColorShader\n"
		"layout(location = 3) in vec2 a_vInstance;   // In SceneColorShader\n"

		// OUTPUT
"out vec2 v_vTexcoord;\n"
"out float v_fLightIntensity;\n"

// UNIFORMS
"uniform mat4            projection;      // In BaseProjectionShader\n"
"uniform mat4            view;       // In SceneColorShader		 \n"
"uniform mat4            model;       // In SceneColorShader		\n "

"uniform vec4            u_vLightPosition;      // In SceneColorShader\n"
"uniform float           u_fLightAmbient;      // In SceneColorShader\n"
"uniform float           u_fLightDiffuse;      // In SceneColorShader\n"
"uniform float           u_fLightSpecular;     // In SceneColorShader\n"
"uniform float           u_fLightShininess;    // In SceneColorShader\n"

	"void main()\n"
	"{\n"
	"vec4 vPosEyeSpace = view * model * (vec4(a_vPosition,1.0) + vec4(a_vInstance.x, 0.0f, a_vInstance.y, 0.0f));\n"

	"	gl_Position = projection * view * vPosEyeSpace;\n"

	"	vec3 normal = normalize(mat3(view * model) * a_vNormal);\n"

	"vec3 lDir;\n"

	"if (u_vLightPosition.w == 0.0)\n"
	"	lDir = normalize(u_vLightPosition.xyz);\n"
	"else\n"
	"	lDir = normalize(u_vLightPosition.xyz - vPosEyeSpace.xyz);\n"

		//here half vector = normalize (lightVector+eyeVector)
		//but the vPosEye if vertexposition in eye space (eye = 0,0,0)
		//hence vector from vertex to eye = [-vPosEye]

		"vec3 halfVector = normalize(lDir - normalize(vPosEyeSpace.xyz));\n"
		"v_fLightIntensity = u_fLightAmbient;\n"

		"float NdotL = max(dot(normal,lDir),0.0), NdotHV;\n"

		"if (NdotL > 0.0)\n"
		"{\n"
		// cosine (dot product) with the normal

	"v_fLightIntensity += u_fLightDiffuse * NdotL;\n"
	"NdotHV = max(dot(normal, halfVector),0.0);\n"
	"v_fLightIntensity += u_fLightSpecular * pow(NdotHV, u_fLightShininess);\n"
	"}\n"

	"	v_vTexcoord = a_vTexCoord;\n"
	"}"

	};
	const char scenecolorvanillaF[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"

		"in vec2 v_vTexcoord;\n"
		"in float v_fLightIntensity;\n"

		"out vec4 o_vFragColor;\n"

		"uniform sampler2D u_tDiffuseTex; // In SceneColorShader\n"
		"void main()\n"
			"{"
		"o_vFragColor = texture(u_tDiffuseTex, v_vTexcoord) * v_fLightIntensity;\n"
			"o_vFragColor.a = 1.0;\n"
		   "}"

	};

	shaders_.push_back(new Shader);
	currentShader = shaders_[1];
	if (currentShader->loadShaderSouce(sceneVertexSahder, scenecolorvanillaF, nullptr) == -1) return false;
	currentShader->turnOn();
	currentShader->setFloat4(currentShader->getVariable("u_vLightPosition"), lightPositionEye.x, lightPositionEye.y, lightPositionEye.z, lightPositionEye.w);
	currentShader->setFloat(currentShader->getVariable("u_fLightAmbient"), 0.1f);
	currentShader->setFloat(currentShader->getVariable("u_fLightDiffuse"), 0.7f);
	currentShader->setFloat(currentShader->getVariable("u_fLightSpecular"), 1.0f);
	currentShader->setFloat(currentShader->getVariable("u_fLightShininess"), 64.0f);

	shaders_.push_back(new Shader);
	currentShader = shaders_[2];
	if (currentShader->loadShaderSouce(scenescenecolormdiV, scenecolorvanillaF, nullptr) == -1) return false;
	currentShader->turnOn();
	currentShader->setFloat4(currentShader->getVariable("u_vLightPosition"), lightPositionEye.x, lightPositionEye.y, lightPositionEye.z, lightPositionEye.w);
	currentShader->setFloat(currentShader->getVariable("u_fLightAmbient"), 0.1f);
	currentShader->setFloat(currentShader->getVariable("u_fLightDiffuse"), 0.7f);
	currentShader->setFloat(currentShader->getVariable("u_fLightSpecular"), 1.0f);
	currentShader->setFloat(currentShader->getVariable("u_fLightShininess"), 64.0f);
	currentShader->turnOff();

	return true;
}

void NVTestScene::drawSkyBox(PassInfo&ps)
{
	Shader * skyboxShader = shaders_[0];
	skyboxShader->turnOn();
	GLint location = skyboxShader->getVariable("projection");
	if (location != -1)
		skyboxShader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera_->getProjectionMatrix()));

	location = skyboxShader->getVariable("verseViewMat");
	if (location != -1)
		skyboxShader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(math::inverse((camera_->getViewMatrix()))));

	glActiveTexture(GL_TEXTURE0);
	texturesObj_[0]->bind();
	getRenderNode(0)->render(skyboxShader, ps);
	skyboxShader->turnOff();
	CHECK_GL_ERROR;
}

void NVTestScene::drawSingleHouse(PassInfo&pi)
{

	Shader * shader = shaders_[2];
	shader->turnOn();
	initUniformVal(shader);
	GLint loc = shader->getVariable("model");
	if (loc != -1)
	{
		shader->setMatrix4(loc, 1, GL_FALSE, math::value_ptr(Matrixf(1.0f)));
	}
	glActiveTexture(GL_TEXTURE0);
	texturesObj_[1]->bind();
	setupMultiDrawParameters();
	indirectBuffer_->bind();
	glBindVertexArray(vao_);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, NULL, gridSize_ * gridSize_, 0);
	glBindVertexArray(0);

	shader->turnOff();
}

void NVTestScene::setConstant()
{
	int numofIndices = mesh_->indices_[0].size();
	int numofVertex = mesh_->vertices_[0].size();

	m_SizeofIndexBuffer = 0;
	m_SizeofVertexBuffer = 0;

	m_SizeofVertexBuffer += numofVertex * sizeof(Vertex) * MAX_INSTANCES;
	m_OffsetofInstanceBuffer = m_SizeofVertexBuffer;
	m_SizeofVertexBuffer += 2 * sizeof(GLfloat)  * MAX_GRID_SIZE * MAX_GRID_SIZE;

	m_SizeofIndexBuffer = numofIndices * sizeof(uint16);
}

void NVTestScene::createMultiDrawParameters()
{
	indirectBuffer_ = new FlashBuffer<DrawElementsIndirectCommand>(MAX_GRID_SIZE * MAX_GRID_SIZE);
	indirectBuffer_->setTarget(GL_DRAW_INDIRECT_BUFFER);
	indirectBuffer_->create_buffers(FlashBuffer<DrawElementsIndirectCommand>::ModeUnsynchronized);
}

void NVTestScene::setupMultiDrawParameters()
{
	unsigned int                        VertexOffset = 0;
	unsigned int                        IndexOffset = 0;

	indirectBuffer_->mapBuffer();

	DrawElementsIndirectCommand * data = indirectBuffer_->getBufferPtr();
	if (data)
	{
		for (int j = 0; j < gridSize_ * gridSize_; j++)
		{
			VertexOffset = (j % MAX_INSTANCES) * mesh_->vertices_[0].size();

			data[j].count_ = mesh_->indices_[0].size();
			data[j].instanceCount_ = 4;
			data[j].firstIndex_ = IndexOffset;
			data[j].baseVertex_ = VertexOffset;
			data[j].baseInstance_ = j;
		}
	}

	indirectBuffer_->flush_data();

	CHECK_GL_ERROR;
}

void NVTestScene::setupMultiDrawIndirectData()
{
	glBindVertexArray(vao_);

	GLuint bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);

	glBufferData(GL_ARRAY_BUFFER,
		m_SizeofVertexBuffer,
		NULL,
		GL_STATIC_DRAW);

	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		m_SizeofIndexBuffer,
		NULL,
		GL_STATIC_DRAW);

	setupMultipleModelData();

	int idx = setupVertexAttribute(mesh_->CFVF());

	// instance attr
	glEnableVertexAttribArray(idx);
	glVertexAttribPointer(idx, 2, GL_FLOAT, GL_FALSE, 0, ((char *)NULL + (m_OffsetofInstanceBuffer)));
	glVertexAttribDivisor(idx, 1);

	glBindVertexArray(0);
}

void NVTestScene::setupMultipleModelData()
{
	Vertex * data = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, m_SizeofVertexBuffer, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	uint16 * indexData = (uint16 *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, m_SizeofIndexBuffer, GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_WRITE_BIT);

	for (int i = 0; i < MAX_INSTANCES; i++)
	{
		const std::vector<Vertex>& vs = mesh_->vertices_[0];
		std::copy(vs.cbegin(), vs.cend(), data);

		Vertex * tmp = data;

		float scale = 1.0f + (rand() / (float)RAND_MAX) * 3.0f;
		for (int i = 0; i < vs.size(); i++)
		{
			(tmp->Position).y *= scale;
			tmp++;
		}

		data += vs.size();
	}

	const std::vector<uint16>&ids = mesh_->indices_[0];
	std::copy(ids.cbegin(), ids.cend(), indexData);

	for (int j = 0; j < MAX_GRID_SIZE; j++)
	{
		getGridPoints(j, m_Offsets);
	}

	float* pInstData = (float*)((void*)data);
	for (int j = 0; j < MAX_GRID_SIZE * MAX_GRID_SIZE; j++)
	{
		*(pInstData++) = m_Offsets[2 * j];
		*(pInstData++) = m_Offsets[(2 * j) + 1];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glUnmapBuffer(GL_ARRAY_BUFFER);

}

int NVTestScene::setupVertexAttribute(const int fvf)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	GLuint indx = 1;
	if (fvf & FVF_NORMAL)
	{
		// vertex normals
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		indx += 1;
	}
	if (fvf & FVF_TEXT0)
	{
		// vertex texture coords
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		indx += 1;
	}
	if (fvf & FVF_TANGENT)
	{
		// vertex tangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		indx += 1;
	}
	if (fvf & FVF_BITANGENT)
	{
		// vertex bitangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		indx += 1;
	}

	return indx;
}

void NVTestScene::getGridPoints(unsigned int j, std::vector<float>& Offsets)
{
	bool Loop;
	unsigned int Points, PointsDrawnUp, PointsDrawnAcross;

	Points = 1 + j;
	PointsDrawnUp = PointsDrawnAcross = 0;

	Loop = true;

	while (Loop)
	{
		Offsets.push_back(PointsDrawnAcross * INSTANCE_SEPERATION);
		Offsets.push_back(j * INSTANCE_SEPERATION);

		PointsDrawnAcross++;

		if (PointsDrawnAcross == Points)
		{
			Loop = false;

			continue;
		}

		Offsets.push_back(j * INSTANCE_SEPERATION);
		Offsets.push_back(PointsDrawnUp * INSTANCE_SEPERATION);

		PointsDrawnUp++;
	}
}

int main()
{
	NVTestScene * scene = new NVTestScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Nv_multriDrawIndirect";
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
	pCamera->setClipPlane(0.1f, 5000.0f);
	application.start();

	return 0;
}
