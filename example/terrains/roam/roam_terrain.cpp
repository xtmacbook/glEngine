
#include <gls.h>
#include "landscape.h"

#include <scene.h>
#include <RenderNode.h>
#include <shader.h>
#include <engineLoad.h>
#include <resource.h>
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <log.h>
#include <helpF.h>
#include <Inputmanager.h>
#include <callback.h>
#include <text.h>
#include <dynamicMesh.h>
#include <glinter.h>

#include "Utility.h"

class RoamScene :public Scene
{
public:
	virtual ~RoamScene();
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					update();

public:

	virtual void					render(PassInfo&);

	Landscape*						landscape_ = nullptr;
	bool							brute_ = false;
};

RoamScene::~RoamScene()
{
	if (landscape_)
	{
		delete landscape_;
		landscape_ = nullptr;
	}
}

bool RoamScene::initSceneModels(const SceneInitInfo&)
{
	std::string heightFile = get_media_BasePath() + "heightmap/Height1024.raw";

	loadTerrain(MAP_SIZE, heightFile.c_str(), &gHeightMap);

	if (roamInit(gHeightMap) != 0) return false;

	landscape_ = new Landscape;
	landscape_->Init(gHeightMap);

	return true;
}

bool RoamScene::initShader(const SceneInitInfo&)
{
	return true;
}


bool RoamScene::update()
{
	landscape_->Reset(getCamera());
	landscape_->Tessellate(getCamera());
	return true;
}


void RoamScene::render(PassInfo&)
{
	landscape_->Render(getCamera());
}

int main()
{
	RoamScene * scene = new RoamScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "3d terrain Test";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->positionCamera(128, 5.3, 128, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	pCamera->setClipPlane(0.1f, 5000.0f);
	application.start();

	return 0;
}

