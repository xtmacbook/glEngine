
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"

namespace IO
{
	//ktx
	struct KTXheader
	{
		unsigned char       identifier[12];
		unsigned int        endianness;
		unsigned int        gltype;
		unsigned int        gltypesize;
		unsigned int        glformat;
		unsigned int        glinternalformat;
		unsigned int        glbaseinternalformat;
		unsigned int        pixelwidth;
		unsigned int        pixelheight;
		unsigned int        pixeldepth;
		unsigned int        numberOfArrayElements;
		unsigned int        faces;
		unsigned int        miplevels;
		unsigned int        keypairbytes;
	};
	static const unsigned char identifier[] =
	{
		0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
	};

	static const unsigned int swap32(const unsigned int u32)
	{
		union
		{
			unsigned int u32;
			unsigned char u8[4];
		} a, b;

		a.u32 = u32;
		b.u8[0] = a.u8[3];
		b.u8[1] = a.u8[2];
		b.u8[2] = a.u8[1];
		b.u8[3] = a.u8[0];

		return b.u32;
	}

	base::Image* ImageFile::loadKtx(const char*filename)
	{
		FILE * fp;
		GLuint temp = 0;
		GLuint retval = 0;
		KTXheader h;
		size_t data_start, data_end;
		GLenum target = GL_NONE;

		fp = fopen(filename, "rb");

		if (!fp)
			return nullptr;

		base::Image * img = nullptr;

		if (fread(&h, sizeof(h), 1, fp) != 1)
			goto fail_read;

		if (memcmp(h.identifier, identifier, sizeof(identifier)) != 0)
			goto fail_header;

		if (h.endianness == 0x04030201)
		{
			// No swap needed
		}
		else if (h.endianness == 0x01020304)
		{
			// Swap needed
			h.endianness = swap32(h.endianness);
			h.gltype = swap32(h.gltype);
			h.gltypesize = swap32(h.gltypesize);
			h.glformat = swap32(h.glformat);
			h.glinternalformat = swap32(h.glinternalformat);
			h.glbaseinternalformat = swap32(h.glbaseinternalformat);
			h.pixelwidth = swap32(h.pixelwidth);
			h.pixelheight = swap32(h.pixelheight);
			h.pixeldepth = swap32(h.pixeldepth);
			h.numberOfArrayElements = swap32(h.numberOfArrayElements);
			h.faces = swap32(h.faces);
			h.miplevels = swap32(h.miplevels);
			h.keypairbytes = swap32(h.keypairbytes);
		}
		else
		{
			goto fail_header;
		}

		// Guess target (texture type)
		if (h.pixelheight == 0)
		{
			if (h.numberOfArrayElements == 0)
				target = GL_TEXTURE_1D;
			else
				target = GL_TEXTURE_1D_ARRAY;
		}
		else if (h.pixeldepth == 0)
		{
			if (h.numberOfArrayElements == 0)
			{
				if (h.faces == 0)
					target = GL_TEXTURE_2D;
				else
					target = GL_TEXTURE_CUBE_MAP;
			}
			else
			{
				if (h.faces == 0)
					target = GL_TEXTURE_2D_ARRAY;
				else
					target = GL_TEXTURE_CUBE_MAP_ARRAY;
			}
		}
		else
			target = GL_TEXTURE_3D;

		// Check for insanity...
		if (target == GL_NONE ||                                    // Couldn't figure out target
			(h.pixelwidth == 0) ||                                  // Texture has no width???
			(h.pixelheight == 0 && h.pixeldepth != 0))              // Texture has depth but no height???
		{
			goto fail_header;
		}

		data_start = ftell(fp) + h.keypairbytes;
		fseek(fp, 0, SEEK_END);
		data_end = ftell(fp);
		fseek(fp, data_start, SEEK_SET);

		img = new base::Image;
		img->allocate(data_end - data_start);
		memset(img->pixels(), 0, data_end - data_start);
		fread(img->pixels(), 1, data_end - data_start, fp);

		if (h.miplevels == 0)
		{
			h.miplevels = 1;
		}
		img->settarget(target);
		img->setnumOfMiplevels(h.miplevels);
		img->setwidth(h.pixelwidth);
		img->setheight(h.pixelheight);
		img->setdepth(h.pixeldepth);
		img->setinternalformat(h.glinternalformat);
		img->settype(h.gltype);
		img->setformat(h.glformat);
		img->settypesize(h.gltypesize);
		img->setbaseInternalformat(h.glbaseinternalformat);
		img->setNumberOfArrayELements(h.numberOfArrayElements);
		img->setfaces(h.faces);

	fail_header:;
	fail_read:;
		fclose(fp);

		return img;
	}
}
