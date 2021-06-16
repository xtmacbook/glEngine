
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"
#include "engineLoad.h"

#ifdef ENGINE_PNG
#include <png.h>
#endif
#include <vector>
namespace IO
{
#if defined(ENGINE_PNG)
	static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		StdInputStream * sis;
		sis = static_cast<StdInputStream*>(png_get_io_ptr(png_ptr));
		if (sis == NULL)
			return;   // add custom error handling here
		if (!sis->isLoading()) return;
		uint rd = sis->serializeData(data, (int)length);
		if (sis->isError())
		{
			png_error(png_ptr, "Read png Error");
		}

	}
#endif

	base::Image* ImageFile::loadPNG(StdInputStream * stream, const ImageOption*option)
	{
		png_image image_png;
		memset(&image_png, 0, sizeof(image_png));
		image_png.version = PNG_IMAGE_VERSION;
		image_png.opaque = nullptr;

		std::vector<int8> byte_array;
		byte_array.resize(stream->size());
		stream->serializeData(byte_array.data(), stream->size());

		if (!png_image_begin_read_from_memory(&image_png, (png_const_voidp)byte_array.data(), stream->size()))

			return nullptr;

		size_t size = PNG_IMAGE_SIZE(image_png); // get image size

		base::Image * image = new base::Image;
		image->allocate(size);
		image->levelDataPtr_ = new uint8*[1];
		image->levelDataPtr_[0] = (uint8*)image->pixels();

		if (png_image_finish_read(&image_png, NULL/*background*/, image->pixels(), 0/*row_stride*/, nullptr/*colormap*/))
		{
			size_t pixel_size = 0;

			image->setwidth(image_png.width);
			image->setheight(image_png.height);

			// check file format
			switch (image_png.format)
			{
			case PNG_FORMAT_GRAY:
				pixel_size = 1;
				image->setComponents(1);
				break;
			case PNG_FORMAT_LINEAR_Y:
				pixel_size = 2;
				image->setComponents(1);
				image->settarget(GL_TEXTURE_2D);
				if (!option->clamped_)
					image->setinternalformat(GL_R16F);
				else
					image->setinternalformat(GL_R16);
				image->settype(GL_SHORT);
				image->setformat(GL_RED);
				break;
			case PNG_FORMAT_RGB:
				pixel_size = 3;
				image->setComponents(3);
				image->settarget(GL_TEXTURE_2D);
				image->setinternalformat(GL_RGB8);
				image->settype(GL_UNSIGNED_BYTE);
				image->setformat(GL_RGB);
				break;
			case PNG_FORMAT_RGBA:
				pixel_size = 4;
				image->setComponents(4);
				image->settarget(GL_TEXTURE_2D);
				image->setinternalformat(GL_RGBA8);
				image->settype(GL_UNSIGNED_BYTE);
				image->setformat(GL_RGBA);
				break;
			default:
			{
				printf("undefine png format!\n");
				delete image;
				return nullptr;
			}
			}

			image->settypesize(0);
			image->setfaces(1);
			image->setElementSize(pixel_size);
			png_image_free(&image_png);
			image->setdepth(1);

			return image;

		}
		return nullptr;

	}

}