/************************************************************************/
/* 2020.08.28 11:47 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RASTER_LOADER__
#define ____RASTER_LOADER__

#include "decl.h"

#include "thread.h"

namespace scene
{
	struct RasterDataQeq;

	class LIBENIGHT_EXPORT RasterLoader :public base::Thread
	{
	public:
		RasterLoader( RasterDataQeq*);

		virtual ~RasterLoader();

		virtual void run();
		
		RasterDataQeq *  dataQ_ = nullptr;
	};
}

#endif
