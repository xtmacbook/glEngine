
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"


#if defined(ENGINE_OPENEXR)
#define OPENEXR_DLL
#include <OpenEXR/ImathBox.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfRgbaFile.h>
#endif

namespace IO
{
	base::Image* ImageFile::loadOpenEXRImage(const char *fileName)
	{
#if defined(ENGINE_OPENEXR)
		try
		{
			Imf::Array2D<Imf::Rgba> pixels;

			Imf::RgbaInputFile file(fileName);
			Imath::Box2i dw = file.dataWindow();
			int width = dw.max.x - dw.min.x + 1;
			int heigth = dw.max.y - dw.min.y + 1;

			pixels.resizeErase(heigth, width);
			file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
			file.readPixels(dw.min.y, dw.max.y);

			img->allocate(width, heigth, 4);
			GLfloat* pTex = (float*)img->pixels();

			for (unsigned int v = 0; v < heigth; v++)
			{
				for (unsigned int u = 0; u < width; u++)
				{
					Imf::Rgba texel = pixels[heigth - v - 1][u];
					pTex[0] = texel.r;
					pTex[1] = texel.g;
					pTex[2] = texel.b;
					pTex[3] = texel.a;
					pTex += 4;
				}
			}

		}
		catch (Iex::BaseExc & e)
		{
			std::cerr << e.what() << std::endl;
			PRINT_ERROR(e.what());
			return false;
		}
#endif
		return nullptr;
	}

}
