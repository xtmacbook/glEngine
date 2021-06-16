
#ifndef __OpenGL__gbuffer__
#define __OpenGL__gbuffer__
#include "BaseObject.h"
#include "smartPointer.h"
#include "decl.h"
#include <vector>

class Texture;
class LIBENIGHT_EXPORT TextureBuffer :public base::BaseObject
{
public:

	TextureBuffer();

	~TextureBuffer();
	unsigned int 		getTexture()const;

	unsigned int		attachment_;
	unsigned int	    textarget_;

	unsigned int		glObj();
	void		bind();
	void		createGLObj();
	void		initGLObj();
	void		destoryGLObj();

	base::SmartPointer<Texture>	 texture_;
};


struct LIBENIGHT_EXPORT RenderBuffer :public base::BaseObject
{
	~RenderBuffer();

	void	createGLObj();
	void	destoryGLObj();
	void	bindGLObj();

	unsigned int w_;
	unsigned int h_;
	unsigned int target_;
	unsigned int internalformat_;
	unsigned int attachment_;
	unsigned int renderbuffertarget_;

	unsigned int		obj_ = 0;
};

enum GBUFFER_C_TYPE
{
	GB_NONE				= 0x0000,
	GB_POSITION			= 0x0001,
	GB_NORMAL			= 0x0002,
	GB_SPECULAR			= 0x0004,
	GB_DEPTH			= 0X0010,
	GB_DEFUAULT			= GB_POSITION | GB_NORMAL | GB_SPECULAR | GB_DEPTH
};

class LIBENIGHT_EXPORT GBuffer :public base::BaseObject
{
public:

	GBuffer(unsigned int w,unsigned int h, GBUFFER_C_TYPE type = GB_DEFUAULT);

	virtual ~GBuffer();

	bool initBuffer();
	void createGLObj();
	bool initGLObj();
	void bindGLObj();
	void unBindGLObj();

	void copyDepthToDefaultBuffer();
	void destoryBuffer();
	void drawToBuffer();


	unsigned int  obj_;
	unsigned int w_;
	unsigned int h_;

	base::SmartPointer<TextureBuffer>  position_;
	base::SmartPointer<TextureBuffer>  normal_;
	base::SmartPointer<TextureBuffer> specular_;
	base::SmartPointer<RenderBuffer> depth_;

	std::vector<unsigned int> attachments_;

	GBUFFER_C_TYPE type_;
};

#endif
