//
//  Inputmanager.cpp
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//


#include "inputmanager.h"
#include "timerManager.h"
#include "scene.h"
#include "gls.h"
#include "window_w32.h"
#include "cameraBase.h"

void  mouse_button_callback(GWindow* window, int button, int action, int mods)
{
	InputManager::instance()->button(button, action, mods);
}

void mouse_curse_pos_callback(GWindow *window, double xpos, double ypos)
{
	if (window->virtualCursorPosX_ == xpos && window->virtualCursorPosY_ == ypos)
		return;

	window->virtualCursorPosX_ = xpos;
	window->virtualCursorPosY_ = ypos;

	InputManager::instance()->move(xpos, ypos);
}

void  mouse_scroll_callback(GWindow *window, double xoffse, double yoffse)
{
	InputManager::instance()->scroll(xoffse, yoffse);
}

//st GL_PRESS GL_RELEASE GL_REPEAT
void  key_callback(GWindow*window, int key, int st, int action, int mods)
{
	if (key == GLU_KEY_ESCAPE)
		window->shouldClose_ = true;

	InputManager::instance()->keyPressed(key, st, action, mods);
}


void windowSize(GWindow*window, int width, int height)
{
	InputManager::instance()->setWindow(width, height);
}

void InputManager::setWindow(GWindow * w)
{
	window_ = w;
}

void InputManager::setWindow(int w, int h)
{
	glViewport(0, 0, w, h);
	if(camera_) camera_->setWindowSize(static_cast<float>(w), static_cast<float>(h));
}


void InputManager::keyPressed(int key, int st, int action, int mods)
{
	if (camera_ == nullptr)
		return;

	camera_->processKeyboard(key, st, action, mods, TimeManager::instance().getDeltaTime());

	if (action == GLU_RELEASE)
	{
		CallbackIter findIter = keyCallbacks_.find(key);
		if (findIter != keyCallbacks_.end())
		{
			findIter->second->Run((int)key);
		}
		MCallbackIter fit = methodCallBacks_.find(key);
		if (fit != methodCallBacks_.end())
		{
			fit->second->Run();
		}
	}
		for (int i = 0; i < scenes_.size(); i++)
	{
		scenes_[i]->processKeyboard(key, st, action, mods, TimeManager::instance().getDeltaTime());
	}

}

void InputManager::button(int button, int action, int mods)
{
	if (camera_ == nullptr)
		return;

	double xpos, ypos;
	if (action == GLU_PRESS)
	{
		gluGetCursorPos(window_, &xpos, &ypos);
		camera_->mouse_down(button, V2f(xpos, ypos), mods);
		for (int i = 0; i < scenes_.size(); i++)
		{
			scenes_[i]->mouse_down(button, V2f(xpos, ypos), mods);
		}
	}
	else if (action == GLU_RELEASE)
	{
		gluGetCursorPos(window_, &xpos, &ypos);
		camera_->mouse_up(button, V2f(xpos, ypos), mods);
		for (int i = 0; i < scenes_.size(); i++)
		{
			scenes_[i]->mouse_up(button, V2f(xpos, ypos), mods);
		}
	}

}

void InputManager::scroll(double xoffse, double yoffse)
{
	if (camera_)
		camera_->mouse_scroll(xoffse, yoffse);
}

//left -top
void InputManager::move(double xpos, double ypos)
{
	if (camera_)
		camera_->mouse_move(V2f(xpos, ypos), 1.0f);
	for (int i = 0; i < scenes_.size(); i++)
	{
		scenes_[i]->mouse_move(V2f(xpos, ypos), 1.0f);
	}
}

int InputManager::getInputKey(int key)
{
	return gluGetKey(window_, key);
}

void InputManager::setCamera(CameraBase *pCamera)
{
	camera_ = pCamera;
}

void InputManager::addScene(Scene*s)
{
	scenes_.push_back(s);
}

InputManager* InputManager::inputManager_ = NULL;
CameraBase*  InputManager::camera_ = NULL;
GWindow* InputManager::window_ = NULL;

std::map<int, InputManager::keyPressCallback> InputManager::keyCallbacks_;
std::map<int, InputManager::ScenekeyMPressCallback> InputManager::methodCallBacks_;

InputManager* InputManager::instance()
{
	if (inputManager_ == NULL)
	{
		inputManager_ = new InputManager;
	}
	return inputManager_;
}

InputManager::InputManager()
{
}

void InputManager::addKeyCallback(int code, keyPressCallback cb)
{
	keyCallbacks_[code] = cb;
}

void InputManager::addMethodCallback(int code, ScenekeyMPressCallback cb)
{
	methodCallBacks_[code] = cb;
}

