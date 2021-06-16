//
//  GLApplication.h
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__GLApplication__
#define __OpenGL__GLApplication__

#include "decl.h"
#include <stdio.h>

class  Camera;
class  Scene;
class  Audo;
class  WindowManager;
struct WindowConfig;
struct DeviceConfig;

class LIBENIGHT_EXPORT GLApplication
{
public:

	GLApplication(bool isFullScreen = false);

	GLApplication(unsigned int w, unsigned int h);

	GLApplication(Scene* scene, bool isFullScreen = false);

	GLApplication(Scene* scene, unsigned int w, unsigned int h);

	~GLApplication() { destroy(); }

	WindowManager* getWindowManager()const;
	void            setWindowManager(WindowManager* pWindowManager);

	virtual void	mouse_move(const int x, const int y);
	virtual void	key(const int key, const bool down);
	void			activated(const bool state);

	bool            initialize(const WindowConfig* ,const DeviceConfig* dc);
	void            start();

	void            pressEvents();
	void            update();
	void			clear();

	void            render();
	void            destroy();
	
	void            initScene();
	void			showFrameRate(bool);

	unsigned int    width()const { return width_; }
	unsigned int	heigh()const { return hight_; }
private:
	void            openglInit(void);

protected:

	WindowManager* windowManager_;
	Scene*			scene_;
	Audo*			audo_;

	unsigned int    width_;
	unsigned int    hight_;
	bool            isFullScreen_;
	bool            useImgui_;
	bool			frameRateToConsole_;

};


#endif /* defined(__OpenGL__GLApplication__) */
