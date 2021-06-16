#include "gbuffer.h"
#include "log.h"
#include "texture.h"
#include "gls.h"

TextureBuffer::TextureBuffer()
{
	texture_ = new Texture;
}

TextureBuffer::~TextureBuffer()
{
	destoryGLObj();
}

GLuint TextureBuffer::getTexture() const
{
	if (texture_)  return texture_->getTexture();
	return 0;
}

GLuint TextureBuffer::glObj()
{
	return texture_->getTexture();
}

void TextureBuffer::bind()
{
	texture_->bind();
}

void TextureBuffer::createGLObj()
{
	texture_->createObj();
}

void TextureBuffer::initGLObj()
{
	texture_->initGLObj();
}

void TextureBuffer::destoryGLObj()
{
	texture_->destoryGLObj();
}

GBuffer::GBuffer(unsigned int w, unsigned int h, GBUFFER_C_TYPE t):obj_(0),
	w_(w),
	h_(h),
	type_(t),
	position_(NULL),
	normal_(NULL),
	specular_(NULL),
	depth_(NULL)
{

}

GBuffer::~GBuffer()
{
	destoryBuffer();
	unBindGLObj();
	glDeleteFramebuffers(1, &obj_);
}

bool GBuffer::initBuffer()
{
	if ((type_ & GB_POSITION) && position_ == NULL)
	{
		TextureBuffer* pt = new TextureBuffer;
		pt->texture_->width_ = w_;
		pt->texture_->height_ = h_;
		pt->texture_->internalformat_ = GL_RGB16F;
		pt->attachment_ = GL_COLOR_ATTACHMENT0;
		pt->textarget_ = GL_TEXTURE_2D;
		pt->texture_->format_ = GL_RGB;
		pt->texture_->type_ = GL_FLOAT;

		position_ = pt;
	}
	if ((type_ & GB_NORMAL) && normal_ == NULL)
	{
		TextureBuffer* nt = new TextureBuffer;
		nt->texture_->width_ = w_;
		nt->texture_->height_ = h_;
		nt->texture_->internalformat_ = GL_RGB16F;
		nt->attachment_ = GL_COLOR_ATTACHMENT1;
		nt->textarget_ = GL_TEXTURE_2D;
		nt->texture_->format_ = GL_RGB;
		nt->texture_->type_ = GL_FLOAT;

		normal_ = nt;
	}
	if ((type_ & GB_SPECULAR) && specular_ == NULL)
	{
		TextureBuffer* st = new TextureBuffer;
		st->texture_->width_ = w_;
		st->texture_->height_ = h_;
		st->texture_->internalformat_ = GL_RGBA;
		st->attachment_ = GL_COLOR_ATTACHMENT2;
		st->textarget_ = GL_TEXTURE_2D;
		st->texture_->format_ = GL_RGBA;
		st->texture_->type_ = GL_UNSIGNED_BYTE;

		specular_ = st;
	}
	if ((type_ & GB_DEPTH) && depth_ == NULL)
	{
		RenderBuffer * rb = new RenderBuffer;
		rb->w_ = w_;
		rb->h_ = h_;
		rb->internalformat_ = GL_DEPTH_COMPONENT;
		rb->target_ = GL_RENDERBUFFER;
		rb->attachment_ = GL_DEPTH_ATTACHMENT;
		rb->renderbuffertarget_ = GL_RENDERBUFFER;

		depth_ = rb;
	}
	return true;
}

void GBuffer::createGLObj()
{
	glGenFramebuffers(1, &obj_);
	
	if (position_)
	{
		position_->createGLObj();
		attachments_.push_back(position_->attachment_);
	}
	if (normal_)
	{
		normal_->createGLObj();
		attachments_.push_back(normal_->attachment_);
	}
	if (specular_)
	{
		specular_->createGLObj();
		attachments_.push_back(specular_->attachment_);
	}
	if (depth_)
		depth_->createGLObj();
	CHECK_GL_ERROR;
}

bool GBuffer::initGLObj()

{
	std::vector<GLenum> attachments_;
	glBindFramebuffer(GL_FRAMEBUFFER, obj_);

	TextureBuffer * buf = 0;
	if (position_)
	{
		buf = position_;
		buf->bind();
		buf->initGLObj();
		glFramebufferTexture2D(GL_FRAMEBUFFER, buf->attachment_, buf->textarget_, buf->glObj(), 0);
		attachments_.push_back(buf->attachment_);
	}

	if (normal_)
	{
		buf = normal_;
		buf->bind();
		buf->initGLObj();
		glFramebufferTexture2D(GL_FRAMEBUFFER, buf->attachment_, buf->textarget_, buf->glObj(), 0);
		attachments_.push_back(buf->attachment_);
	}

	if (specular_)
	{
		buf = specular_;
		buf->bind();
		buf->initGLObj();
		glFramebufferTexture2D(GL_FRAMEBUFFER, buf->attachment_, buf->textarget_, buf->glObj(), 0);
		attachments_.push_back(buf->attachment_);
	}

	if (depth_)
	{
		RenderBuffer * rb = depth_;
		rb->bindGLObj();
		glRenderbufferStorage(rb->target_,rb->internalformat_, rb->w_, rb->h_);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, rb->attachment_, rb->renderbuffertarget_, rb->obj_);
	}

	GLenum chrust = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	std::string errorMsg;
	switch (chrust)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		errorMsg = "specified framebuffer is the default read or draw framebuffer, but the default framebuffer does not exist!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		errorMsg = "the framebuffer attachment points are framebuffer incomplete!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		errorMsg = " framebuffer does not have at least one image attached to it!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		errorMsg = "  the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAW_BUFFERi!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		errorMsg = "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_UNSUPPORTED :
		errorMsg = " combination of internal formats of the attached images violates an implementation-dependent set of restrictions. !";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_INVALID_ENUM:
		errorMsg = "target is not GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER!";
		Log::printMessage(errorMsg.c_str());
		return false;
	case GL_FRAMEBUFFER_COMPLETE:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	default:
		return false;
	}

}

void GBuffer::bindGLObj()
{
	if (obj_ != 0)
		glBindFramebuffer(GL_FRAMEBUFFER, obj_);
}

void GBuffer::unBindGLObj()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::copyDepthToDefaultBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, obj_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBlitFramebuffer(0, 0, w_, h_, 0, 0, w_, h_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::destoryBuffer()
{
	if (position_) delete position_;
	if (normal_) delete normal_;
	if (specular_) delete specular_;
}


void GBuffer::drawToBuffer()
{
	glDrawBuffers(attachments_.size(), &attachments_[0]);
}

RenderBuffer::~RenderBuffer()
{
	destoryGLObj();
}

void RenderBuffer::createGLObj()
{
	glGenRenderbuffers(1, &obj_);
}

void RenderBuffer::destoryGLObj()
{
	if (obj_ != 0)
		glDeleteRenderbuffers(1, &obj_);
}

void RenderBuffer::bindGLObj()
{
	glBindRenderbuffer(GL_RENDERBUFFER, obj_);
}
