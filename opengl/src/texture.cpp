//
//  texture.cpp
//  OpenGL
//
//  Created by xt on 15/8/20.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "texture.h"
#include "log.h"
#include "image.h"
#include "mesh.h"
#include "engineLoad.h"
#include "gls.h"


static unsigned int calculate_stride(const GLenum  h, const GLenum type, unsigned int width,
	unsigned int typesize, unsigned int pad = 4)
{
	unsigned int channels = 0;

	switch (h)
	{
	case GL_RED:    channels = 1;
		break;
	case GL_RG:     channels = 2;
		break;
	case GL_BGR:
	case GL_RGB:    channels = 3;
		break;
	case GL_BGRA:
	case GL_RGBA:   channels = 4;
		break;
	}

	unsigned int stride = typesize * channels * width;
	stride = (stride + (pad - 1)) & ~(pad - 1);
	return stride;
}

static unsigned int calculate_face_size(unsigned int w, unsigned int h, const GLenum type, unsigned int typesize)
{
	unsigned int stride = calculate_stride(h, type, w, typesize, 4);
	return stride * h;
}

#define SUBIMAGE  for (unsigned int i = 0; i < miplevels(); i++){ \
						glTexSubImage2D(GL_TEXTURE_2D, i, 0, 0, w, h, externFormat(), type(), ptr); \
						ptr += h * calculate_stride(h, type(), width(), typeSize(), 1); \
						h >>= 1; \
						w >>= 1; \
						if (!h) \
							h = 1; \
						if (!w) \
							w = 1;\
						}


Texture::Texture(const char*imgFileName) :
	img_(NULL), texObject_(-1)
{
	initParams();
	fileName_ = (imgFileName);
}

Texture::Texture(const Image_SP img)
{
	img_ = img;
}

Texture::Texture()
{
	initParams();
	type_ = GL_UNSIGNED_BYTE;
	target_ = GL_TEXTURE_2D;
	format_ = GL_RGBA;
	internalformat_ = GL_RGB8;
}

Texture::~Texture()
{
	if (img_)
	{
		delete img_;
		img_ = nullptr;
	}
	destoryGLObj();
}

void Texture::createObj()
{
	glGenTextures(1, &texObject_);
}
void Texture::initGLObj()
{
	contextNULL();
	glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, eMinFilter);
	glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, eMagFilter);
	glTexParameteri(target(), GL_TEXTURE_WRAP_S, iWrapS);
	glTexParameteri(target(), GL_TEXTURE_WRAP_T, iWrapT);
	glTexParameteri(target(), GL_TEXTURE_WRAP_R, iWrapR);
}
void Texture::destoryGLObj()
{
	if (texObject_ != -1)
		glDeleteTextures(1, &texObject_);
}

bool Texture::loadData()
{
	img_ = IO::EngineLoad::loadImage(fileName_.c_str());
	if (img_ == NULL) return false;
	return true;
}

GLuint Texture::getTexture(void)const
{
	return texObject_;
}
void Texture::activeTexture(unsigned int num)
{
	glActiveTexture(GL_TEXTURE0 + num);
}

void  Texture::bind(void)
{
	glBindTexture(target(), texObject_);
}
void Texture::unBind(void)
{
	glBindTexture(target(), 0);
}

GLuint  Texture::width(void)const
{
	return (img_) ? img_->width() : width_;
}
GLuint  Texture::heigh(void)const
{
	return (img_) ? img_->height() : height_;
}
GLuint  Texture::depth(void)const
{
	return (img_) ? img_->depth() : depth_;
}

