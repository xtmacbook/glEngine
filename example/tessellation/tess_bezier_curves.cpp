

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
#include "alg.h"

using namespace math;
/************************************************************************/
/* https://www.scratchapixel.com/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot
*/
/************************************************************************/

const bool _SUPPORT_TESS_ = false;

V4f g_p_x, g_p_y, g_p_z;

class TessScene :public Scene
{
public:
	enum TessType
	{
		TESS_SHADER,
		GEOM_SHADER,
		CPU
	};
	virtual ~TessScene();
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					render(PassInfo&);
private:

	void							generate_tess_line(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, int level,std::vector<V3f>&data,int flag);

	V3f								bezierBasisMatrix(float t);

	//								https://www.scratchapixel.com/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot/fast-forward-differencing
	void							taylorSeries(const V3f& P0, const V3f& P1, const V3f& P2, const V3f& P3, int level, std::vector<V3f>&data);
private:
	Shader_SP						tess_shader = nullptr;
	Shader_SP						geo_shader = nullptr;
	Shader_SP						base_shader = nullptr;

	GLuint							vao_[2];
	GLuint							vbo_[2];

	int								cpu_verter_num = 0;
	TessType						type_ = CPU;
};


TessScene::~TessScene()
{
	if (vbo_[0] != 0) glDeleteBuffers(2, vbo_);
	if (vao_[0] != 0) glDeleteVertexArrays(2, vao_);
}

bool TessScene::initShader(const SceneInitInfo&)
{
	const char vertex_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"

		"layout (location = 0) in vec3 position;\n"
		"out vec3 vposition;"
		"void main()\n"
		"{"
			"vposition = position;\n"
		"}"

	};
	const char vertex_normal_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"
		"uniform mat4 view;"
		"uniform mat4 projection; "
		"layout (location = 0) in vec3 position;\n"
		"out vec3 vposition;"
		"void main()\n"
		"{"
			"gl_Position = projection * view * vec4(position,1.0);"
		"}"

	};
	const char tess_ct_shader[] =
	{
		"#version 410 core\n"
		"precision mediump float;\n"

		"layout(vertices = 4) out;\n"
		"uniform int uOuter0,uOuter1;"
		"in vec3 vposition[];"
		"void main()\n"
		"{"
			"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; "
			"gl_TessLevelInner[0] = uOuter0;"
			"gl_TessLevelInner[1] = uOuter1;"
		"}"

	};
	const char tess_et_shader[] =
	{
		"#version 410 core\n"
		"precision mediump float;\n"

		"layout(isolines,equal_spacing) in;\n"
		"uniform mat4 projection;"
		"uniform mat4 view;"
		"vec3 bezier(float u,vec4 p0,vec4 p1,vec4 p2,vec4 p3)"
		"{"
			"float b0 = (1.0 - u ) * (1.0 - u) * (1.0 - u);"
			"float b1 = 3.0 * u * ( 1.0 - u ) * ( 1.0 - u);"
			"float b2 =	3.0 * u * u * (1.0 - u);"
			"float b3 = u * u * u;"
			"return b0 * p0 + b1 * p1 + b2 * p2 + b3 * p3;"
		"}"

		"void main()\n"
		"{"
			"float u = gl_TessCoord.x;"
			"gl_Position = projection * view * bezier(u,gl_in[0].gl_Position,gl_in[1].gl_Position,gl_in[2].gl_Position,gl_in[3].gl_Position);"
		"}"

	};
	const char tess_fr_shader[] =
	{
		"#version 330 core\n"
		"precision mediump float;\n"
		"out vec4 o_vFragColor;\n"
		"uniform int idx;"
		"void main()\n"
		"{"
			"if(idx == 0) o_vFragColor = vec4(1.0,0.0,0.0,1.0);\n"
			"else if(idx == 1) o_vFragColor = vec4(0.0,1.0,0.0,1.0);\n"
			"else   o_vFragColor = vec4(0.0,0.0,1.0,1.0);\n"
		"}"

	};

	if (_SUPPORT_TESS_)
	{
		tess_shader = new Shader;
		if (GL_FALSE == tess_shader->addShader(Shader::VERTEX, vertex_shader, false)) return false;
		if (GL_FALSE == tess_shader->addShader(Shader::TESS_TCL, tess_ct_shader, false)) return false;
		if (GL_FALSE == tess_shader->addShader(Shader::TESS_TES, tess_et_shader, false)) return false;
		if (GL_FALSE == tess_shader->addShader(Shader::FRAGMENT, tess_fr_shader, false)) return false;
		tess_shader->linkProgram();
		tess_shader->checkProgram();
		CHECK_GL_ERROR;
		tess_shader->turnOn();
		GLint loc = tess_shader->getVariable("uOuter0");
		if (loc != -1)
			tess_shader->setFloat(loc, 5);
		loc = tess_shader->getVariable("uOuter1");
		if (loc != -1)
			tess_shader->setFloat(loc, 6);
		tess_shader->turnOff();
	}
	
	//GEOM SHADER
	geo_shader = new Shader;
	if (GL_FALSE == geo_shader->addShader(Shader::VERTEX, vertex_shader, false)) return false;
	if (GL_FALSE == geo_shader->addShader(Shader::GEOMETRY, "tessellation/tess_curvers.geo")) return false;
	if (GL_FALSE == geo_shader->addShader(Shader::FRAGMENT, tess_fr_shader, false)) return false;
	geo_shader->linkProgram();
	geo_shader->checkProgram();
	CHECK_GL_ERROR;
	geo_shader->turnOn();
	GLint mloc = geo_shader->getVariable("u_four_bezier_basis_matrix");
	if (mloc != -1)
	{
		geo_shader->setMatrix4(mloc, 1, GL_TRUE, bezier_basis_matrix);
	}
	geo_shader->turnOff();

	//BASE SHADER 
	base_shader = new Shader;
	//line shader
	if (GL_FALSE == base_shader->addShader(Shader::VERTEX, vertex_normal_shader, false)) return false;
	if (GL_FALSE == base_shader->addShader(Shader::FRAGMENT, tess_fr_shader, false)) return false;
	base_shader->linkProgram();
	base_shader->checkProgram();
	CHECK_GL_ERROR;

	return true;
}

