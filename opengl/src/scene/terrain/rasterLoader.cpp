
#include "rasterTile.h"
#include "rasterUpdater.h"
#include "rasterLoader.h"
#include "fence.h"
#include "glu.h"

namespace scene {
	RasterLoader::RasterLoader(RasterDataQeq*r):dataQ_(r)
	{

	}

	RasterLoader::~RasterLoader()
	{

	}

	void RasterLoader::run()
	{
		//gluMakeCurrentShareContext();

		if (!dataQ_) return;

		while (true)
		{
			TileLoadRequest * request = nullptr;

			dataQ_->requestListMutex_.lock();

			while (dataQ_->requestList_.empty() && !dataQ_->stopRequest_)
			{
				dataQ_->requestCondition_.wait(dataQ_->requestListMutex_, 0xFFFFFFFF);//INFINITE
			}

			if (dataQ_->stopRequest_&& dataQ_->requestList_.empty() == 0)
			{
				dataQ_->requestListMutex_.unlock();
				//break; 
				return;
			}

			request = dataQ_->requestList_.firstRequest();
			dataQ_->requestList_.removeFistNode();
		
			dataQ_->requestListMutex_.unlock();
		
			if (request)
			{
				if (request)
				{
					RasterTile * tile = request->title_;
					request->data_ = tile->loadData();

					/*base::Fence * fence = new base::FenceGL3x;
					fence->clientWait();*/

					//dataQ_->doneQue_.post(request);
				}
			}
		}
	}
}

