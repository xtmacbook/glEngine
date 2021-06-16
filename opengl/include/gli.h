
#ifndef _GLI_H_
#define  _GLI_H_
#include "decl.h"
namespace GL_X
{
	LIBENIGHT_EXPORT void								testOpenGl();
	LIBENIGHT_EXPORT bool								isSupportExtension(const char*ext);
	LIBENIGHT_EXPORT void								init_opengl_dbg_win(bool syn);
	LIBENIGHT_EXPORT bool								initGlew();
	LIBENIGHT_EXPORT size_t								glTypeSize(unsigned int  type);

	///////////////////////////////////
	/*Multisampling*/
	///////////////////////////////////
	LIBENIGHT_EXPORT bool								isSupportMultisampling();

}

#endif

