#pragma once
#ifndef __OpenGL__ImgLoad__
#define __OpenGL__ImgLoad__

#include "decl.h"
#include "stream.h"

namespace base
{
	class Image;
	class OpenExrImage;
}

namespace IO
{
	struct ImageOption;

    class LIBENIGHT_EXPORT ImageFile
    {
    public:
		static base::Image*				loadDDS(StdInputStream * stream);
		static base::Image*				loadOpenEXRImage(const char *fileName);
		static base::Image*				loadBMP(StdInputStream * stream);
		static base::Image*				loadJPG(StdInputStream * stream);
		static base::Image*				loadPNG(StdInputStream * stream, const ImageOption*);
		static base::Image*				loadKtx(const char*fileName);
	    static base::Image*				loadTga(StdInputStream * stream);
		static base::Image*				loadHDR(const char*fileName);
    };
}
#endif
