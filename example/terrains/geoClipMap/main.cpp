
#include <scene.h>
#include <RenderNode.h>
#include <shader.h>
#include <gLApplication.h>
#include <fspCamera.h>
#include <windowManager.h>
#include <log.h>
#include <Inputmanager.h>
#include <glinter.h>
#include <resource.h>
#include <camera.h>
#include "geoClipMapping.h"


/*

github DigitalRune
*/

class ClipMappingScene :public Scene
{
public:
	ClipMappingScene();
	virtual ~ClipMappingScene();
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);

	virtual bool					update();
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);

	void							initState();


public:
	virtual void					render(PassInfo&);

	ClipmapTerrain*					terrain_ = nullptr;
};

ClipMappingScene::ClipMappingScene()
{
	terrain_ = new ClipmapTerrain(255, 7);
}

ClipMappingScene::~ClipMappingScene()
{
	if (terrain_)
	{
		delete terrain_;
		terrain_ = nullptr;
	}
}

bool ClipMappingScene::initSceneModels(const SceneInitInfo&)
{
	int vertexUniformBlock, uniformBlockSize;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &vertexUniformBlock);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uniformBlockSize);

	return terrain_->init();
}

bool ClipMappingScene::initShader(const SceneInitInfo&)
{
	return true;
}

bool ClipMappingScene::update()
{
	terrain_->update(getCamera());
	return true;
}

void ClipMappingScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);
}

void ClipMappingScene::initState()
{

}

void ClipMappingScene::render(PassInfo&)
{
	terrain_->render(getCamera());
}

int main()
{
	ClipMappingScene * scene = new ClipMappingScene;
	FSPCamera *pCamera = new FSPCamera();
	pCamera->positionCamera(V3f(0.0, 50.0, 0.0), 0.0, 0.0, V3f(0.0, 1.0, 0.0));

	pCamera->setClipPlane(1.0f, 5000.0f);
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.shouldCreateSharedContext_ = true;
	wc.title_ = "Geometry ClipMappings terrain Test";
	wc.width_ = 1024;
	wc.height_ = 906;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);

	application.initScene();

	application.start();
}
