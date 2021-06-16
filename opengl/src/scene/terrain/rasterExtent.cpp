#include "rasterExtent.h"

namespace scene
{

	RasterExtent::RasterExtent()
	{

	}

	RasterExtent::RasterExtent(const RasterExtent&other)
	{
		left_ = other.left_;
		right_ = other.right_;
		buttom_ = other.buttom_;
		top_ = other.top_;
	}

	RasterExtent::RasterExtent(int left, int right, int top, int buttom)
	{
		left_ = left;
		right_ = right;
		buttom_ = buttom;
		top_ = top;
	}

	RasterExtent& RasterExtent::operator=(const RasterExtent&other)
	{
		left_ = other.left_;
		right_ = other.right_;
		buttom_ = other.buttom_;
		top_ = other.top_;
		return *this;
	}

}



