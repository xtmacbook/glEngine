//
//  antialias.cpp
//  openGLTest
//
//  Created by xt on 18/6/9.
//  Copyright © 2018年 xt. All rights reserved.
//

#include <stdio.h>
#include "antialias.h"
#include "log.h"
#include "gls.h"

namespace render {

	Antialias::Antialias(TYPE t) :type_(t) {}

	bool Antialias::init()
	{
		if (type_ == FRAMEBUF)
		{
			width_ = 1024;
			hight_ = 986;
			createSupportMultisampleBuffer(width_, hight_, 4);
			createTextureForDivision(width_, hight_);
		}
		else if (type_ == TEXTURE)
		{
			width_ = 1024;
			hight_ = 986;
			createSupportMultisampleBuffer(width_, hight_, 4);
		}
		return true;
	}

	//from: https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
	bool Antialias::createSupportMultisampleBuffer(int width, int height, int samples)
	{

		glGenFramebuffers(1, &framebuffer_);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

		glGenTextures(1, &textureColorBufferMultiSampled_);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled_);
		//establishes the data storage,format dimensions and number
		//samples : the number of samples we want the texture to have
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		//attach texture to a framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled_, 0);


		//also create renderbuffer for depth and stencil attachments
		GLuint rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::printMessage("create framebuffer error in antialias!!!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool Antialias::createTextureForDivision(int width, int height)
	{

		// configure second post-processing framebuffer

		glGenFramebuffers(1, &intermediateFBO_);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO_);
		// create a color attachment texture

		glGenTextures(1, &screenTexture_);
		glBindTexture(GL_TEXTURE_2D, screenTexture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture_, 0);	// we only need a color buffer

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Log::printMessage("ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		return true;
	}
	void Antialias::bindMultSampleFrameBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	}
	void Antialias::blitFrameBuffer()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO_);
		glBlitFramebuffer(0, 0, width_, hight_, 0, 0, width_, hight_, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	}
	void Antialias::bindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, screenTexture_);
	}

	void Antialias::bindMultSampleTexture()
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled_);
	}
}



