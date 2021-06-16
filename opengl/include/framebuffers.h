
#ifndef  _FRAME_BUFFER_
#define  _FRAME_BUFFER_

#include "decl.h"
#include "BaseObject.h"
#include "util.h"

#include <vector>

class Texture;

/*
	Attach :Attachment is different from binding. Objects are bound to the context; objects are attached to one another.

	Attachment point:  location within a framebuffer object hat a framebuffer-attachable image or layered image can be attached to.
*/

class LIBENIGHT_EXPORT FrameBufferObject  :public base::BaseObject
{

	enum Type
	{
		COLOR_ONLY = 0x0001,
		DEPTH_ONLY = 0x0002,
		COLOR_DEPTH = 0x0003
	};

	enum DepthType
	{
		DEPTH,
		STENCIL,
		DEPTH_STENCIL
	};

	ENGINE_FORBID_COPY(FrameBufferObject)

	FrameBufferObject(unsigned int target);

	virtual ~FrameBufferObject();

	void		colorTextureAttachments(Texture * color);
	void		colorTextureAttachments(std::vector<Texture *>color);
	void		depthTextureAttachments(Texture*depth, DepthType);
	void		colorAndDepthTextureAttachments(std::vector<Texture *>color, Texture*depth, DepthType);

	static const int	getMaxColorAttachments();

	static const int	getMaxRenderBufferSize()  ;

	void				bindObj(bool bind = true,bool flag = true);

	void			clearBuffer();
private:

	void				createGLObject();
	void				destoryGLObject();

	bool				checkFramebufferStatus();
	bool				hasCreateObj_ = false;

private:

	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, this->target_, this->colorTextures_[i], 0);

	std::vector<Texture*>		colorTextures_;
	Texture*					depthTexture_ = nullptr;

	unsigned int	fboObj_;

	unsigned int				ncolorBuffer_;

	unsigned int	width_;
	unsigned int	height_;
	unsigned int	target_;

	Type			type_ = COLOR_DEPTH;

};


#endif