/************************************************************************/
/* 2020.08.28 15:21 xiangtao                                                                     */
/************************************************************************/


#ifndef ____RASTER_TILE__
#define ____RASTER_TILE__

#include "decl.h"
#include "rasterExtent.h"
namespace scene
{
	class RasterTile;
	class RasterResource;
	class RasterLevel;
	struct TileData;

	class LIBENIGHT_EXPORT RasterTileIdentifier
	{
	public:
		RasterTileIdentifier(int, int, int);

		inline bool operator == (const RasterTileIdentifier& other) const { return (level_ == other.level_) && (x_ == other.x_) && (y_ == other.y_); }
		inline bool operator != (const RasterTileIdentifier& other) const { return (level_ != other.level_) || (x_ != other.x_) || (y_ != other.y_); }

		int level_;
		int x_;
		int y_;
	};

	struct TileIdentifierComp {
		bool operator() (const RasterTileIdentifier* lhs, const RasterTileIdentifier* rhs) const
		{
			if (lhs->level_ == rhs->level_)
			{
				if (lhs->x_ == rhs->x_)
					return lhs->y_ < rhs->y_;
				return lhs->x_ < rhs->x_;
			}
			return lhs->level_ < rhs->level_;
		}
	};


	class LIBENIGHT_EXPORT RasterTileRegion
	{
	public:

		RasterTileRegion();

		void setLeft(int);
		void setRight(int);
		void setTop(int);
		void setButtom(int);
		void setTitle(RasterTile*);

		int left()const;
		int right()const;
		int top()const;
		int buttom()const;

		RasterTile*	title()const;
		
		RasterTileRegion(RasterTile* tile, int left, int buttom, int right, int top);

	private:
		RasterTile* tile_;
		RasterExtent extent_; //the extent in this title
		
	};

	class LIBENIGHT_EXPORT RasterTile
	{
	public:

		RasterTile(RasterResource*, const RasterTileIdentifier*);

		virtual ~RasterTile();

		TileData*		loadData();
		RasterLevel *   level()const;

		const RasterTileIdentifier*		identifier()const;
		const RasterExtent* extent()const;

		RasterTile * southwestChild();
		RasterTile * southeastChild();
		RasterTile * northwestChild();
		RasterTile * northeastChild();

	private:
		RasterLevel*				level_ = nullptr;
		RasterTileIdentifier		identifier_;
		RasterExtent				extend_;
	};
}

#endif
