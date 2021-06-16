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
#include <baseMesh.h>
#include <Inputmanager.h>
#include <callback.h>
#include <text.h>
#include "gls.h"
#include "dynamicMesh.h"
#include "IGeometry.h"
#include <glinter.h>
#include <glQuery.h>

/**
 * test opengl Draw Api:
	glDrawElements								 (GLenum mode, GLsizei count, GLenum type, const void*indices)
	glDrawElementsInstance						 (GLenum mode, GLsizei count, GLenum type, const void*indices, GLsizei instancecount)
	glDrawElementsBaseVertex					 (GLenum mode, GLsizei count, GLenum type, const void*indices,							GLint basevert)
	glDrawElementsIndirect						 (GLenum mode,				  GLenum type, void* indirect)
	glDrawElementsInstancedBaseInstance			 (GLenum mode, GLsizei count, GLenum type, const void*indices,  GLsizei instancecount,					GLuint baseinstance)
	glDrawElementsInstancedBaseVertex			 (GLenum mode, GLsizei count, GLenum type, void *indices,       GLsizei instancecount, GLint basevertex)
	glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type​, void *indices​,       GLsizei instancecount​, GLint basevertex​, GLuint baseinstance)

	glMultiDrawElements	

	glMultiDrawElements:
	void glMultiDrawElements(mode,const GLsizei *count,GLenum type,const Glvoid*const*indices,Glsizei pricount)
	{
		GLsizei i;
		for(i = 0;i < primcount;i++)
		{
			glDrawElements(mode,count[i],type,indices[i]);
		}
	}

	glMultDrawElementsBaseVertex:
	void glMultDrawElementsBaseVertex(GLenum mode,const GLsizei*count,GLenum type,const GLvoid*const*indices,GLsizei primcount,const GLint * baseVertex)
	{
		GLsize i;
		for(i = 0;i <primcout;i++)
		{
			glDrawElementsBaseVertex(mode,count[i],type,indices[i],baseVertex[i]);
		}
	}

 */

#define INSTANCE_COUNT 200

class DrawScene;

DrawScene* g_scene;

static GLfloat quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,0.0,
	-1.0f, -1.0f, 0.0, 0.0f, 0.0f,0.0,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,0.0,

	-1.0f,  1.0f, 0.0, 0.0f, 1.0f,0.0,
	1.0f, -1.0f, 0.0, 1.0f, 0.0f,0.0,
	1.0f,  1.0f, 0.0, 1.0f, 1.0f,0.0
};


static const GLfloat vertex_positions[] =
{
	-1.0f, -1.0f,  0.0f,
	1.0f, -1.0f,  0.0f, 
	-1.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f,
};

// Color for each vertex
static const GLfloat vertex_colors[] =
{
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f
};

static const GLushort vertex_indices[] =
{
	0, 1, 2
};

static const GLfloat cube_positions[] =
{
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f, -1.0f, -1.0f, 
	1.0f, -1.0f,  1.0f, 
	1.0f,  1.0f, -1.0f, 
	1.0f,  1.0f,  1.0f
};

// Color for each vertex
static const GLfloat cube_colors[] =
{
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f
};

static const GLushort cube_indices[] =
{
	0, 1, 2, 3, 6, 7, 4, 5,         // First strip
	0xFFFF,                         // <<-- This is the restart index
	2, 6, 0, 4, 1, 5, 3, 7          // Second strip
};


const std::vector<GLfloat> vertices
{
	-0.63f,  0.00f, 0.0f, 0.0f, 0.5f, 0.5f,
	-0.63f,  0.50f, 0.0f, 0.0f, 0.5f, 0.5f,
	0.63f,  0.50f, 0.0f,  0.0f, 0.5f, 0.5f,
	0.63f,  0.00f, 0.0f,  0.0f, 0.5f, 0.5f,
						  
	-0.63f, -0.50f, 0.0f, 0.0f, 0.5f, 0.5f,
	-0.63f,  0.00f, 0.0f, 0.0f, 0.5f, 0.5f,
	0.63f,  0.00f, 0.0f,  0.0f, 0.5f, 0.5f,
	0.63f, -0.50f, 0.0f	, 0.0f, 0.5f, 0.5f
};

const std::vector<GLuint> indices
{
	0, 1, 2,
	2, 3, 0
};

class DrawScene :public Scene
{
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
public:
};


class  TestMeshGeoemtry :public CommonGeometry
{
public:
	TestMeshGeoemtry(bool update = false);
	~TestMeshGeoemtry();

	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

private:
	void initG0();
	void drawG0(const DrawInfo&di);

	//restart primivte
	void initG1();
	void drawG1(const DrawInfo&di);

	//glDrawElementsBaseVertex
	void initG2();
	void drawG2(const DrawInfo&di);

	//glDrawElementsInstanced
	void initG3();
	void drawG3(const DrawInfo&di);

	//glDrawElementsIndirect
	void initG4();
	void drawG4(const DrawInfo&di);
private:
	unsigned int vbo_;
	unsigned int ebo_;

	unsigned int vbo1_;

};

TestMeshGeoemtry::TestMeshGeoemtry(bool update /*= false*/)
{
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);
	glGenBuffers(1, &vbo1_);

}

TestMeshGeoemtry::~TestMeshGeoemtry()
{
	glDeleteVertexArrays(1, &ebo_);
}

void TestMeshGeoemtry::updateGeometry()
{
}

