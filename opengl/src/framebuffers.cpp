#include "framebuffers.h"
#include "gls.h"
#include "log.h"
#include "texture.h"



FrameBufferObject::FrameBufferObject(unsigned int target):target_(target)
{
	createGLObject();
}

FrameBufferObject::~FrameBufferObject()
{
	destoryGLObject();
}

void FrameBufferObject::colorTextureAttachments(std::vector<Texture *>color)
{
	int maxSize = getMaxColorAttachments();
	int size = color.size();
	size = (maxSize < size) ? maxSize : size;

	ncolorBuffer_ = size;

	colorTextures_ = color;

	type_ = COLOR_ONLY;

	for (int i = 0; i < size; i++)
	{
		glFramebufferTexture2D(target_, GL_COLOR_ATTACHMENT0 + i, colorTextures_[i]->target(), colorTextures_[i]->getTexture(), 0);
	}

	checkFramebufferStatus();
}

void FrameBufferObject::colorTextureAttachments(Texture * color)
{
	std::vector<Texture *> texturev;
	texturev.push_back(color);
	colorTextureAttachments(texturev);
}

void FrameBufferObject::depthTextureAttachments(Texture*depth, DepthType type)
{
	type_ = DEPTH_ONLY;

	depthTexture_ = depth;

	GLenum attachment;
	if (type == DEPTH) attachment = GL_DEPTH_ATTACHMENT;
	if (type == STENCIL) attachment = GL_STENCIL_ATTACHMENT;
	if (type == DEPTH_STENCIL) attachment = GL_DEPTH_STENCIL_ATTACHMENT;

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, depthTexture_->target(), depthTexture_->getTexture(), 0);

	checkFramebufferStatus();

}

void FrameBufferObject::colorAndDepthTextureAttachments(std::vector<Texture *>color, Texture*depth, DepthType type)
{

	type_ = COLOR_DEPTH;

	int maxSize = getMaxColorAttachments();
	int size = color.size();
	size = (maxSize < size) ? maxSize : size;

	ncolorBuffer_ = size;

	colorTextures_ = color;
	for (int i = 0; i < size; i++)
	{
		glFramebufferTexture2D(target_, GL_COLOR_ATTACHMENT0 + i, colorTextures_[i]->target(), colorTextures_[i]->getTexture(), 0);
	}

	depthTexture_ = depth;

	GLenum attachment;
	if (type == DEPTH) attachment = GL_DEPTH_ATTACHMENT;
	if (type == STENCIL) attachment = GL_STENCIL_ATTACHMENT;
	if (type == DEPTH_STENCIL) attachment = GL_DEPTH_STENCIL_ATTACHMENT;

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, depthTexture_->target(), depthTexture_->getTexture(), 0);

	if (type_ & COLOR_ONLY)
	{
		GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(ncolorBuffer_, DrawBuffers);
	}

	checkFramebufferStatus();

}

const int FrameBufferObject::getMaxColorAttachments()
{
	GLint maxAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
	return maxAttach;
}

const int FrameBufferObject::getMaxRenderBufferSize()
{
	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
	return maxSize;
}

void FrameBufferObject::bindObj(bool bind /*= true*/, bool flag)
{
	if (bind)
	{
		glBindFramebuffer(target_, fboObj_);
		if (!flag) return;

		if (type_ == DEPTH_ONLY)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		if (type_ == COLOR_ONLY)
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}

		if (type_ & COLOR_ONLY)
		{
			GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(ncolorBuffer_, DrawBuffers);
		}
	}
	else
	{
		glBindFramebuffer(target_, 0);
		if (!flag) return;

		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);

		if (type_ == COLOR_ONLY)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
	}
}

void FrameBufferObject::clearBuffer()
{
	GLbitfield bit = 0;

	if (type_ & COLOR_ONLY)
	{
		bit |= GL_COLOR_BUFFER_BIT;
	}
	if (type_ & DEPTH_ONLY)
	{
		bit |= GL_DEPTH_BUFFER_BIT;
		bit |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(bit);
}

void FrameBufferObject::createGLObject()
{
	if (!hasCreateObj_)
	{
		glGenFramebuffers(1, &fboObj_);
		hasCreateObj_ = !hasCreateObj_;
		CHECK_GL_ERROR;
	}
}

void FrameBufferObject::destoryGLObject()
{
	if (hasCreateObj_ && fboObj_)
	{
		bindObj(false,false);
		glDeleteFramebuffers(1, &fboObj_);
	}
}

bool FrameBufferObject::checkFramebufferStatus()
{
	GLenum status;
	bool ret = false;

	status = (GLenum)glCheckFramebufferStatus(target_);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		LOGI("Frame Buffer Complete");
		ret = true;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOGE("Unsupported framebuffer format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOGE("Framebuffer incomplete, missing attachment");
		break;
		//case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
		//    printf("Framebuffer incomplete, duplicate attachment");
		//   break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOGE("Framebuffer incomplete attachment");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		LOGE("Framebuffer incomplete, attached images must have same dimensions");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		LOGE("Framebuffer incomplete, attached images must have same format");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		LOGE("Framebuffer incomplete, missing draw buffer");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		LOGE("Framebuffer incomplete, missing read buffer");
		break;
	default:
		LOGE("Framebuffer incomplete, UNKNOW ERROR");
		// assert(0);
	}

	return ret;
}