GLenum  Texture::externFormat(void)const
{
	return (img_) ? img_->format() : format_;
}
GLenum  Texture::interFormat(void)const
{
	return (img_) ? img_->internalformat() : internalformat_;
}
GLenum Texture::baseInterFormat(void) const
{
	return (img_) ? img_->baseInternalformat() : baseInternalformat_;
}
GLenum Texture::target(void) const
{
	GLenum t = (img_) ? img_->target() : target_;
	return (t == 0) ? target_ : t;
}
unsigned int Texture::miplevels(void) const
{
	return  numOfMiplevels_;
}
GLenum Texture::type()const
{
	return (img_) ? img_->type() : type_;
}
bool   Texture::mapMapping(void)const
{
	return  (img_) ? img_->numOfMiplevels() : numOfMiplevels_;
}
unsigned int Texture::faces(void) const
{
	return (img_) ? img_->faces() : faces_;
}
unsigned int Texture::typeSize(void) const
{
	return (img_) ? img_->typesize() : typesize_;
}
bool Texture::compressed(void) const
{
	return (img_) ? img_->isCompressed() : compressed_;
}
void Texture::initParams()
{
	img_ = NULL;
	fileName_ = "";
	texObject_ = -1;
	t_type_ = T_NONE;


	width_ = 0;
	height_ = 0;
	depth_ = 0;
	numOfMiplevels_ = 1;
	target_ = GL_TEXTURE_2D;
	internalformat_ = GL_RGB8;
	baseInternalformat_ = GL_RGB;
	format_ = GL_RGB;
	type_ = GL_UNSIGNED_BYTE;
	typesize_ = 0;
	faces_ = 0;

	compressed_ = false;
}


void Texture::context1D(void * data)
{
	if (data != NULL)
	{
		glTexStorage1D(GL_TEXTURE_1D, miplevels(), interFormat(), width());
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width(), externFormat(), interFormat(), data);
	}
	else
	{
		unsigned int w = width();
		glTexStorage1D(GL_TEXTURE_1D, img_->numOfMiplevels(), interFormat(), w);
		for (int32 l = 0; l < img_->numOfMiplevels(); l++)
		{
			glTexSubImage1D(GL_TEXTURE_1D, 0, 0, w, externFormat(), interFormat(), img_->getLevel(l));
			w >>= 1;
			w = w ? w : 1;
		}
		
	}
	
}
void Texture::context2D(void * data)
{
	if (data != NULL)
	{
		if (type() == GL_NONE)
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, interFormat(), width(), heigh(), 0, 420 * 380 / 2, data);
		else
		{
			glTexStorage2D(GL_TEXTURE_2D, miplevels(), interFormat(), width(), heigh());
			CHECK_GL_ERROR;
			{
				unsigned int h = heigh();
				unsigned int w = width();
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				if (type() == GL_UNSIGNED_BYTE)
				{
					const uint8 * ptr = (const uint8*)data;
					SUBIMAGE
				}
				else
				{
					const float * ptr = (const float*)data;
					SUBIMAGE
				}
			}
		}
	}
	else
	{
		if (type() == GL_NONE)
		{
			int32 w = width();
			int32 h = heigh();

			for (int32_t l = 0; l < img_->numOfMiplevels(); l++) {
				if (compressed()) {
					glCompressedTexImage2D(GL_TEXTURE_2D, l, interFormat(), w, h,
						0, img_->getImageSize(l), img_->getLevel(l));
				}
				w >>= 1;
				h >>= 1;
				w = w ? w : 1;
				h = h ? h : 1;
			}
		}
		else
		{
			{
				unsigned int h = heigh();
				unsigned int w = width();

				glTexStorage2D(GL_TEXTURE_2D, img_->numOfMiplevels(), interFormat(), w, h);
				for (int32 l = 0; l < img_->numOfMiplevels(); l++)
				{
					glTexSubImage2D(GL_TEXTURE_2D, l, 0, 0, w, h, externFormat(), type(), img_->getLevel(l));;
					w >>= 1;
					h >>= 1;
					w = w ? w : 1;
					h = h ? h : 1;
				}
			}
		}
	}

}
void Texture::context3D(void * data)
{
	glTexStorage3D(GL_TEXTURE_3D, miplevels(), interFormat(), width(), heigh(), depth());
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width(), heigh(), depth(), externFormat(), type(), data);
}
void Texture::context1DA(void * data)
{
	glTexStorage2D(GL_TEXTURE_1D_ARRAY, miplevels(), interFormat(), width(), heigh());
	glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, width(), heigh(), externFormat(), type(), data);
}
void Texture::context2DA(void ** data)
{

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, miplevels(), interFormat(), width(), heigh(), depth());

	for (int i = 0; i < depth(); i++)
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 
			0, //mipmap level
			0, 0, i, //x y z offset
			width(), heigh(), 1,//w h d 
			externFormat(), type(), ((math::uint8**)data)[i]);
	}
	CHECK_GL_ERROR;

}
void Texture::contextC(void * data)
{
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, miplevels(), interFormat(), width(), heigh());
	for (int32 f = GL_TEXTURE_CUBE_MAP_POSITIVE_X; f <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; f++)
	{
		int32 w = width();
		int32 h = heigh();
		for (int32 l = 0; l < miplevels(); l++)
		{
			if (compressed())
			{
				glCompressedTexImage2D(f, l, interFormat(), w, h, 0, img_->getImageSize(l), img_->getLevel(l, f));
				CHECK_GL_ERROR;
			}
			else
			{
				glTexSubImage2D(f, l, 0,0, w, h, 
					externFormat(), type(), img_->getLevel(l, f));
				CHECK_GL_ERROR;
			}
			w >>= 1;
			h >>= 1;
			w = w ? w : 1;
			h = h ? h : 1;
		}
	}

}

