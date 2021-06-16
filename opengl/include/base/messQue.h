
#ifndef __MESSAGEQUEUE_H__
#define __MESSAGEQUEUE_H__

#include "decl.h"
#include "synchron.h"
#include "callback.h"
#include <vector>
//2020.09.08 16.04
namespace base
{
	template<class R, class M>
	class LIBENIGHT_EXPORT MessageQueues
	{
	public:

		ResultCallback1<R,M*>*  queueCallback_ = nullptr;

		struct MessageInfo
		{
			ResultCallback1<R, M*>* callback_ = nullptr;
			M*  info_;
		};

		enum State
		{
			STOPPED,
			RUNNING,
			STOPPING,
		};

		void post(M* message)
		{
			mutex_.lock();

			MessageInfo info;
			info.callback_ = nullptr;
			info.info_ = message;
			queue_.push_back(info);

			mutex_.unlock();
		}

		void processMessage(MessageInfo&message)
		{
			if (message.callback_)
			{
				message.callback_->Run(message.info_);
			}
			else
			{
				if (queueCallback_)
					queueCallback_->Run(message.info_);
			}
		}

		void processQueue()
		{
			std::vector<MessageInfo> copyQue;

			mutex_.lock();

			if (queue_.size() > 0)
			{
				copyQue.resize(queue_.size());
				std::copy(queue_.begin(), queue_.end(), copyQue.begin());
				queue_.clear();
			}
			
			mutex_.unlock();

			if (!copyQue.empty())
			{
				for (int i = 0; i < copyQue.size(); i++)
					processMessage(copyQue[i]);
			}
		}

	
	private:
		SameProcessMutex  mutex_;
		std::vector<MessageInfo> queue_;
		State	state_;
	};
}

#endif
