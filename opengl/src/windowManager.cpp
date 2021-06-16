//    //
//  WindowManager.cpp    //  GLApplication.cpp
//  OpenGL    //  OpenGL
//    //
//  Created by xt on 15/8/10.    //  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.    //  Copyright (c) 2015年 xt. All rights reserved.
//    //


#include "windowManager.h"
#include "inputmanager.h"
#include "camera.h"
#include "sys.h"
#include "log.h"
#include "text.h"
#include "ad.h"
#include "gls.h"
#include "gli.h"
#include "window_w32.h"

extern void  mouse_button_callback(GWindow* window, int button, int action, int mods);
extern void  mouse_scroll_callback(GWindow* window, double xoffse, double yoffse);
extern void  mouse_curse_pos_callback(GWindow* window, double xpos, double ypos);
extern void  windowSize(GWindow* window, int width, int height);
extern void  key_callback(GWindow* window, int, int, int, int);

extern GWindow * g_main_window;
WindowManager::WindowManager() :inputManager_(nullptr),
window_(nullptr)
{
}

WindowManager::~WindowManager()
{
	destroy();
}

bool WindowManager::initialize(const DeviceConfig* dc, const WindowConfig* wc)
{
	if (!gluInitWindow()) return false;

	window_ = gluCreateWindow(dc, wc);

	g_main_window = window_;

	if (!window_) return false;

	gluMakeCurrentContext(window_);

	gluSwapInterval(1);

	if (!GL_X::initGlew()) return false;

	GL_X::testOpenGl();

	if (dc->glContext_.debug_context_)
		if (GL_X::isSupportExtension("GL_ARB_debug_output"))
			GL_X::init_opengl_dbg_win(dc->glContext_.debug_sync_);

	inputManager_ = InputManager::instance();
	inputManager_->setWindow(window_);

	return true;
}

void WindowManager::loop()
{

}

void WindowManager::destroy()
{
	if (inputManager_)
	{
		delete inputManager_;
		inputManager_ = NULL;
	}
}

GWindow* WindowManager::getWindow()
{
	return window_;
}

bool WindowManager::shouldeClose()const
{
	if (window_) return window_->shouldClose_;
	return true;
}

bool WindowManager::processInput(bool continueGame = true)
{
	return continueGame;
}

InputManager* WindowManager::getInputManager()
{
	return  inputManager_;
}

void WindowManager::swapTheBuffers()
{
	if (window_) gluSwapBuffer(window_);
}

Version_Config::Version_Config()
{
	glMaxJor_ = 3;
	glMinJor_ = 3;
	glforward_compat_ = GL_TRUE;
	glProfile_ = OPENGL_CORE_PROFILE;
}