bool TessScene::initSceneModels(const SceneInitInfo&)
{
	GLfloat fourPoints[] = {
		// positions  
		0.0f,  0.0f,  0.0f, 
		1.0f, 1.0f,  1.0f, 
		2.0f, 1.0f,  0.0f,
		3.0f,  0.0f,  1.0f,
	};

	{
		Matrixf mat(
			bezier_basis_matrix[0], bezier_basis_matrix[1], bezier_basis_matrix[2], bezier_basis_matrix[3],
			bezier_basis_matrix[4], bezier_basis_matrix[5], bezier_basis_matrix[6], bezier_basis_matrix[7],
			bezier_basis_matrix[8], bezier_basis_matrix[9], bezier_basis_matrix[10], bezier_basis_matrix[11],
			bezier_basis_matrix[12], bezier_basis_matrix[13], bezier_basis_matrix[14], bezier_basis_matrix[15]
		);

		V4f x = V4f(fourPoints[0], fourPoints[3], fourPoints[6], fourPoints[9]);
		V4f y = V4f(fourPoints[1], fourPoints[4], fourPoints[7], fourPoints[10]);
		V4f z = V4f(fourPoints[2], fourPoints[5], fourPoints[8], fourPoints[11]);

		g_p_x = mat * x;
		g_p_y = mat * y;
		g_p_z = mat * z;
	}

	glGenVertexArrays(2, vao_);
	glGenBuffers(2, vbo_);

	glBindVertexArray(vao_[0]);//tess or geo shader
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, fourPoints, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	std::vector<V3f> data;
	int level = 10;
	generate_tess_line(V3f(fourPoints[0], fourPoints[1], fourPoints[2]), 
		V3f(fourPoints[3], fourPoints[4], fourPoints[5]), 
		V3f(fourPoints[6], fourPoints[7], fourPoints[8]), 
		V3f(fourPoints[9], fourPoints[10], fourPoints[11]), level, data,2);

	cpu_verter_num = data.size();
	glBindVertexArray(vao_[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size() * 3, &data[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
	return true;
}

bool TessScene::initThisDemo(const SceneInitInfo&)
{
	if (_SUPPORT_TESS_)
	{
		GLint MaxPatchVertices = 0;
		glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
		printf("Max supported patch vertices %d\n", MaxPatchVertices);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
	}
	
	return true;
}

void TessScene::render(PassInfo& ps)
{
	if (type_ == TESS_SHADER)
	{
		tess_shader->turnOn();
		initUniformVal(tess_shader);
		glBindVertexArray(vao_[0]);
		glDrawArrays(GL_PATCHES, 0, 4);
	}
	else if (type_ == GEOM_SHADER)
	{
		geo_shader->turnOn();
		initUniformVal(geo_shader);
		glBindVertexArray(vao_[0]);
		//bezier
		geo_shader->setInt(geo_shader->getVariable("idx"), 0);
		geo_shader->setFloat(geo_shader->getVariable("beizer_line"), 1.0);
		glDrawArrays(GL_LINES_ADJACENCY, 0, 4);

		//decasteljau
		geo_shader->setInt(geo_shader->getVariable("idx"), 1);
		geo_shader->setFloat(geo_shader->getVariable("beizer_line"), 0.0); //decasteljau
		glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
	}
	else //cpu
	{
		base_shader->turnOn();
		initUniformVal(base_shader);
		base_shader->setInt(base_shader->getVariable("idx"), 0);
		glBindVertexArray(vao_[1]);
		glDrawArrays(GL_LINE_STRIP, 0, cpu_verter_num);
	}
	//base line
	base_shader->turnOn();
	initUniformVal(base_shader);
	base_shader->setInt(base_shader->getVariable("idx"), 2);
	glBindVertexArray(vao_[0]);
	glDrawArrays(GL_LINE_STRIP, 0, 4);
	base_shader->turnOff();

	CHECK_GL_ERROR;
}


void TessScene::generate_tess_line(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, int level, 
	std::vector<V3f>&data , int flag)
{
	for (int i = 0; i <= level; i++)
	{
		if (flag == 0)
		{
			data.push_back(evalBezierCurve(p0, p1, p2, p3, i * 1.0f / level));
			data.push_back(bezierBasisMatrix(i * 1.0f / level));
		}
		else if (flag == 1)
		{
			data.push_back(decasteljau(p0, p1, p2, p3, i * 1.0f / level));
		}
	}

	if (flag == 2)
	{
		taylorSeries(p0, p1, p2, p3, level, data);
	}
}
math::V3f TessScene::bezierBasisMatrix(float t)
{
	V4f tq(t * t * t, t * t, t, 1);
	return V3f(math::dotVec(g_p_x , tq), math::dotVec(g_p_y ,tq), math::dotVec(g_p_z ,tq));
}


void TessScene::taylorSeries(const V3f& P0, const V3f& P1, const V3f& P2, const V3f& P3,int level,std::vector<V3f>&data)
{
	V3f* B = new V3f[level];

	float h = 1.f / level;
	V3f b0 = P0;
	V3f fph = (P1 - P0) * h * 3.0f;
	V3f fpphh = (P0 * 6.0f - P1 * 12.0f + P2 * 6.0f) * h * h;
	V3f fppphhh = (P0 * -6.0f + P1 * 18.0f - P2 * 18.0f + P3 * 6.0f) * h * h * h;

	B[0] = b0;
	for (uint32_t i = 1; i <= level; ++i)
	{
		B[i] = B[i - 1] + fph + fpphh / 2.0f + fppphhh / 6.0f;
		// update bd, bdd
		fph = fph + fpphh + fppphhh / 2.0f;
		fpphh = fpphh + fppphhh;
	}

	for (int i = 0; i < level; i++)
		data.push_back(B[i]);

	//delete []B;
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
	if (_SUPPORT_TESS_)
	{
		dc.glVersion_.glMaxJor_ = 4;
		dc.glVersion_.glMinJor_ = 3;
	}
	
	wc.title_ = "Tess_bezier_curves";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}