void TestMeshGeoemtry::initG0()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TestMeshGeoemtry::drawG0(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
	Matrixf transform = di.matrix_* getModelMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);
	transform = transform * m;
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 0);
	transform = transform * m;
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

}

void TestMeshGeoemtry::initG1()
{
	//restart primitive
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_positions) + sizeof(cube_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_positions), cube_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_positions), sizeof(cube_colors), cube_colors);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(cube_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void TestMeshGeoemtry::drawG1(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
	Matrixf transform = di.matrix_* getModelMatrix();

	////restart primivte
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	transform = transform * m;
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);
	glDisable(GL_PRIMITIVE_RESTART);
}

void TestMeshGeoemtry::initG2()
{
	////base vertex
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TestMeshGeoemtry::drawG2(const DrawInfo&di)
{
	////base vertex
	GLint  location = di.draw_shader_->getVariable("model");
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
	Matrixf transform = di.matrix_* getModelMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	transform = transform * m;
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 0);
	transform = transform * m;
	di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 4);
}

void TestMeshGeoemtry::initG3()
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 vcolor;"

		//Instanced vertex attributes
		"layout(location = 2) in vec4 instance_weights;\n"
		
		"out vec3 ocolor;"
		"uniform mat4 model[4];"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"   vec4 weights = normalize(instance_weights);\n"
		"   mat4 m = mat4(0.0);\n"
		"    for (int n = 0; n < 4; n++)\n"
		"    {\n"
		"        m += (model[n] * weights[n]);\n"
		"    }\n"
		"	gl_Position = projection * view * m * vec4(position, 1.0f);"
		"	ocolor = vcolor;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec3 ocolor;"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(ocolor, 1.0f);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	g_scene->setShader(0,shader);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	V4f colors[INSTANCE_COUNT];
	for (int n = 0; n < INSTANCE_COUNT; n++)
	{
		float a = float(n) / 4.0f;
		float b = float(n) / 5.0f;
		float c = float(n) / 6.0f;

		colors[n][0] = 0.5f * (sinf(a + 1.0f) + 1.0f);
		colors[n][1] = 0.5f * (sinf(b + 2.0f) + 1.0f);
		colors[n][2] = 0.5f * (sinf(c + 3.0f) + 1.0f);
		colors[n][3] = 1.0f;
	}

	glGenBuffers(1, &vbo1_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
	glVertexAttribDivisor(2, 1);
	// It's otherwise the same as any other vertex attribute - set the pointer and enable it
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}

void TestMeshGeoemtry::drawG3(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");

	V4f weights[INSTANCE_COUNT];
	float t = GLQuery::getCurrentTime() / 100000000;
	for (int n = 0; n < INSTANCE_COUNT; n++)
	{
		float a = float(n) / 4.0f;
		float b = float(n) / 5.0f;
		float c = float(n) / 6.0f;

		weights[n][0] = 0.5f * (sinf(t * 6.28318531f * 8.0f + a) + 1.0f);
		weights[n][1] = 0.5f * (sinf(t * 6.28318531f * 26.0f + b) + 1.0f);
		weights[n][2] = 0.5f * (sinf(t * 6.28318531f * 21.0f + c) + 1.0f);
		weights[n][3] = 0.5f * (sinf(t * 6.28318531f * 13.0f + a + b) + 1.0f);
	}

	Matrixf  model_matrix[4];
	for (int n = 0; n < 4; n++)
	{
		Matrixf tt;
		tt = math::scaleR(tt, V3f(5.0f, 5.0f, 5.0f) );
		tt = math::rotateR(tt,t * 360.0f * 40.0f + float(n + 1) * 29.0f, V3f(0.0f, 1.0f, 0.0f));
		tt = math::rotateR(tt,t * 360.0f * 20.0f + float(n + 1) * 35.0f, V3f(0.0f, 0.0f, 1.0f));
		tt = math::rotateR(tt,t * 360.0f * 30.0f + float(n + 1) * 67.0f, V3f(0.0f, 1.0f, 0.0f));
		tt = math::translateR(tt,V3f((float)n * 10.0f - 15.0f, 0.0f, 0.0f));
		model_matrix[n] = tt;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo1_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(weights), weights, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	di.draw_shader_->setMatrix4(location, 4, GL_FALSE, math::value_ptr(model_matrix[0]));
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, INSTANCE_COUNT);
}

void TestMeshGeoemtry::initG4()
{

}

void TestMeshGeoemtry::drawG4(const DrawInfo&di)
{

}

void TestMeshGeoemtry::initGeometry()
{
	bindVAO();
	//initG0();
	//initG1();
	initG3();
	glBindVertexArray(0);
	CHECK_GL_ERROR;
}

void TestMeshGeoemtry::drawGeoemtry(const DrawInfo&di)
{

	if (di.needBind_) glBindVertexArray(vao_);
	//drawG0(di);
	//drawG1(di);
	drawG3(di);
	glBindVertexArray(0);
}

void TestMeshGeoemtry::computeBoundingBox(void *)
{

}


bool DrawScene::initSceneModels(const SceneInitInfo&)
{

	RenderNode_SP quadNode(new RenderNode);
	CommonGeometry_Sp qg = new TestMeshGeoemtry;
	qg->initGeometry();
	quadNode->setGeometry(qg);
	addRenderNode(quadNode);
	return true;
}

int main()
{
	DrawScene * scene = new DrawScene;
	g_scene = scene;

	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();

	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Draw Api Test";
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
