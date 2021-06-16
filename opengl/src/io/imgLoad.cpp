#include "imgLoad.h"
#include "stream.h"
#include "log.h"
#include "image.h"
#include "color.h"
#include "rgbe.h"

#include <vector>

using namespace math;

namespace IO
{
	base::Image* ImageFile::loadHDR(const char*file)
	{
		FILE *fp = fopen(file, "rb");
		if (!fp) {
			std::string err = "Error opening file:";
			err += file;
			PRINT_ERROR(err.c_str());
			return nullptr;
		}

		rgbe_header_info header;
		int w, h;
		if (RGBE_ReadHeader(fp, &w, &h, &header))
			return nullptr;

		base::OpenExrImage *img = new base::OpenExrImage();

		img->setwidth(w);
		img->setheight(h);
		img->setdepth(4);
		img->allocate(w * h * 4);

		//  if (RGBE_ReadPixels_RLE(fp, img.data, img.width, img.height))
		if (RGBE_ReadPixels_Raw_RLE(fp, (unsigned char*)img->pixels(), w, h))
		{
			delete img;
			return nullptr;
		}

		fclose(fp);

		return img;
	}
	
}
