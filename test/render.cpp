#include "render.h"
#include "glu.h"
#include "glinter.h"
#include "gLApplication.h"
#include "windowManager.h"
#include "window_w32.h"

GLApplication * app;

Render::Render()
{
	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Test Falsh buffer";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	HWND  hwnd = app->getWindowManager()->getWindow()->win_.handle_;
	HGLRC shared = app->getWindowManager()->getWindow()->context_.wgl_ct_.handle_;
	Context * context = gluCreateContext(&hwnd,&shared,&dc,NULL);
}

Render::~Render()
{

}

void Render::render()
{

}

void Render::run()
{

}
