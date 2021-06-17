/************************************************************************/
/* 2020.08.28 15:21 xiangtao                                                                     */
/************************************************************************/


#ifndef ____RASTER_SOURCE__
#define ____RASTER_SOURCE__

#include "decl.h"
#include <map>

#include "rasterExtent.h"
#include "smartPointer.h"

class Texture;

namespace scene
{
	class RasterLoader;
	class RasterLevel;

	struct TileData
	{
		base::SmartPointer<::Texture>	texture_ ;
		virtual ~TileData();
	};
	
	class LIBENIGHT_EXPORT RasterResource
	{
	public:
		virtual ~RasterResource();

		virtual		bool				init() = 0;

		virtual const GeodeticExtent*	extend()const;

		int								getLevelsNum()const;
		int								getTitleWidth()const;
		int								getTileHeigh()const;

		RasterTile *					getTile(const RasterTileIdentifier*);

		virtual TileData *				loadTileData(const RasterTileIdentifier*) = 0;

		void							setNumLevel(int num);
		void							setTileWH(int w, int h);
		void							setlevelZeroDeltaRasterWH(int w, int h);
		void							setExtent(const GeodeticExtent&);

		RasterLevel *					level(int);

		std::map<const RasterTileIdentifier*, RasterTile*, TileIdentifierComp>	activeTiles_;
		int								num_levels_;
		int								tile_with_;
		int								tile_hight_;

		double							levelZeroDeltaRasterW_ = 0.0;
		double							levelZeroDeltaRasterH_ = 0.0;
		GeodeticExtent					extend_;

	protected:
		std::map<int, RasterLevel*>	levels_;
	};

	class LIBENIGHT_EXPORT TerrainResource :public RasterResource
	{
	public:

		TerrainResource();

		virtual ~TerrainResource();
		virtual		bool				init();
		virtual TileData *				loadTileData(const RasterTileIdentifier*);

	};
}

#endif
