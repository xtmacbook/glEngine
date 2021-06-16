//
//  Inputmanager.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__Inputmanager__
#define __OpenGL__Inputmanager__


#include "decl.h"
#include "type.h"
#include "callback.h"
#include <vector>
#include <map>

class CameraBase;
struct GWindow;
class Scene;
class LIBENIGHT_EXPORT InputManager
{
public:
	static InputManager *   instance();

	void                    keyPressed(int key, int st,int action, int mods);

	void					button(int button, int action, int mods);

	void					scroll(double xoffse, double yoffse);

	void					move(double xpos, double ypos);

	int						getInputKey(int);
	void                    setWindow(int w, int h);
    static void             setWindow(GWindow *);

	static void             setCamera(CameraBase *pCamera);
	static CameraBase*      getCamera() { return camera_; }

	void					addScene(Scene*);
	typedef					base::ResultCallback1<bool, int>* keyPressCallback;
	static  void            addKeyCallback(int, keyPressCallback);

	typedef					base::Closure* ScenekeyMPressCallback;
	static void			    addMethodCallback(int, ScenekeyMPressCallback);
private:
	InputManager();
	static CameraBase          *								camera_;
	static InputManager    *									inputManager_;
	static GWindow      *										window_;
	static std::map<int, keyPressCallback>						keyCallbacks_;
	static std::map<int, ScenekeyMPressCallback>				methodCallBacks_;

	typedef std::map<int, keyPressCallback>::iterator				CallbackIter;
	typedef std::map<int, ScenekeyMPressCallback>::iterator			MCallbackIter;

	std::vector<Scene*>												scenes_;
};

#define    KEY_CALLBACK(inputCode,callback) InputManager::addKeyCallback(inputCode,callback);
#define    KEY_M_CALLBACK(inputCode,callback) InputManager::addMethodCallback(inputCode,callback);

#endif /* defined(__OpenGL__Inputmanager__) */
