
#include "rasterLevel.h"
#include "rasterSource.h"

namespace scene
{

	RasterLevel::RasterLevel(RasterResource* source, int level, GeodeticExtent extent, 
		int rasterXnum, int rasterYnum, int tileWidth, int tileHeigh)
	{
		resources_ = source;
		level_ = level;
		extent_ = extent;

		rasterXNum_ = rasterXnum;//all grid nums
		rasterYNum_ = rasterYnum;

		titleWidth_ = tileWidth;
		titleHeigh_ = tileHeigh;

		deltaX_ = (extent_.right_ - extent.left_) / (rasterXNum_ - 1);
		deltaY_ = (extent_.top_ - extent.buttom_) / (rasterYNum_ - 1);
	}

	RasterLevel::~RasterLevel()
	{

	}

	RasterResource* RasterLevel::resource()
	{
		return resources_;
	}

	int RasterLevel::level()const
	{
		return level_;
	}

	int RasterLevel::rasterXNum()const
	{
		return rasterXNum_;
	}

	int RasterLevel::rasterYNum()const
	{
		return rasterYNum_;
	}

	int RasterLevel::titleWidth()const
	{
		return titleWidth_;
	}

	int RasterLevel::titleHeigh()const
	{
		return titleHeigh_;
	}

	double RasterLevel::xtoIndex(double x)const
	{
		return (x - extent_.left_) / deltaX_;
	}

	double RasterLevel::ytoIndex(double y)const
	{
		return (y - extent_.buttom_) / deltaY_;
	}

	double RasterLevel::indextoX(int xIndex)const
	{
		return extent_.left_ + xIndex * deltaX_;
	}

	double RasterLevel::indextoY(int yIndex)const
	{
		return extent_.buttom_ + yIndex * deltaY_;
	}

	void  RasterLevel::getTilesInExtent(int left, int buttom, int right, int top, std::vector<RasterTileRegion>&result)const
	{
		int tileXStart = left / titleWidth_;
		int tileXStop = right / titleWidth_;

		if (left < 0) --tileXStart;
		if (right < 0) --tileXStop;

		int tileYStart = buttom / titleHeigh_;
		int tileYStop = top / titleHeigh_;

		if (buttom < 0) --tileYStart;
		if (top < 0) --tileYStop;

		int tileWidth = tileXStop - tileXStart + 1;
		int tileHeight = tileYStop - tileYStart + 1;

		result.resize(tileWidth * tileHeight);
		int resultIndex = 0;

		for (int tileY = tileYStart; tileY <= tileYStop; ++tileY)
		{
			int tileYOrigin = tileY * titleHeigh_;

			int currentSouth = buttom - tileYOrigin;
			if (currentSouth < 0)
				currentSouth = 0;

			int currentNorth = top - tileYOrigin;
			if (currentNorth >= titleHeigh_)
				currentNorth = titleHeigh_ - 1;

			for (int tileX = tileXStart; tileX <= tileXStop; ++tileX)
			{
				int tileXOrigin = tileX * titleWidth_;

				int currentWest = left - tileXOrigin;
				if (currentWest < 0)
					currentWest = 0;

				int currentEast = right - tileXOrigin;
				if (currentEast >= titleWidth_)
					currentEast = titleWidth_ - 1;

				RasterTileIdentifier tileID(level_, tileX, tileY);
				RasterTile* tile = (RasterTile*)resources_->getTile(&tileID);
				result[resultIndex].setTitle(tile);
				result[resultIndex].setButtom(currentSouth);
				result[resultIndex].setTop(currentNorth);
				result[resultIndex].setRight(currentEast);
				result[resultIndex].setLeft(currentWest);

				++resultIndex;
			}
		}
	}

}
