//
//  BaseRender.cpp
//  
//
//  Created by xt on 16/1/31.
//
//

#include "baseScene.h"
#include "shader.h"
#include "camera.h"
#include "helpF.h"
#include "texture.h"
#include "gls.h"

BaseScene::BaseScene()
{
	uboBlock_ = 0;
}

void BaseScene::initUniformVal(Shader*shader)
{
	GLint location = shader->getVariable("projection");
	if (location != -1)
		shader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera_->getProjectionMatrix()));

	location = shader->getVariable("view");
	if (location != -1)
		shader->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera_->getViewMatrix()));

	//
	//   location = shader->GetVariable("lightPos");
	//   if(location != -1 && !_lights.empty())
	//       shader->SetFloat3(location,_lights[0].getPosition().x,_lights[0].getPosition().y,_lights[0].getPosition().z);
	//   
	//   location = shader->GetVariable("viewPos");
	//   if(location != -1)
	//       shader->SetFloat3(location,camera->GetPosition().x,camera->GetPosition().y,camera->GetPosition().z);
}

CameraBase* BaseScene::getCamera()const
{
	return camera_;
}

void  BaseScene::setMasterCamera(CameraBase*pCamera)
{
	camera_ = pCamera;
}


void BaseScene::addLight(Light_SP light)
{
	if (light->type_ == LIGHT_DIRECTION)
	{
		dlights_.push_back(light);
	}
	else if (light->type_ == LIGHT_POINT)
	{
		plights_.push_back(light);
	}
	else if (light->type_ == LIGHT_SPOT)
	{
		slights_.push_back(light);
	}
	else
	{

	}
}


void BaseScene::setShader(int i, Shader*shd)
{
	if (i < shaders_.size())
	{
		Shader* t = shaders_[i];
		delete t;

		shaders_[i] = shd;
	}
}

void BaseScene::preRenderShaderData()
{
	Matrixf projectionMatrix = camera_->getProjectionMatrix();

	Matrixf viewMatrix = camera_->getViewMatrix();

	glBindBuffer(GL_UNIFORM_BUFFER, uboBlock_);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrixf), &projectionMatrix[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrixf), sizeof(Matrixf), &viewMatrix[0][0]);
	//glBindBuffer(GL_UNIFORM_BUFFER,0);
}

BaseScene::~BaseScene()
{
	for (int i = shaders_.size() - 1; i >= 0; i--)
	{
		Shader * s = shaders_[i];
		delete s;
		s = NULL;
	}

	for (int i = texturesObj_.size() - 1; i >= 0; i--)
	{
		Texture * to = texturesObj_[i];
		delete to;
		to = NULL;
	}

	for (int i = dlights_.size() - 1; i >= 0; i--)
	{
		BaseLight * light = dlights_[i];
		delete light;
		light = NULL;
	}
	for (int i = slights_.size() - 1; i >= 0; i--)
	{
		BaseLight * light = slights_[i];
		delete light;
		light = NULL;
	}
	for (int i = plights_.size() - 1; i >= 0; i--)
	{
		BaseLight * light = plights_[i];
		delete light;
		light = NULL;
	}
}

PassInfo::PassInfo()
{
	tranform_  = Matrixf(1.0);
}
