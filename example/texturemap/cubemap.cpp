
//
#include <vector>
#include <string>
#define LIBENIGHT_EXPORT __declspec(dllimport)

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
#include "glinter.h"

unsigned int skyboxVAO;
unsigned int textureID;


class LScene :public Scene
{

protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initKeyCallback();


public:
	virtual void					render(PassInfo&);
};

bool LScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);
	Shader * currentShader = shaders_[0];

	std::string vs = get_shader_BasePath() + "skybox/skyboxcolor.vert";
	std::string fs = get_shader_BasePath() + "skybox/skyboxcolor.frag";
	if (!currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr))
	{
		CHECK_GL_ERROR;
		return false;
	}

	shaders_.push_back(new Shader);
	currentShader = shaders_[1];

	vs = get_shader_BasePath() + "texture/reflect.vert";
	fs = get_shader_BasePath() + "texture/reflect.frag";
	if (!currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr))
	{
		CHECK_GL_ERROR;
		return false;
	}

	return true;
}


bool LScene::initSceneModels(const SceneInitInfo&)
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	base::BoundingBox box(-1, -1, -1, 1, 1, 1);
	expandSceneByBox(box);

	RenderNode_SP sphereNode(new RenderNode);

	CommonGeometry_Sp  sphere =
		new ObjGeometry((get_model_BasePath() + "sphere.obj").c_str());
	sphere->initGeometry();
	sphereNode->setGeometry(sphere);
	addRenderNode(sphereNode);

	return true;
}

bool loadCubemap(std::vector<std::string>& faces)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	std::vector<base::SmartPointer<Image> > images;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string t = faces[i];
		base::SmartPointer<Image> img = IO::EngineLoad::loadImage(faces[i].c_str());
		if (!img)
		{
			return false;
		}
		images.push_back(img);
	}

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		base::SmartPointer<Image>& img = images[i];
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img->width(),img->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels());
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}


bool LScene::initTexture(const SceneInitInfo&)
{
	std::vector<std::string> imgs;
	imgs.push_back(get_texture_BasePath() + "skybox/right.jpg");
	imgs.push_back(get_texture_BasePath() + "skybox/left.jpg");
	imgs.push_back(get_texture_BasePath() + "skybox/top.jpg");
	imgs.push_back(get_texture_BasePath() + "skybox/bottom.jpg");
	imgs.push_back(get_texture_BasePath() + "skybox/front.jpg");
	imgs.push_back(get_texture_BasePath() + "skybox/back.jpg");

	return loadCubemap(imgs);
}


bool key(int code)
{
	return true;
}

bool LScene::initKeyCallback()
{
	bool result = Scene::initKeyCallback();

	KEY_CALLBACK(GLU_KEY_A, base::NewPermanentCallback(key));

	return result;
}

void LScene::render(PassInfo&ps)
{
	Shader * currentShader = shaders_[0];
	currentShader->turnOn();
	
	GLint location = currentShader->getVariable("projection");
	if (location != -1)
		currentShader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera_->getProjectionMatrix()));

	location = currentShader->getVariable("verseViewMat");
	if (location != -1)
		currentShader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(glm::mat4(math::inverse(glm::mat3(camera_->getViewMatrix())))));

	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	currentShader->turnOff();

	currentShader = shaders_[1];
	currentShader->turnOn();
	initUniformVal(currentShader);

	location = currentShader->getVariable("cameraPos");
	if (location != -1)
	{
		V3f pos = camera_->getPosition();
		currentShader->setFloat3(location,pos.x,pos.y,pos.z );
	}
	getRenderNode(0)->render(currentShader,ps);

	currentShader->turnOff();
}

int main()
{
	LScene* scene = new LScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "cubemap";
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


