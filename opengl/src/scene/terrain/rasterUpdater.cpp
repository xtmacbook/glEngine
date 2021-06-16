#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterUpdater.h"
#include "rasterLevel.h"
#include "helpF.h"
#include "rasterLoader.h"
#include "shader.h"
#include "texture.h"
#include "smartPointer.h"
#include "texture.h"
#include <algorithm>

namespace scene
{

	RasterUpdater::RasterUpdater() :loader_(new RasterLoader(&detail_))
	{
		
	}

	RasterUpdater::~RasterUpdater()
	{

	}

	void RasterUpdater::requestTileResidency(TerrainRasterLevel * rl, RasterLevel* level, const RasterExtent* nextExtent)
	{
		std::vector<RasterTileRegion> rasterTileRegions;
		level->getTilesInExtent(nextExtent->left_, nextExtent->buttom_, nextExtent->right_, nextExtent->top_, rasterTileRegions);

		for (int i = 0; i < rasterTileRegions.size(); i++)
		{
			if (detail_.loadedTiles_.find(rasterTileRegions[i].title()->identifier()) == detail_.loadedTiles_.end())
			{
				requestTileLoad(rl, rasterTileRegions[i].title());
			}
		}
	}

	void RasterUpdater::requestTileLoad(TerrainRasterLevel *rl, RasterTile *title)
	{
		TileLoadRequest* request = nullptr;
		RasterDataQeq::LOADINGCITER iter =  detail_.loadingTiles_.find(title->identifier());
		
		bool noExist = (iter == detail_.loadingTiles_.cend());
		if (noExist)
		{
			request = new TileLoadRequest(rl,title);
		}
		
		detail_.requestListMutex_.lock();
		
		if (!noExist)
		{
			if (!request->isNode_)
			{
				return;//now is loading this request by other thread
			}
			detail_.requestList_.removeRequest(request);
		}
		
		detail_.requestList_.addRequest(request);
		detail_.requestListMutex_.unlock();
		detail_.requestCondition_.notify_all();
		detail_.loadingTiles_[title->identifier()] = request;

	}

	int RasterUpdater::getDoneReqs(TileLoadRequest * rq, std::vector<TileLoadRequest*>*doneQ)
	{
		detail_.loadedTiles_[rq->title_->identifier()] = rq->data_;
		detail_.loadingTiles_.erase(rq->title_->identifier());
		doneQ->push_back(rq);
		return 0;
	}

	void RasterUpdater::applyIfNotLoaded(TerrainRasterLevel *level, RasterTile*tile)
	{
		RasterDataQeq::TEXTUREITER loadTexture = detail_.loadedTiles_.find(tile->identifier());
		if (loadTexture == detail_.loadedTiles_.end() || loadTexture->second->texture_.addr() == 0)
		{
			applyNewTile(tile,level);
		}
	}

	void RasterUpdater::applyNewData()
	{
		std::vector<TileLoadRequest*> doneQueue;
		detail_.doneQue_.queueCallback_ = base::NewPermanentCallback(this,&RasterUpdater::getDoneReqs,&doneQueue);
		detail_.doneQue_.processQueue();
		for each (TileLoadRequest* req in doneQueue)
		{
			applyNewTile(req->title_, req->level_);
		}
		delete detail_.doneQue_.queueCallback_;
	}
	
	void RasterUpdater::stopUpdater()
	{
		detail_.requestListMutex_.lock();
		detail_.stopRequest_ = true;
		detail_.requestListMutex_.unlock();
	}

	TileLoadRequest::TileLoadRequest(TerrainRasterLevel*level, RasterTile *title):title_(title),
		level_(level)
	{
		data_ = new TileData;
		data_->texture_ = new Texture;
		data_->texture_->createObj();

	}


	TileLoadRequest::~TileLoadRequest()
	{
		if (data_)
		{
			delete data_;
			data_ = nullptr;
		}
	}

}
