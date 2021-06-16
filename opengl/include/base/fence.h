#pragma once

#ifndef __FENCE_H__
#define __FENCE_H__
#include "decl.h"

/************************************************************************/
/* xt 2020.09.03 15:09                                                                     */
/************************************************************************/
#include "gls.h"

namespace base
{

	enum SynchronizationStatus
	{
		Unsignaled,
		Signaled
	};

	enum ClientWaitResult
	{
		AlreadySignaled,
		ConSignaled,
		TimeoutExpired
	};

	class LIBENIGHT_EXPORT Fence
	{
	public:

		virtual void serverWait() = 0;
		virtual ClientWaitResult clientWait() = 0;
		virtual ClientWaitResult clientWait(int timeoutInNanoseconds) = 0;
		virtual SynchronizationStatus status() = 0;
	};

	class LIBENIGHT_EXPORT FenceGL3x : public Fence
	{
	public:

		FenceGL3x();
		virtual ~FenceGL3x();

		virtual void serverWait();
		virtual ClientWaitResult clientWait();
		virtual ClientWaitResult clientWait(int timeoutInNanoseconds);
		virtual SynchronizationStatus status();

	private:
		GLsync syncObj_;
	};
}


#endif
