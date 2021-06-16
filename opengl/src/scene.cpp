//
//  Model.cpp
//  OpenGL
//
//  Created by xt on 15/8/10.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "Scene.h"
#include "resource.h"
#include "log.h"
#include "shader.h"
#include "inputmanager.h"
#include "renderNode.h"
#include "axes.h"
#include "gls.h"
#include "glu.h"
#include "cameraBase.h"
#include <random>


Scene::Scene():BaseScene()
{
}


Scene::~Scene()
{
	destroy();
}


void Scene::preRender(PassInfo&ps)
{
	//axes
	axisShder_->turnOn();
	initUniformVal(axisShder_);
	axesNodes_->render(axisShder_, ps);
	axisShder_->turnOff();
}

bool frameInfo(int code)
{
	printf("frame info\n");
	return true;
}


bool Scene::initKeyCallback()
{
	//71 glfw_key_H GLFW_KEY_SPACE 32
	KEY_CALLBACK(71,	base::NewPermanentCallback(frameInfo));
	KEY_M_CALLBACK(32,	base::NewPermanentCallback(this,  &Scene::home));
	KEY_M_CALLBACK(263, base::NewPermanentCallback(this, &Scene::stepRotate, 263)); //left
	KEY_M_CALLBACK(262, base::NewPermanentCallback(this, &Scene::stepRotate, 262)); //right 
	KEY_M_CALLBACK(82,	base::NewPermanentCallback(this,  &Scene::refreshShader, 0)); //R 

	return true;
}

bool Scene::initialize(const SceneInitInfo&info)
{
	scene_box_.init();
	CHECK_GL_ERROR;

	if (!initKeyCallback())
	{
		Log::instance()->printMessage("init key callback error !\n");
		return false;
	}
	CHECK_GL_ERROR;
	if (!initOpengl(info))
	{
		Log::instance()->printMessage("init key callback error !\n");
		return false;
	}
	CHECK_GL_ERROR;
	if (!initLight(info))
	{
		Log::instance()->printMessage("init scene light error !\n");
		return false;
	}
	CHECK_GL_ERROR;
   if (!initShader(info))
    {
        Log::instance()->printMessage("init shader error !\n");
        return false;
    }
	CHECK_GL_ERROR;
    if (!initTexture(info))
	{
		Log::instance()->printMessage("init texture error !\n");
		return false;
	}
	CHECK_GL_ERROR;
	if (!initSceneModels(info))
	{
		Log::instance()->printMessage("init scene models error !\n");
		return false;
	}
	CHECK_GL_ERROR;
	if (!initThisDemo(info))
	{
		Log::instance()->printMessage("init this  Demo error !\n");
		return false;
	}

	CHECK_GL_ERROR;
	axesNodes_ = createAxesRenderNode();
	axisShder_ = createAxesRenderNodeShader();

	home();

	return true;
}

bool Scene::update()
{
	for each (IRenderNode_SP node in renderNodes_)
	{
		node->update(camera_);
	}
	return true;
}

bool Scene::initOpengl(const SceneInitInfo&)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return true;
}

bool Scene::initTexture(const SceneInitInfo&)
{
	/*Texture * curTexObj = new Texture(get_texture_BasePath().c_str());
	curTexObj->bind();
	curTexObj->context(NULL);
	curTexObj->filterLinear();
	curTexObj->mirrorRepeat();
	curTexObj->unBind();
	texturesObj_.push_back(curTexObj);*/
	return true;
}


bool Scene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 vcolor;"

		"out vec3 ocolor;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"	ocolor = vcolor;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec3 ocolor;"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(ocolor, 1.0f);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	shaders_.push_back(shader);

	CHECK_GL_ERROR;

	return true;
}

bool Scene::initSceneModels(const SceneInitInfo&)
{
	return true;
}

void Scene::render(PassInfo&ps)
{
	if (shaders_.empty()) return;

	Shader * shader = shaders_[0];
	shader->turnOn();
	initUniformVal(shader);

	for each (IRenderNode_SP r in renderNodes_)
	{
		r->render(shader, ps);
	}
	shader->turnOff();

	CHECK_GL_ERROR;
}

void Scene::home()
{
	if (scene_box_.valid())
	{
		V3f center = scene_box_.center();
		float distance = scene_box_.radius() * 1.2;
		camera_->positionCamera(center[0], center[1], center[2] + distance, center[0], center[1], center[2],0.0f,1.0f,0.0f);
	}
}

void Scene::stepRotate(int key)
{
	if (scene_box_.valid())
	{
		static float deltaDegree = 10.0;
		static float curDegree = 0.0;
		V3f center = scene_box_.center();
		float distance = scene_box_.radius() * 1.2;

		if (key == 263) //GLFW_KEY_LEFT
		{
			curDegree -= deltaDegree;
		}
		else if (key == 262)
		{
			curDegree += deltaDegree;
		}
		else {}
		if (curDegree > 360.0) curDegree = 360.0;
		if (curDegree < 0.0)   curDegree = 0.0;
		
		float cv = cos(curDegree * PI / 180.0);
		float sv = sin(curDegree * PI / 180.0);

		camera_->positionCamera(center[0] - (cv * distance) ,
								center[1], 
								center[2] - (sv * distance), 
								center[0], center[1], center[2], 0.0f, 1.0f, 0.0f);

		/*camera_->positionCamera(center[0], center[1], center[2], center[0] + (cv * distance),
			center[1],
			center[2] + (sv * distance),0.0f,1.0f,0.0f);
*/

	}

}

void Scene::refreshShader(int num)
{

}

// This handles all the cleanup for our model, like the VBO/VAO buffers and shaders.
void Scene::destroy()
{
}

void Scene::expandSceneByBox(const base::BoundingBox&box)
{
	scene_box_.expandBy(box);
}

bool Scene::initThisDemo(const SceneInitInfo&)
{
	return true;
}


bool Scene::initLight(const SceneInitInfo&)
{
	return true;
}


void Scene::clearRenderNode()
{
	renderNodes_.clear();
}

void Scene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_U)
		{
			GLint mode[2];
			glGetIntegerv(GL_POLYGON_MODE, mode);

			if (mode[0] == GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else if (mode[0] == GL_LINE)
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

	}
	CHECK_GL_ERROR;
}

void Scene::mouse_move(const V2f& pt, const float& z_scale)
{

}

void Scene::mouse_down(int button, const V2f& pt, int state)
{
	
}

void Scene::mouse_up(int button, const V2f& pt, int state)
{

}

void Scene::mouse_scroll(double xoffset, double yoffset)
{

}

void Scene::addRenderNode(IRenderNode_SP g)
{
	renderNodes_.push_back(g);
	expandSceneByBox(g->getBox());
}

IRenderNode_SP Scene::getRenderNode(unsigned int indice)
{
	assert(indice <= (renderNodes_.size() - 1));

	return renderNodes_[indice];
}


unsigned int Scene::renderNodeNum() const
{
	return renderNodes_.size();
}

void Scene::createSceneData(void)
{

}

