

#include <vector>
#include <string>
#include <algorithm>

#include "windowManager.h"
#include "gLApplication.h"
#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "resource.h"
#include "log.h"
#include "engineLoad.h"
#include "image.h"
#include "helpF.h"
#include "callback.h"
#include "Inputmanager.h"
#include "renderNode.h"
#include <util.h>
#include <light.h>
#include <glinter.h>


#define	 RESTART_INDEX	0xffffffff

/************************************************************************/
/* https://www.scratchapixel.com/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot/bezier-curvechrom

Once we understand the principle of B¨¦zier curve extending the same technique to B¨¦zier surface is really straightforward. 
Rather than having 4 points we will define the surface with 16 points which you can see as a grid of 4x4 control points.
*/
/************************************************************************/

extern void createPolyTeapot(int level, std::vector<V3f>&Vs,
	std::vector<V3f>&Ns,
	std::vector<V2f>&st,
	std::vector<uint32>&indices);

class TessScene :public Scene
{
public:
	enum TessType
	{
		TESS_SHADER,
		GEOM_SHADER,
		CPU
	};
	virtual ~TessScene() {}
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);


public:
	virtual void					render(PassInfo&);

private:
	Shader_SP						tess_shader_ = nullptr;
	Shader_SP						baseShader_ = nullptr;

	GLuint							tess_vao_;

	GLuint							quad_vao_;
	int								quad_indiecs_count_;

	TessType						type_ = CPU;
};

bool TessScene::initShader(const SceneInitInfo&)
{
	const char vertex_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"

		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 normal;\n"
		"out vec3 vposition;"
		"out vec3 vnormal;"
		"void main()\n"
		"{"
			"vposition = position;\n"
			"vnormal = normal;"
		"}"

	};

	const char vertex_base_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"

		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec3 normal;\n"
		"out vec3 vnormal;"
		"uniform mat4 projection;"
		"uniform mat4 view;"
		"void main()\n"
		"{"
			"vnormal = normal;"
			"gl_Position = projection * view * vec4(position,1.0) ;"
		"}"

	};

	
	const char fr_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"
		"out vec4 o_vFragColor;\n"
		"void main()\n"
		"{"
			"o_vFragColor = vec4(1.0,0.0,0.0,1.0);\n"
		"}"

	};

	if(0)
	{
		tess_shader_ = new Shader;
		if (GL_FALSE == tess_shader_->addShader(Shader::VERTEX, vertex_shader, false)) return false;
		if (GL_FALSE == tess_shader_->addShader(Shader::TESS_TCL, "tessellation/tess_surface.cs")) return false;
		if (GL_FALSE == tess_shader_->addShader(Shader::TESS_TES, "tessellation/tess_surface.es")) return false;
		if (GL_FALSE == tess_shader_->addShader(Shader::FRAGMENT, fr_shader, false)) return false;
		CHECK_GL_ERROR;
		tess_shader_->linkProgram();
		tess_shader_->checkProgram();

		tess_shader_->turnOn();
		float bezier_basis_matrix[16] = {
			-1.0f, 3.0f, -3.0f ,1.0f,
			3.0f,-6.0f, 3.0f, 0.0f,
			-3.0f, 3.0f, 0.0f, 0.0f,
			1.0f, 0.0f ,0.0f ,0.0f
		};
		//reference from ://Oregon State University (Mike Bailey :Tessllation Shaders)
		GLint loc = tess_shader_->getVariable("B");
		if (loc != -1)
			tess_shader_->setMatrix4(loc, 1, GL_FALSE, bezier_basis_matrix);
		loc = tess_shader_->getVariable("BT");
		if (loc != -1)
			tess_shader_->setMatrix4(loc, 1, GL_TRUE, bezier_basis_matrix);
		tess_shader_->turnOff();

	}
	
	baseShader_ = new Shader;
	if (GL_FALSE == baseShader_->addShader(Shader::VERTEX, vertex_base_shader, false)) return false;
	if (GL_FALSE == baseShader_->addShader(Shader::FRAGMENT, fr_shader, false)) return false;
	baseShader_->linkProgram();
	baseShader_->checkProgram();
	CHECK_GL_ERROR;
	return true;
}