void Texture::contextCA(void * data)
{
	glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, miplevels(), interFormat(), width(), heigh(), depth());
	glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, width(), heigh(), depth(), externFormat(), type(), data);
}

bool Texture::context(void *data)
{
		switch (target())
		{
		case GL_TEXTURE_1D:
			context1D(data);
			CHECK_GL_ERROR;
			break;
		case GL_TEXTURE_2D:
			context2D(data);
			CHECK_GL_ERROR;
			break;
		case GL_TEXTURE_3D:
			context3D(data);
			CHECK_GL_ERROR;
			break;
		case GL_TEXTURE_1D_ARRAY:
			context1DA(data);
			CHECK_GL_ERROR;
			break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			contextCA(data);
			CHECK_GL_ERROR;
			break;
		case GL_TEXTURE_CUBE_MAP:
			contextC(data);
			CHECK_GL_ERROR;
			break;
		default:                                               // Should never happen
			return false;
		}

		if (miplevels() > 1)
		{
			if (img_ && (img_->numOfMiplevels() == 1)) glGenerateMipmap(target());
			if (!img_) glGenerateMipmap(target());
		}
		return true;
}

bool Texture::contextArr(void ** data)
{
	switch (target())
	{
	case GL_TEXTURE_2D_ARRAY:
		context2DA(data);
		CHECK_GL_ERROR;
	default:                                               // Should never happen
		return false;
	}

	if (miplevels() > 1)
		glGenerateMipmap(target());
	return true;
}

bool Texture::contextNULL()
{
	GLenum t = target();
	switch (t)
	{
	case GL_TEXTURE_1D:
		break;
	case GL_TEXTURE_2D:
		glTexStorage2D(GL_TEXTURE_2D, miplevels(), interFormat(), width(), heigh());
		break;
	case GL_TEXTURE_3D:
		break;
	case GL_TEXTURE_1D_ARRAY:
		break;
	case GL_TEXTURE_2D_ARRAY:
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, miplevels(), interFormat(), width(), heigh(), depth());
		break;
	case GL_TEXTURE_CUBE_MAP:
		break;
	case GL_TEXTURE_CUBE_MAP_ARRAY:
		break;
	default:
		break;// Should never happen
	}
	CHECK_GL_ERROR;
	return true;
}


void Texture::clampToEdge()
{
	glTexParameteri(target(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	if (heigh() != 0)
	{
		glTexParameteri(target(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (depth() != 0)
		{
			glTexParameteri(target(), GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
}
void Texture::mirrorRepeat()
{
	glTexParameteri(target(), GL_TEXTURE_WRAP_S, GL_REPEAT);
	if (heigh() != 0)
	{
		glTexParameteri(target(), GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (depth() != 0)
		{
			glTexParameteri(target(), GL_TEXTURE_WRAP_R, GL_REPEAT);
		}
	}
}
void Texture::filterLinear()
{
	if (miplevels())
	{
		glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		eMinFilter = GL_LINEAR_MIPMAP_LINEAR;
	}
	else
	{
		glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		eMinFilter = GL_LINEAR;
	}
	glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	eMagFilter = GL_LINEAR;
}
void Texture::filterNearest()
{
	if (miplevels())
	{
		glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		eMinFilter = GL_NEAREST_MIPMAP_NEAREST;
	}
	else
	{
		glTexParameteri(target(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		eMinFilter = GL_NEAREST;
	}
	glTexParameteri(target(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	eMagFilter = GL_NEAREST;
}
