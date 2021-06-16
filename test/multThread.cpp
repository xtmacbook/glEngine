

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
#include "render.h"
#include <glinter.h>

extern GLApplication * app;

class TestScene :public Scene
{
public:
	TestScene();
protected:
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					update();
	//virtual void					render(PassInfo&);

public:
	bool							brute_ = false;

	Render*							render_;
};

TestScene::TestScene()
{
}

bool TestScene::initSceneModels(const SceneInitInfo&)
{
	base::SmartPointer<DynamicMeshGeoemtry> mg = new DynamicMeshGeoemtry(4, 1248 * 1248, 1248 * 1248);
	mg->RFVF() |= FVF_XYZ;
	mg->RFVF() |= FVF_NORMAL;
	mg->initGeometry();

	Mesh_SP mesh = new Mesh;
	mesh->rmode() = GL_TRIANGLE_FAN;
	mesh->RFVF() |= FVF_XYZ;
	mesh->RFVF() |= FVF_NORMAL;
	mg->addMesh(mesh);

	RenderNode_SP rn = new RenderNode;
	if (rn) rn->setGeometry(mg);
	addRenderNode(rn);

	return true;
}

bool TestScene::initThisDemo(const SceneInitInfo&)
{
	render_ = new Render;
	
	return true;
}

bool TestScene::update()
{
	CommonGeometry_Sp geo = ((RenderNode*)getRenderNode(0).addr())->getGeometry();
	DynamicMeshGeoemtry* dGeo = (DynamicMeshGeoemtry*)(geo.addr());
	Mesh_SP mesh = dGeo->meshs_[0];
	mesh->clear();

	Vertex v;
	v.Position = V3f(0.0, 0.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(0.0, 100.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(100.0, 100.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(100.0, 0.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);


	v.Position = V3f(200.0, 0.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(200.0, 100.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(300.0, 100.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);

	v.Position = V3f(300.0, 0.0, 0.0);
	v.Normal = V3f(1.0, 0.0, 0.0);
	mesh->addVertex(v);


	Scene::update();

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

	app = &application;

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
