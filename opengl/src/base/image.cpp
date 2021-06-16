#include "image.h"
#include "color.h"
#include "rgbe.h"
#include "imgLoad.h"
#include "gls.h"
#include "blockDXT.h"

#include <algorithm>

namespace base
{
	Image::Image()
	{
		initParam();
	}

	Image::~Image()
	{
		destory();
	}


	void Image::initParam()
	{
		//For 1D textures pixelHeight and pixelDepth must be 0
		//For 2D and cube textures pixelDepth must be 0.
		width_ = 0;
		height_ = 0;
		depth_ = 0;

		numberOfArrayElements_ = 0;//If the texture is not an array texture, numberOfArrayElements must equal 0.
		cubeMap_ = false;
		compressed_ = false;
		numOfMiplevels_ = 1;
		faces_ = 1;

		type_ = GL_NONE;

		internalformat_ = GL_RGB;
		baseInternalformat_ = GL_RGB;
		format_ = GL_RGB;

		typesize_ = 0;//specifies the data type size that should be used when endianness conversion is required for the texture data stored in the file

		elementSize_ = 1;

		data_ = nullptr;

		blockSize_ = 0; //for compressed texture
		blockSize_x_ = 4;
		blockSize_y_ = 4;

		
		levelDataPtr_ = nullptr;

	}

	void Image::allocate(uint w, uint h)
	{
		allocate(width_, height_, elementSize_);
	}

	void Image::allocate(uint size)
	{
		data_ = new int8[size];
	}

	void Image::allocate(uint w, uint h, uint elementSize)
	{
		destory();
		width_ = w;
		height_ = h;
		elementSize_ = elementSize;

		allocate(w * h * elementSize_);
	}

	void  * Image::pixels()
	{
		return data_;
	}

	void Image::releaseData()
	{
		destory();
	}

	bool Image::isValid() const
	{
		return (data_ != nullptr);
	}

	uint Image::width() const
	{
		return width_;
	}

	uint Image::height() const
	{
		return height_;
	}

	uint Image::depth() const
	{
		return depth_;
	}

	uint Image::numOfMiplevels()const
	{
		return numOfMiplevels_;
	}

	uint Image::target()const
	{
		return target_;
	}
	uint Image::internalformat()const
	{
		return internalformat_;
	}
	uint Image::baseInternalformat()const
	{
		return baseInternalformat_;
	}
	uint Image::format()const
	{
		return format_;
	}

	uint Image::type()const
	{
		return type_;
	}
	uint Image::typesize()const
	{
		return typesize_;
	}

	uint16 Image::faces()const
	{
		return faces_;
	}

	math::uint16& Image::rfaces()
	{
		return faces_;
	}


	bool Image::cubMap() const
	{
		return cubeMap_;
	}


	math::uint8 Image::components() const
	{
		return numofComponet_;
	}

	void Image::setwidth(uint val)
	{
		width_ = val;
	}
	void Image::setheight(uint val) {
		height_ = val;
	}
	void Image::setdepth(uint val) {
		depth_ = val;
	}
	void Image::setnumOfMiplevels(uint val) {
		numOfMiplevels_ = val;
	}
	void Image::settarget(uint val) {
		target_ = val;
	}
	void Image::setinternalformat(uint val) {
		internalformat_ = val;
	}
	void Image::setbaseInternalformat(uint val) {
		baseInternalformat_ = val;
	}

	void Image::setNumberOfArrayELements(uint n)
	{
		numberOfArrayElements_ = n;
	}

	void Image::setformat(uint val) {
		format_ = val;
	}
	void Image::settype(uint val) {
		type_ = val;
	}
	void Image::settypesize(uint val) {
		typesize_ = val;
	}

	void Image::setfaces(uint val) {
		faces_ = val;
	}


	void Image::setElementSize(uint16 s)
	{
		elementSize_ = s;
	}

	void Image::setCubeFlag(bool f)
	{
		cubeMap_ = f;
	}


	void Image::setComponents(uint8 c)
	{
		numofComponet_ = c;
	}

