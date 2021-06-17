/************************************************************************/
/* 2020.08.31 14:21 xiangtao                                                                     */
/************************************************************************/


#ifndef ____RASTER_LEVEL__
#define ____RASTER_LEVEL__

#include "decl.h"
#include "rasterExtent.h"
#include "rasterTile.h"
#include <vector>
namespace scene
{
	class	RasterResource;
	class	RasterTileRegion;

	class LIBENIGHT_EXPORT RasterLevel
	{
	public:
		RasterLevel(RasterResource* source, int level, GeodeticExtent extent, int rasterXnum, 
			int rasterYnum, int tileWidth, int tileHeigh);

		virtual ~RasterLevel();

		RasterResource*		resource();
		int					level()const;

		int					rasterXNum()const;
		int					rasterYNum()const;
		int					titleWidth()const;
		int					titleHeigh()const;

		double				xtoIndex(double x)const;
		double				ytoIndex(double y)const;
		double				indextoX(int idx)const;
		double				indextoY(int idx)const;

		void 				getTilesInExtent(int left, int buttom, int right, int top,std::vector<RasterTileRegion>&)const;

	private:
		int					level_;

		RasterResource *	resources_ = nullptr;
		GeodeticExtent		extent_;

		int					rasterXNum_;
		int					rasterYNum_;
		int					titleWidth_;
		int					titleHeigh_;
		double				deltaX_;
		double				deltaY_;
	};
}

#endif