bool TessScene::initSceneModels(const SceneInitInfo&)
{
	int level = 8;

	int num_vertice = (level + 1) * (level + 1);
	int num_indice = 2 * (level + 1) * level + level;

	num_vertice *= 32;
	num_indice *= 32;

	std::vector<V3f>Vs(num_vertice,V3f(0.0f, 0.0f, 0.0f));
	std::vector<V3f>Ns(num_vertice, V3f(0.0f, 0.0f, 0.0f));
	std::vector<V2f>st;
	std::vector<uint32>indices(num_indice, 0);
	createPolyTeapot(level, Vs, Ns, st, indices);

	quad_indiecs_count_ = indices.size();

	glGenVertexArrays(1, &quad_vao_);
	glBindVertexArray(quad_vao_);

	GLuint v_buff;
	glGenBuffers(1, &v_buff);
	glBindBuffer(GL_ARRAY_BUFFER, v_buff);
	glBufferData(GL_ARRAY_BUFFER, Vs.size() * sizeof(V3f), &Vs[0], GL_STATIC_DRAW);

	GLuint normals;
	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, Ns.size() * sizeof(V3f), &Ns[0], GL_STATIC_DRAW);

	GLuint e_buff;
	glGenBuffers(1, &e_buff);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e_buff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, v_buff);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3f), 0);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V3f), 0);

	glBindVertexArray(0);


	//const GLsizei Vec3Stride = 3 * sizeof(float);
	//const GLsizei IndexStride = sizeof(unsigned short);

	//vertCount_ = sizeof(TeapotPositions) / Vec3Stride;
	//knotCount_ = sizeof(TeapotKnots) / IndexStride;

	//// Create the VAO:
	//glGenVertexArrays(1, &tess_vao_);
	//glBindVertexArray(tess_vao_);

	//// Create the VBO for positions:
	//{
	//	GLsizei totalSize = Vec3Stride * vertCount_;
	//	glGenBuffers(1, &positions);
	//	glBindBuffer(GL_ARRAY_BUFFER, positions);
	//	glBufferData(GL_ARRAY_BUFFER, totalSize, TeapotPositions, GL_STATIC_DRAW);
	//}

	//{
	//	GLsizei totalSize = Vec3Stride * vertCount_;
	//	glGenBuffers(1, &normals);
	//	glBindBuffer(GL_ARRAY_BUFFER, normals);
	//	glBufferData(GL_ARRAY_BUFFER, totalSize, TeapotNormals, GL_STATIC_DRAW);
	//}

	//// Create the knots VBO:
	//{
	//	GLsizei totalSize = IndexStride * knotCount_;
	//	glGenBuffers(1, &knots);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, knots);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalSize, TeapotKnots, GL_STATIC_DRAW);
	//}

	//// Create the VAO
	//glBindBuffer(GL_ARRAY_BUFFER, positions);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vec3Stride, 0);
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, normals);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vec3Stride, 0);


	
	CHECK_GL_ERROR;
	return true;
}


bool TessScene::initThisDemo(const SceneInitInfo&)
{
	if (type_ == TESS_SHADER)
	{
		GLint MaxPatchVertices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		printf("Max supported patch vertices %d\n", MaxPatchVertices);
		glPatchParameteri(GL_PATCH_VERTICES, 16);
	}
	
	return true;
}


void TessScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);

	if (key == GLU_KEY_R)
	{
	}
}


void TessScene::render(PassInfo& ps)
{
	if (type_ == TESS_SHADER)
	{
		tess_shader_->turnOn();
		initUniformVal(tess_shader_);
		glBindVertexArray(tess_vao_);
		glDrawArrays(GL_PATCHES, 0, 16);
		tess_shader_->turnOff();
	}
	else if(type_ == CPU)
	{
		baseShader_->turnOn();
		initUniformVal(baseShader_);
		glBindVertexArray(quad_vao_);
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(RESTART_INDEX);
		glDrawElements(GL_TRIANGLE_STRIP, quad_indiecs_count_, GL_UNSIGNED_INT,nullptr);
		baseShader_->turnOff();
		glDisable(GL_PRIMITIVE_RESTART);
		CHECK_GL_ERROR;
	}
}

int main()
{

	TessScene* scene = new TessScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	
	/*dc.glVersion_.glMaxJor_ = 4;
	dc.glVersion_.glMinJor_ = 3;*/

	wc.title_ = "Tess quad with bezier";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	glDisable(GL_CULL_FACE);

	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}