	void* Image::getLevel(uint level)
	{
		return getLevel(level, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	}

	void * Image::getLevel(int32 level, uint32 face)
	{
		assert(level < numOfMiplevels_);
		assert(face >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		assert(face == GL_TEXTURE_CUBE_MAP_POSITIVE_X || cubeMap_);

		face = face - GL_TEXTURE_CUBE_MAP_POSITIVE_X;

		// make sure we don't hand back a garbage pointer
		if (level >= numOfMiplevels_ || (int32)face >= faces_)
			return NULL;

		return levelDataPtr_[face*numOfMiplevels_ + level];
	}


	math::int32 Image::getImageSize(int32 level /*= 0*/) const
	{
		bool compressed = isCompressed();
		int32 w = width_ >> level;
		int32 h = height_ >> level;
		int32 d = depth_ >> level;
		w = (w) ? w : 1;
		h = (h) ? h : 1;
		d = (d) ? d : 1;
		int32 bw = (compressed) ? (w - 1) / blockSize_x_ + 1 : w;
		int32 bh = (compressed) ? (h - 1) / blockSize_y_ + 1 : h;
		int32 elementSize = elementSize_;

		return bw*bh*d*elementSize;
	}

	void Image::flipSurface(uint8 *surf, uint width, uint height, uint depth)
	{
		uint lineSize;

		depth = (depth) ? depth : 1;

		if (!isCompressed()) {
			lineSize = elementSize_ * width;
			uint sliceSize = lineSize * height;

			uint8 *tempBuf = new uint8[lineSize];

			for (uint ii = 0; ii < depth; ii++) {
				uint8 *top = surf + ii*sliceSize;
				uint8 *bottom = top + (sliceSize - lineSize);

				for (uint jj = 0; jj < (height >> 1); jj++) {
					memcpy(tempBuf, top, lineSize);
					memcpy(top, bottom, lineSize);
					memcpy(bottom, tempBuf, lineSize);

					top += lineSize;
					bottom -= lineSize;
				}
			}

			delete[]tempBuf;
		}
		else
		{
			void(*flipblocks)(uint8*, uint32);
			width = (width + 3) / 4;
			height = (height + 3) / 4;
			uint32 blockSize = 0;

			switch (format_)
			{
			case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
				blockSize = 8;
				flipblocks = &Image::flip_blocks_dxtc1;
				break;
			case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
				blockSize = 16;
				flipblocks = &Image::flip_blocks_dxtc3;
				break;
			case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
				blockSize = 16;
				flipblocks = &Image::flip_blocks_dxtc5;
				break;
			case GL_COMPRESSED_LUMINANCE_LATC1_EXT:
			case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT:
			case GL_COMPRESSED_RED_RGTC1:
			case GL_COMPRESSED_SIGNED_RED_RGTC1:
				blockSize = 8;
				flipblocks = &Image::flip_blocks_bc4;
				break;
			case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT:
			case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT:
			case GL_COMPRESSED_RG_RGTC2:
			case GL_COMPRESSED_SIGNED_RG_RGTC2:
				blockSize = 16;
				flipblocks = &Image::flip_blocks_bc5;
				break;
			default:
				return;
			}

			lineSize = width * blockSize;
			uint8 *tempBuf = new uint8[lineSize];

			uint8 *top = surf;
			uint8 *bottom = surf + (height - 1) * lineSize;

			for (uint j = 0; j < std::max((uint)height >> 1, (uint)1); j++)
			{
				if (top == bottom)
				{
					flipblocks(top, width);
					break;
				}

				flipblocks(top, width);
				flipblocks(bottom, width);

				memcpy(tempBuf, top, lineSize);
				memcpy(top, bottom, lineSize);
				memcpy(bottom, tempBuf, lineSize);

				top += lineSize;
				bottom -= lineSize;
			}
			delete[]tempBuf;
		}
	}

	math::uint Image::componentSwapSurface(uint8 *surf, uint width, uint height, uint depth)
	{
		depth = (depth) ? depth : 1;

		if (type_ != GL_UNSIGNED_BYTE)
			return format_;

		if (isCompressed())
			return format_;

		if (format_ == GL_BGR) {
			for (int32 ii = 0; ii < depth; ii++) {
				for (int32 jj = 0; jj < height; jj++) {
					for (int32 kk = 0; kk < width; kk++) {
						uint8 tmp = surf[0];
						surf[0] = surf[2];
						surf[2] = tmp;
						surf += 3;
					}
				}
			}
			return GL_RGB;
		}
		else if (format_ == GL_BGRA) {
			for (int32 ii = 0; ii < depth; ii++) {
				for (int32 jj = 0; jj < height; jj++) {
					for (int32 kk = 0; kk < width; kk++) {
						uint8 tmp = surf[0];
						surf[0] = surf[2];
						surf[2] = tmp;
						surf += 4;
					}
				}
			}
			return GL_RGBA;
		}
		return format_;
	}

	void Image::expandDXT(uint8 *surf, uint width, uint height, uint depth, uint srcSize)
	{
		depth = (depth) ? depth : 1;

		uint32* dest = (uint32*)surf;
		uint32* plane = dest;

		uint8* srcBlock = new uint8[srcSize];
		uint8* src = srcBlock;
		memcpy(src, surf, srcSize);

		int32 bh = (height + 3) / 4;
		int32 bw = (width + 3) / 4;

		if (format_ == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) {
			for (int32 k = 0; k < depth; k++) {

				for (int32 j = 0; j < bh; j++) {
					int32 yBlockSize = std::min(4U, height - 4 * j);

					for (int32 i = 0; i < bw; i++) {
						int32 xBlockSize = std::min(4U, width - 4 * i);
						base::BlockDXT1* block = (base::BlockDXT1*)src;
						base::ColorBlock color;

						block->decodeBlock(&color);

						// Write color block.
						for (int32 y = 0; y < yBlockSize; y++) {
							for (int32 x = 0; x < xBlockSize; x++) {
								plane[4 * i + x + (4 * j + y)*width] = (uint32)color.color(x, y);
							}
						}

						src += sizeof(base::BlockDXT1); // 64bits
					}
				}

				plane += width * height;
			}
		}
		else if (format_ == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) {
			for (int32 k = 0; k < depth; k++) {

				for (int32 j = 0; j < bh; j++) {
					int32 yBlockSize = std::min(4U, height - 4 * j);

					for (int32 i = 0; i < bw; i++) {
						int32 xBlockSize = std::min(4U, width - 4 * i);
						base::BlockDXT3* block = (base::BlockDXT3*)src;
						base::ColorBlock color;

						block->decodeBlock(&color);

						// Write color block.
						for (int32 y = 0; y < yBlockSize; y++) {
							for (int32 x = 0; x < xBlockSize; x++) {
								plane[4 * i + x + (4 * j + y)*width] = (uint32)color.color(x, y);
							}
						}

						src += sizeof(base::BlockDXT3); // 64bits
					}
				}

				plane += width * height;
			}
		}
		else {
			for (int32 k = 0; k < depth; k++) {

				for (int32 j = 0; j < bh; j++) {
					int32 yBlockSize = std::min(4U, height - 4 * j);

					for (int32 i = 0; i < bw; i++) {
						int32 xBlockSize = std::min(4U, width - 4 * i);
						base::BlockDXT5* block = (base::BlockDXT5*)src;
						base::ColorBlock color;

						block->decodeBlock(&color);

						// Write color block.
						for (int32 y = 0; y < yBlockSize; y++) {
							for (int32 x = 0; x < xBlockSize; x++) {
								plane[4 * i + x + (4 * j + y)*width] = (uint32)color.color(x, y);
							}
						}

						src += sizeof(base::BlockDXT5); // 64bits
					}
				}

				plane += width * height;
			}
		}

		delete[] srcBlock;
	}


	bool Image::isCompressed() const
	{
		switch (format_) {
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_COMPRESSED_LUMINANCE_LATC1_EXT:
		case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT:
		case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT:
		case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT:
		case GL_COMPRESSED_RG_RGTC2:
		case GL_COMPRESSED_RED_RGTC1:
		case GL_COMPRESSED_SIGNED_RG_RGTC2:
		case GL_COMPRESSED_SIGNED_RED_RGTC1:
		case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
		case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
		case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
		case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
		case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
			return true;
		}
		return false;
	}

	void Image::destory()
	{
		if (data_)
		{
			delete[] data_;
			data_ = nullptr;
		}
		if (levelDataPtr_)
		{
			delete[] levelDataPtr_;
			levelDataPtr_ = nullptr;
		}
	}

	OpenExrImage::~OpenExrImage()
	{
	}

	HDRImage::HDRImage() :Image(), hilodata_(nullptr)
	{
	}

	HDRImage::~HDRImage()
	{
	}

	/*
	bool HDRImage::load(const char * name)
	{
		bool result = Image::load(name);
		analyze();
		convertHILO();
		target_ = GL_TEXTURE_2D;
		internalformat_ = GL_RGB16F;
		baseInternalformat_ = GL_RGB;
		format_ = GL_RGB;
		type_ = GL_FLOAT;
		return result;
	} */
	void HDRImage::analyze()
	{
		max_r_ = max_g_ = max_b_ = 0.0;
		min_r_ = min_g_ = min_b_ = 1e10;
		unsigned char mine = 255;
		unsigned char maxe = 0;

		unsigned char *ptr = (unsigned char*)data_;
		for (int i = 0; i < width()*height(); i++) {
			unsigned char e = *(ptr + 3);
			if (e < mine) mine = e;
			if (e > maxe) maxe = e;

			float r, g, b;
			rgbe2float(&r, &g, &b, ptr);
			if (r > max_r_) max_r_ = r;
			if (g > max_g_) max_g_ = g;
			if (b > max_b_) max_b_ = b;
			if (r < min_r_) min_r_ = r;
			if (g < min_g_) min_g_ = g;
			if (b < min_b_) min_b_ = b;
			ptr += 4;
		}
		fprintf(stderr, "max intensity: %f %f %f\n", max_r_, max_g_, max_b_);
		fprintf(stderr, "min intensity: %f %f %f\n", min_r_, min_g_, min_b_);
		fprintf(stderr, "max e: %d = %f\n", maxe, ldexp(1.0, maxe - 128));
		fprintf(stderr, "min e: %d = %f\n", mine, ldexp(1.0, mine - 128));

		max_ = max_r_;
		if (max_g_ > max_) max_ = max_g_;
		if (max_b_ > max_) max_ = max_b_;
		fprintf(stderr, "max: %f\n", max_);

	}

	static float remap(float x, float max)
	{
		if (x > max) x = max;
		return (x / max);
	}

	bool HDRImage::convertHILO()
	{
		hilodata_ = new float[width()*height() * 3];

		unsigned char *src = (unsigned char*)data_;
		float *dest = hilodata_;

		for (int i = 0; i < width_*height_; i++)
		{
			float r, g, b;
			rgbe2float(&r, &g, &b, src);

			dest[0] = remap(r, max_);
			dest[1] = remap(g, max_);
			dest[2] = remap(b, max_);

			src += 4;
			dest += 3;
		}
		destory();
		data_ = hilodata_;
		return true;
	}

};
