//
//  WindowManager.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__GLFWManager__
#define __OpenGL__GLFWManager__

#include "BaseObject.h"
#include "common.h"
#include <stdio.h>
#include <string>

class  GWindow;
class  GLApplication;
class  InputManager;
class  Camera;
struct DeviceConfig;
struct WindowConfig;

class LIBENIGHT_EXPORT WindowManager : public base::BaseObject
{
public:

	WindowManager();

	~WindowManager();

	bool					 initialize(const DeviceConfig*, const WindowConfig* wc);
	void					 loop();
	void					 swapTheBuffers();
	bool					 processInput(bool continueGame);
	virtual void			 destroy();
	GWindow* getWindow();
	InputManager* getInputManager();
	bool					 shouldeClose()const;
private:
	InputManager* inputManager_;
protected:

	GWindow* window_;
	Camera* camera_;
};

#endif /* defined(__OpenGL__GLFWManager__) */
