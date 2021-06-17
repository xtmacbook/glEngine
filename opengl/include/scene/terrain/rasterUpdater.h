/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/

#ifndef ____RASTER_UPDATER__
#define ____RASTER_UPDATER__

#include "decl.h"
#include "synchron.h"
#include "dataQueue.h"
#include "messQue.h"
#include "rasterTile.h"
#include "BaseObject.h"
#include <map>
#include <list>
#include <vector>

namespace scene
{
	class RasterResource;
	class RasterLevel;
	class RasterTile;
	class TerrainRasterLevel;
	class RasterLoader;

	struct TileData;

	class LIBENIGHT_EXPORT TileLoadRequest
	{
	public:

		TileLoadRequest(TerrainRasterLevel* , RasterTile *);

		~TileLoadRequest();

		RasterTile *			title_ = nullptr;
		TileData    *			data_ = nullptr;
		TerrainRasterLevel*		level_ = nullptr;
		bool					isNode_ = false; //is one node of list(be reference by one list/queue)


		bool operator==(const TileLoadRequest*other)const
		{
			return (this->title_->identifier() == other->title_->identifier());
		}
		bool operator!=(const TileLoadRequest*other)const
		{
			return (this->title_->identifier() != other->title_->identifier());
		}
	};

	struct LIBENIGHT_EXPORT RasterDataQeq
	{
	public:
		std::map <const RasterTileIdentifier*, TileLoadRequest*, TileIdentifierComp> loadingTiles_;
		std::map <const RasterTileIdentifier*, TileData*, TileIdentifierComp> loadedTiles_;
		base::DataQequest<TileLoadRequest> requestList_;
		
		typedef std::map <const RasterTileIdentifier*, TileLoadRequest*, TileIdentifierComp>::const_iterator LOADINGCITER;
		typedef std::map <const RasterTileIdentifier*, TileData*, TileIdentifierComp>::const_iterator TEXTUREITER;

		bool					  stopRequest_ = false;

		base::SameProcessMutex	  requestListMutex_;
		base::ConditionVar        requestCondition_;

		base::MessageQueues<int, TileLoadRequest> doneQue_;
	};

	class LIBENIGHT_EXPORT RasterUpdater :public base::BaseObject
	{
	public:
		RasterUpdater();

		virtual ~RasterUpdater();

		virtual void	requestTileResidency(TerrainRasterLevel * ,RasterLevel*,const RasterExtent*);
		virtual void	requestTileLoad(TerrainRasterLevel * ,RasterTile * );
		virtual void	applyNewData();
		virtual void	applyNewTile(RasterTile * title, TerrainRasterLevel*) = 0;
		void			stopUpdater();
		int				getDoneReqs(TileLoadRequest * rq, std::vector<TileLoadRequest*>*doneQ);
		void			applyIfNotLoaded(TerrainRasterLevel *, RasterTile*);

	protected:
		RasterDataQeq		detail_;
		RasterLoader *		loader_ = nullptr;
	private:
		RasterResource *	resource_ = nullptr;
	};
}

#endif
