//
//  antialias.h
//  openGLTest
//
//  Created by xt on 18/6/9.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef antialias_h
#define antialias_h

#include "decl.h"

namespace render {

	class LIBENIGHT_EXPORT Antialias
	{
	public:

		enum TYPE
		{
			TEXTURE,
			TEXTURE_NO_RESOLVE,
			FRAMEBUF
		};

		Antialias(TYPE t);

		bool       init();

		void       bindMultSampleFrameBuffer();
		void       blitFrameBuffer();
		void       bindTexture();

		void       bindMultSampleTexture();
	private:
		//create custom multiSample buffer
		bool        createSupportMultisampleBuffer(int width, int height, int samples);
		bool        createTextureForDivision(int width, int height);
		unsigned int     textureColorBufferMultiSampled_;
		unsigned int    framebuffer_;

		unsigned int    intermediateFBO_;
		int				width_;
		int				hight_;
		unsigned int    screenTexture_;
		TYPE        type_;
	};
}

#endif /* antialias_h */
