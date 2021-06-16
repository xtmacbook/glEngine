//
//  main.cpp
//  opengl3
//
//  Created by xt on 19/3/14.
//  Copyright ? 2019年 xt. All rights reserved.
//

//GLUT
//#include <glut/glut.h>

#include "windowManager.h"
#include "gLApplication.h"
#include "camera.h"
#include "scene.h"
#include "resource.h"
#include "log.h"
#include "json.h"
#include "water.h"
#include "glinter.h"
#include <fstream>

#define WATERFILEEXTENSION ".water"
#define XSTRIDEID "x_stride"
#define ZSTRIDEID "z_stride"
#define WAVESID "waves"
#define AMPID "amplitude"
#define STEEPID "steepness"
#define LENID "length"
#define SPEEDID "speed"
#define DIRID "direction"



class localScene :public Scene
{

public:
	localScene();
	~localScene();

	
	virtual void					render(PassInfo&);
	virtual bool					update();
    virtual void                   	imguiRender();
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);


private:

	Water*							water_;
};


localScene::localScene()
{
	water_ = new Water(100, 100);
    WaterEnditer::SetWater(water_);
}

localScene::~localScene()
{
	if (water_)
	{
		delete water_;
		water_ = nullptr;
	}
}

bool localScene::initShader(const SceneInitInfo&)
{
	if (water_)
	{
		water_->prepareShader();
		return true;
	}
	return false;
}


bool localScene::initSceneModels(const SceneInitInfo&)
{
	std::string wave_config = get_media_BasePath() + "../resource/water_wave.config";
	std::ifstream water_config_file(wave_config.c_str());
	if (!water_config_file.is_open())
	{
		Log::instance()->printMessage("water wave config file open error! \n");
		return false;
	}
	else
	{
		Json::Value json_water;
		water_config_file >> json_water;
		if (water_)
		{
			water_->setStride(json_water[XSTRIDEID].asUInt(), json_water[ZSTRIDEID].asUInt());
			water_->prepareGridData();
		}

		// Adding waves from configuration file
		const Json::Value & json_waves = json_water["waves"];
		const std::vector<std::string> & wave_names = json_waves.getMemberNames();

		for (const std::string & wave_name : wave_names)
		{
			const Json::Value & json_wave = json_waves[wave_name];
			float amplitude = json_wave[AMPID].asFloat();
			float steepness = json_wave[STEEPID].asFloat();
			float length = json_wave[LENID].asFloat();
			float speed = json_wave[SPEEDID].asFloat();
			V2f direction;
			direction.x = json_wave[DIRID][0].asFloat();
			direction.y = json_wave[DIRID][1].asFloat();
			direction = normalize(direction);
			water_->addWave(Wave(wave_name, amplitude, steepness,
				length, speed, direction));
		}

		water_config_file.close();
	}

	if (water_)
	{
		water_->initGeometry();
	}

	return true;
}


bool localScene::initTexture(const SceneInitInfo&)
{
	return true;
}

void localScene::render(PassInfo&info)
{
	if (water_)
		water_->render(camera_);

	Scene::render(info);
}

bool localScene::update()
{
	if (water_)
		water_->update();
	return true;
}

void localScene::imguiRender() {

    if(water_) WaterEnditer::DisplayEditor();
}


 
int main(void)
{

	localScene * scene = new localScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Wave";
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
