
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
#include "geoMipmapping.h"
#include "../skydom.h"
#include "../cloud.h"
/**
 * 
	this demo is from Willem H.de Boer "Fast Terrain Rendering Using Geometrical MipMapping"
 */

/************************************************************************/
/*
		tess
		vertex_texture

*/
/************************************************************************/

GLApplication * g_app;
class GeoScene :public Scene
{
public:
	GeoScene();
	virtual ~GeoScene();
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);

	virtual bool					update();
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);

	void							initState();
public:
	virtual void					render(PassInfo&);
	GeoMipMapping_SP				terrain_ = nullptr;
	base::SmartPointer<SkyDom>		skyDom_ = nullptr;
	base::SmartPointer<Cloud>		cloud_ = nullptr;
};

GeoScene::GeoScene()
{
	terrain_ = new GeoMipMapping;
	skyDom_ = new SkyDom;
	cloud_ = new Cloud;
}

GeoScene::~GeoScene()
{

}

bool GeoScene::initSceneModels(const SceneInitInfo&)
{
	bool result =  terrain_->initHeightMap((get_media_BasePath() + "heightmap/ps_height_4k.png").c_str());
	terrain_->initConstant(getCamera(), g_app->heigh());
	if (!result) return false;
	result = terrain_->initHeightTexture(256, 256);
	if (!result) return false;
	result = terrain_->initBlock();
	if (!result) return false;
	result = terrain_->initGometry();

	skyDom_->init();
	cloud_->init();

	CHECK_GL_ERROR;
	return result;
}

bool GeoScene::initShader(const SceneInitInfo&)
{
	bool result =  terrain_->initShader();
	if (!result) return false;
	skyDom_->initShader();
	cloud_->initShader();
	return true;
}


bool GeoScene::initTexture(const SceneInitInfo&)
{
	 cloud_->initTexture();
	 return true;
}

bool GeoScene::update()
{
	if (terrain_) terrain_->updateGeometry(getCamera());
	return true;
}

void GeoScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);

	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_PAGE_UP || key == GLU_KEY_PAGE_DOWN)
		{
			terrain_->keyBoard(key);
		}
	}
}


void GeoScene::initState()
{
	//depth 
	glEnable(GL_DEPTH_TEST);
	//not update depth buffer,Even if the depth buffer exists and the depth mask is non-zero,
	//the depth buffer is not updated if the depth test is disabled.
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);

	//stencil
	glEnable(GL_STENCIL_TEST);
	//set front and back function and reference value for stencil testing
	glStencilFunc(GL_NEVER, GL_NEVER, 1);
	
	//blend
	//specify the equation used for both the RGB blend equation and the Alpha blend equation
	glEnable(GL_BLEND);

	glBlendEquation(GL_FUNC_ADD);//souce :new pixel
	glBlendFunc(GL_ONE, GL_ONE);

}

void GeoScene::render(PassInfo&)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);
	skyDom_->render(getCamera());

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);//souce :new pixel
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cloud_->render(getCamera());

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	terrain_->render(getCamera());
}

int main()
{
	GeoScene * scene = new GeoScene;
	FSPCamera *pCamera = new FSPCamera();
	pCamera->setClipPlane(1.0f, 5000.0f);
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	g_app = &application;
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "3d terrain Test";
	wc.width_ = 1024;
	wc.height_ = 906;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	
	glDisable(GL_CULL_FACE);

	application.initScene();
	pCamera->positionCamera(2048, 5.3, 2048, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//pCamera->positionCamera(.0,.0, 3.3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//pCamera->positionCamera(0.0, 0.0, 0.0,  1.0, 0.0, 0.0, 0.0,0.0, 1.0);
	application.start();

	return 0;
}
