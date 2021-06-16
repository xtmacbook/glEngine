
#include "imgLoad.h"
#include "image.h"
#include "log.h"
namespace IO {
	base::Image* ImageFile::loadBMP(StdInputStream * stream)
	{
		struct bmpHeader
		{
			unsigned char type[2];
			int f_lenght;
			short rezerved1;
			short rezerved2;
			int offBits;
		};
		struct header_info
		{
			int size;
			int width;
			int height;
			short planes;
			short bitCount;
			int compresion;
			int sizeImage;
			int xPelsPerMeter;
			int yPelsPerMeter;
			int clrUsed;
			int clrImportant;
		};


		uint8 header[54]; // Each BMP file begins by a 54-bytes header
		uint32 dataPos;       // Position in the file where the actual data begins
		uint32 width, height;
		uint32 imageSize;     // = width*height*3
		uint8* data;

		stream->serializeData(header, 54);
		if (stream->isError())
		{
			Log::instance()->printMessage("read bmp file is error !\n");
			return nullptr;
		}

		if (header[0] != 'B' || header[1] != 'M') {
			Log::instance()->printMessage("Not a correct BMP file\n"); 
			return nullptr;
		}
		// Read ints from the byte array
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
		if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

		data = new uint8[imageSize];
		stream->serializeData(data, imageSize);

		base::Image * image = new base::Image();

		image->allocate(width, height,3);

		image->levelDataPtr_ = new uint8*[1];
		image->levelDataPtr_[0] = (uint8*)image->pixels();

		uint8 * dst = (uint8*)image->pixels();
		const int size = image->height() * image->width();
		const uint8 * src = data;

		for (int i = 0; i < size; i++)
		{
			*dst++ = src[0];
			*dst++ = src[1];
			*dst++ = src[2];

			src += 3;
		}

		return image;
	}
}
