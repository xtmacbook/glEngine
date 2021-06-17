//
//  texture.h
//  OpenGL
//
//  Created by xt on 15/8/20.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__texture__
#define __OpenGL__texture__

#include "BaseObject.h"
#include "smartPointer.h"
#include "image.h"

#include <string>
#include <map>

enum TEXTURE_TYPE
{
	T_DIFFUSE,
	T_SPECULAR,
	T_NORMAL,
	T_HEIGHT,
	T_NONE
};

/************************************************************************/
/* 
	# https://www.khronos.org/opengl/wiki/Texture_Storage
	levle: mipmaps
	array Texture :layer
	cube :face

	glTexStorage2D:
		For 1D array textures, the number of array layers in each mipmap level is the height​ value. 
		For rectangle textures, the number of mipmap levels must be 1.
	glTexStorage3D:
		For 2D array textures, the number of array layers in each mipmap level is the depth​ value.
		For 2D cubemap array textures, the number of cubemap layer-faces is the depth​, which must be 
			a multiple of 6. Therefore, the number of individual cubemaps in the array is given by depth​ / 6.
	
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, zoffset, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, texels);

	update from zoffset (layer index) and update count num is layerCount 
*/
/************************************************************************/

class LIBENIGHT_EXPORT Texture :public base::BaseObject
{
public:

	enum
	{
		TEXTURE_LOAD_AUTO = 0,
		TEXTURE_LOAD_L = 1,
		TEXTURE_LOAD_LA = 2,
		TEXTURE_LOAD_RGB = 3,
		TEXTURE_LOAD_RGBA = 4
	};

	Texture();
	Texture(const char*imgFileName);
	Texture(const Image_SP);
	~Texture();

	void				createObj();
	void				initGLObj();
	void				destoryGLObj();
	bool				loadData();
	bool                context(void * data);
	bool                contextArr(void ** data);
	bool				contextNULL();
	unsigned int        getTexture(void)const;

	void                mirrorRepeat();
	void                clampToEdge();

	void                filterLinear();
	void                filterNearest();

	void                bind(void);
	void                unBind(void);
	static void			activeTexture(unsigned int);
	unsigned int        type(void)const;
	unsigned int        width(void)const;
	unsigned int        heigh(void)const;
	unsigned int        depth(void)const;
	unsigned int        externFormat(void)const;
	unsigned int        interFormat(void)const;
	unsigned int		baseInterFormat(void)const;
	unsigned int		target(void)const;
	unsigned int		miplevels(void)const;
	bool                mapMapping(void)const;
	unsigned int		faces(void)const;
	unsigned int		typeSize(void)const;
	bool				compressed(void)const;

	inline void			setType(TEXTURE_TYPE t) { t_type_ = t; }

	void				initParams();

	std::string         fileName_;
	unsigned int        texObject_;
	TEXTURE_TYPE		t_type_;


	uint				width_;
	uint				height_;
	uint				depth_;

	uint				numOfMiplevels_;

	uint				target_;

	uint				internalformat_;
	uint				baseInternalformat_;
	uint				format_;

	uint				type_;
	uint				typesize_;

	uint				faces_;

	bool				compressed_;

	int iWrapS, iWrapT, iWrapR; //GL_TEXTURE_WRAP_R
	unsigned int eComparisonFunc, eMinFilter, eMagFilter; //GL_TEXTURE_MAG_FILTER

	base::SmartPointer<base::Image>		img_;

private:
	void                context1D(void * data);
	void                context2D(void * data);
	void                context3D(void * data);
	void                context1DA(void * data);
	void                context2DA(void ** data);
	void                contextC(void * data);
	void                contextCA(void * data);
	
};

typedef base::SmartPointer<Texture>  Texture_Sp;
#endif /* defined(__OpenGL__texture__) */
