

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
#include "glinter.h"
#include "engineLoad.h"

typedef struct {
	int elements_count;
	float *array;
} fan_s;

GLuint g_vao;
GLuint g_vbo;
#define VBO_SIZE 4096
#define MAX_FANS 1024
GLint fans_starts[MAX_FANS];
GLsizei fans_sizes[MAX_FANS];

GLApplication* g_app;

float fan0_data[] = {
	0.0, 0.0, 0.0,1.0,0.0,0.0,
	0.0, 10.0, 0.0,1.0,0.0,0.0,
	10.0, 10.0, 0.0,1.0,0.0,0.0,
	10.0, 0.0, 0.0,1.0,0.0,0.0
};
float fan1_data[] = {
	20.0, 0.0, 0.0,1.0,0.0,0.0,
	20.0, 10.0, 0.0,1.0,0.0,0.0,
	15.0, 10.0, 0.0,1.0,0.0,0.0,
	15.0, 0.0, 0.0,1.0,0.0,0.0
};

class TestScene:public Scene
{
public:
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					update();
	virtual void					render(PassInfo&);
	virtual bool					initShader(const SceneInitInfo&);

	virtual bool					initTexture(const SceneInitInfo&);

public:
	bool							brute_ = false;
};

Texture * texture;

base::SmartPointer< FlashBuffer<uint8>  > texture_datas_;

bool TestScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec2 position;"
		"layout(location = 1) in vec2 v_tex;"

		"out vec2 tex_;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position,1.0, 1.0f);"
		"	tex_ = v_tex;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec2 tex_;"
		"uniform sampler2D diffTexture; "
		"out vec4 color;"
		"void main()"
		"{"
		"color = texture(diffTexture,tex_);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	shaders_.push_back(shader);

	shader->turnOn();
	shader->setInt(shader->getVariable("diffTexture"),0);

	return true;
}

 bool					TestScene::initTexture(const SceneInitInfo&)
{
	 texture = new Texture("D:/temp/terrain/0/0/0.png");
	 texture->target_ = GL_TEXTURE_2D;

	 if (texture->loadData())
	 {
		 texture->createObj();
		 texture->bind();
		 texture->mirrorRepeat();
		 texture->filterLinear();
		 if (!texture->context(NULL))
		 {
		 }
		 CHECK_GL_ERROR;
	 }

	 return true;
}

 base::SmartPointer<Quad> quad;
 
 base::SmartPointer<base::Image> imgs[4];

bool TestScene::initSceneModels(const SceneInitInfo&)
{
	quad = new Quad;
	quad->initGeometry();
	return true;
}

bool TestScene::initThisDemo(const SceneInitInfo&)
{
	int oneTextureSize = 128 * 128 * 3 ;
	texture_datas_ = new FlashBuffer<uint8>(oneTextureSize * 4);

	texture_datas_->setTarget(GL_PIXEL_UNPACK_BUFFER);
	texture_datas_->create_buffers(FlashBuffer<uint8>::ModeUnsynchronized, false);

	imgs[0] = IO::EngineLoad::loadImage("D:/temp/terrain/0/0/1.png");
	imgs[1] = IO::EngineLoad::loadImage("D:/temp/terrain/0/0/2.png");
	imgs[2] = IO::EngineLoad::loadImage("D:/temp/terrain/0/0/3.png");
	imgs[3] = IO::EngineLoad::loadImage("D:/temp/terrain/0/0/4.png");

	return true;
}

int index = 0;
bool TestScene::update()
{
	if (++index > 3) index = 0;

	texture_datas_->mapBuffer();
	uint8 * buffer = texture_datas_->getBufferPtr();
	memcpy(buffer, imgs[index]->pixels(), 128 * 128 * 3 );
	texture_datas_->flush_data();


	// bind the texture and PBO
	texture->bind();
	texture_datas_->bind();

	// copy pixels from PBO to texture object
	// Use offset instead of ponter.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, 0);
	texture->unBind();
	texture_datas_->unbind();
	return true;
}

void TestScene::render(PassInfo&)
{
	shaders_[0]->turnOn();
	initUniformVal(shaders_[0]);
	glActiveTexture(GL_TEXTURE0);
	texture->bind();
	

	DrawInfo info;
	info.draw_shader_ = shaders_[0];
	quad->drawGeoemtry(info);

	shaders_[0]->turnOff();
}

int main()
{
	TestScene * scene = new TestScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);
	g_app = &application;

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Test Falsh buffer";
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
