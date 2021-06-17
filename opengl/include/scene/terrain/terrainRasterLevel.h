/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RENDER_RASTER_LEVEL__
#define ____RENDER_RASTER_LEVEL__

#include "rasterExtent.h"
#include "type.h"

using namespace math;

namespace scene
{
	class RasterLevel;

	class LIBENIGHT_EXPORT TerrainRasterLevel
	{
	public:
		TerrainRasterLevel();
		virtual ~TerrainRasterLevel();

		void					setRasterLevel(RasterLevel*);
		RasterLevel*			getRaster();

		void					setLevel(int level);
		int						level()const;

		void					setFiner(TerrainRasterLevel*);
		void					setCoarser( TerrainRasterLevel*);

		TerrainRasterLevel*	finerLevel()const;
		TerrainRasterLevel*	coarserLevel()const;

	protected:
		TerrainRasterLevel *		finerLevel_ = nullptr;
		TerrainRasterLevel *		coarserLevel_ = nullptr;
		RasterLevel*					raster_ = nullptr;
		int								level_;			//current lay level

	};
}

#endif
