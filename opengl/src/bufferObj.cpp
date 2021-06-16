//
//  bufferObj.cpp
//  openGLTest
//
//  Created by xt on 18/5/16.
//  Copyright © 2018年 xt. All rights reserved.
//

#include <stdlib.h>

#include "bufferObj.h"
#include "shader.h"
#include "gls.h"


BufferObj::BufferObj(Shader * s)
{
	shader_ = s;
	size_ = offset_ = type_ = NULL;
	indices_ = NULL;
}

void BufferObj::setUniformName(const char** names, const char* uniformName, int uniformNum)
{
	ubbIndex_ = shader_->getUniformBlock(uniformName, uboDataSize_);

	indices_ = new unsigned int[uniformNum];
	size_ = new int[uniformNum];
	offset_ = new int[uniformNum];
	type_ = new int[uniformNum];

	shader_->getUniformsiv(ubbIndex_, names, indices_, size_, offset_, type_, uniformNum);

	if (ubbIndex_ != -1)
	{
		GLvoid * buffer = malloc(uboDataSize_);
		if (buffer != NULL)
		{

			//            memcpy(buffer + offset[Scale], &scale,
			//                   size[Scale] * TypeSize(type[Scale]));

			GLuint ubo;
			glGenBuffers(1, &ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferData(GL_UNIFORM_BUFFER, uboDataSize_,
				buffer, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

		}
	}


}


BufferObj::~BufferObj()
{
    
}
