//
//  GLApplication.cpp
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "gLApplication.h"
#include "timerManager.h"
#include "camera.h"
#include "scene.h"
#include "log.h"
#include "text.h"
#include "media/ad.h"
#include "Inputmanager.h"
#include "gls.h"
#include "glinter.h"
#include "windowManager.h"
#include "debug.h"

void GLApplication::openglInit(void)
{
	glClearColor(0.3f, 0.2f, 0.4f, 1.0f);
	
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
#if 0
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);
	//Depth testing is done in screen space after the fragment shader has run,gl_FragCoord represent the fragment's screen-space coordinate
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFFFFFFFF);
	glStencilFunc(GL_EQUAL, 0x00000000, 0x00000001);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.0);
	glClearStencil(0);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DITHER);
#endif
}

WindowManager* GLApplication::getWindowManager()const
{
	return  windowManager_;
}
void  GLApplication::setWindowManager(WindowManager* pWindowManager)
{
	windowManager_ = pWindowManager;
}

void GLApplication::mouse_move(const int x, const int y)
{

}

void GLApplication::key(const int key, const bool down)
{

}

void GLApplication::activated(const bool state)
{

}

bool GLApplication::initialize(const WindowConfig* wc, const DeviceConfig* dc)
{
	width_ = wc->width_;
	hight_ = wc->height_;

#ifdef FMOD_HAVE
	initAudo();
#endif
	// Make sure the window manager is initialized prior to calling this and creates the OpenGL context

	if (!windowManager_ || !windowManager_->initialize(dc, wc))
	{
		enDebug("GLApplication initialize error!\n");
		
		return false;
	}

	TimeManager::instance().initTime();

	glViewport(0, 0, width_, hight_);
	openglInit();

	KEY_M_CALLBACK(70, base::NewPermanentCallback(this, &GLApplication::showFrameRate, true)); //left

	return true;
}

void  GLApplication::pressEvents()
{
	windowManager_->processInput(true);
}

void  GLApplication::update()
{
	TimeManager::instance().calculateFrameRate(frameRateToConsole_);
#ifdef FMOD_HAVE

#endif
	updateAudo();

	if (scene_)
	{
		if (scene_->getCamera())
		{	
			scene_->getCamera()->update();
			scene_->getCamera()->calculateViewFrustum();
		}
		scene_->update();
	}
}

void GLApplication::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLApplication::render()
{
	int fr = TimeManager::instance().getFrameRate();
	std::string pss = std::to_string(fr);
	std::string msg = "FPS: " + pss;
	printScreen(msg.c_str(), 20, heigh() - 50, width_, hight_);

	if (scene_)
	{
		PassInfo info;
		scene_->preRender(info);
		scene_->render(info);
	}
}

void GLApplication::start()
{
	while (!windowManager_->shouldeClose())
	{
		clear();
		update();
		render();
		windowManager_->swapTheBuffers();
		gluPollEvents();
	}

	gluTerminate();
}

void GLApplication::initScene()
{
	if (scene_ == nullptr)
		scene_ = new Scene();

	if (scene_->getCamera() != NULL)
	{
		CameraBase* camera = scene_->getCamera();

		camera->setClipPlane(0.1f, 1000.0f);
		camera->setFov(45.0f);
		camera->setWindowSize(static_cast<float> (width_), static_cast<float>(hight_));
		if(!camera->initPos_)camera->positionCamera(0.0f, 0.0f, 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		windowManager_->getInputManager()->setCamera(camera);
		windowManager_->getInputManager()->addScene(scene_);

	}

	SceneInitInfo info;
	info.w_ = width_;
	info.h_ = hight_;

	if (!initText())
	{
		Log::instance()->printMessage("init type error!!");
		exit(-1);
	}

	if (!scene_->initialize(info))
	{
		exit(-1);
	}
}

void GLApplication::showFrameRate(bool show)
{
	frameRateToConsole_ = !frameRateToConsole_;
}

void GLApplication::destroy()
{
	if (windowManager_)
	{
		delete windowManager_;
		windowManager_ = nullptr;
	}
	delete scene_;
	scene_ = NULL;
	Log::instance()->destroy();
}

GLApplication::GLApplication(bool isFullScreen) :
	windowManager_(nullptr),
	isFullScreen_(isFullScreen),
	scene_(nullptr),
	width_(1200),
	hight_(900),
	useImgui_(false),
	frameRateToConsole_(false)
{
}

GLApplication::GLApplication(Scene* scene, bool isFullScreen /*= false*/) :
	windowManager_(nullptr),
	isFullScreen_(isFullScreen),
	scene_(scene),
	width_(1200),
	hight_(900),
	useImgui_(false),
	frameRateToConsole_(false)
{
}

GLApplication::GLApplication(unsigned int w, unsigned int h) :
	windowManager_(nullptr),
	isFullScreen_(false),
	scene_(nullptr),
	width_(w),
	hight_(h),
	useImgui_(false),
	frameRateToConsole_(false)
{

}

GLApplication::GLApplication(Scene* scene, unsigned int w, unsigned int h) :
	windowManager_(nullptr),
	isFullScreen_(false),
	scene_(scene),
	width_(w),
	hight_(h),
	useImgui_(false),
	frameRateToConsole_(false)
{

}

