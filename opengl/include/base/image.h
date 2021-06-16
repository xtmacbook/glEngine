#pragma once
#include "decl.h"
#include "type.h"
#include "BaseObject.h"
#include "smartPointer.h"

using namespace math;
namespace base
{
	class Color32;

	class LIBENIGHT_EXPORT Image :public BaseObject
	{
	public:

		Image();
		virtual ~Image();

		void initParam();

		virtual void allocate(uint w, uint h);
		virtual void allocate(uint size);
		virtual void allocate(uint w, uint h, uint elementSize);

		virtual void  * pixels();

		template<class T>
		void	setVal(uint w, uint h, T * r, T * g, T *b);
		template<class T>
		void	getVal(uint w, uint h, T * r, T * g, T *b);
		void	releaseData();

		bool isValid()const;

		uint width() const;
		uint height() const;
		uint depth() const;
		uint numOfMiplevels()const;
		uint target()const;
		uint internalformat()const;
		uint baseInternalformat()const;
		uint format()const;
		uint type()const;
		uint typesize()const;
		uint16 faces()const;
		uint16& rfaces();
		bool  cubMap()const;
		uint8 components()const;

		void setwidth(uint);
		void setheight(uint);
		void setdepth(uint);
		void setnumOfMiplevels(uint);
		void settarget(uint);
		void setinternalformat(uint);
		void setbaseInternalformat(uint);
		void setNumberOfArrayELements(uint);
		void setformat(uint);
		void settype(uint);
		void settypesize(uint);
		void setfaces(uint);
		void setElementSize(uint16);
		void setCubeFlag(bool);
		void setComponents(uint8);

		void* getLevel(uint level);
		/// Get a pointer to the pixel data for a given mipmap level and cubemap face.
		void *getLevel(int32 level, uint32 face);

		int32 getImageSize(int32 level = 0) const;

		void flipSurface(uint8 *surf, uint width, uint height, uint depth);
		uint componentSwapSurface(uint8 *surf, uint width, uint height, uint depth);
		void expandDXT(uint8 *surf, uint width, uint height, uint depth, uint srcSize);

		bool isCompressed()const;

		uint   blockSize_; //for compressed texture
		uint   blockSize_x_;
		uint   blockSize_y_;

		//pointers to the levels
		uint8** levelDataPtr_;

	protected:
		uint	width_;
		uint	height_;
		uint	depth_; //for volume textures

		uint	target_;

		uint	internalformat_;
		uint    baseInternalformat_;
		uint	format_; //For compressed textures, glFormat must equal 0

		uint	type_;

		//maybe delete this attribute and use the levelDataPtr_
		uint	typesize_;//specifies the data type size that should be used when endianness conversion is required for the texture data stored in the file

		uint    numberOfArrayElements_;//If the texture is not an array texture, numberOfArrayElements must equal 0.

		uint16	faces_; //cubemap

		uint16  elementSize_;//bytesPerElement
		uint8	numofComponet_; 

		uint16	numOfMiplevels_;

		bool    compressed_;
		bool    cubeMap_;


	protected:
		void *			data_;
		void			destory();

		static void flip_blocks_dxtc1(uint8 *ptr, uint numBlocks);
		static void flip_blocks_dxtc3(uint8 *ptr, uint numBlocks);
		static void flip_blocks_dxtc5(uint8 *ptr, uint numBlocks);
		static void flip_blocks_bc4(uint8 *ptr, uint numBlocks);
		static void flip_blocks_bc5(uint8 *ptr, uint numBlocks);
	};

	template<class T>
	void base::Image::getVal(uint x, uint y, T * r, T * g, T *b)
	{
		if ((x < width_) && (y < height_))
		{
			T* data = (T*)data_;

			*r = (T)data[((y*width_) + x)* elementSize_];
			*g = (T)data[((y*width_) + x)* elementSize_ + 1];
			*b = (T)data[((y*width_) + x)* elementSize_ + 2];
		}
	}

	template<class T>
	void base::Image::setVal(uint x, uint y, T * r, T * g, T *b)
	{
		T * data = (T*)data_;
		if ((x < width_) && (y < height_))
		{
			data[((y*width_) + x)* elementSize_] = *r;
			data[((y*width_) + x)* elementSize_ + 1] = *g;
			data[((y*width_) + x)* elementSize_ + 2] = *b;
		}
	}


	class LIBENIGHT_EXPORT OpenExrImage :public Image
	{
	public:
		virtual ~OpenExrImage();
	};

	class LIBENIGHT_EXPORT HDRImage :public Image
	{
	public:

		HDRImage();
		virtual ~HDRImage();
		void analyze();
		bool convertHILO();

	private:
		float *hilodata_;

		float max_r_;
		float max_g_;
		float max_b_;
		float min_r_;
		float min_g_;
		float min_b_;
		float max_;

	};

	class LIBENIGHT_EXPORT DDSImage : public Image
	{
	public:

	};
}

typedef base::SmartPointer<base::Image>		Image_SP;

