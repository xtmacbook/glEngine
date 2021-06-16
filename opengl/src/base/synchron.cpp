#include "synchron.h"

#include <Windows.h>

namespace base {
	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	SameProcessMutex::SameProcessMutex()
		: cs_()
	{
		InitializeCriticalSectionAndSpinCount(&cs_, 4);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	SameProcessMutex::~SameProcessMutex()
	{
		DeleteCriticalSection(&cs_);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void SameProcessMutex::lock()
	{
		EnterCriticalSection(&cs_);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void SameProcessMutex::unlock()
	{
		LeaveCriticalSection(&cs_);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool SameProcessMutex::try_lock()
	{
		return 0 != TryEnterCriticalSection(&cs_);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	ConditionVar::ConditionVar()
	{
		InitializeConditionVariable(&variable_);
	}

	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void ConditionVar::wait(SameProcessMutex&mutex, int dwMilliseconds)
	{
		SleepConditionVariableCS(&variable_, &mutex.cs_, dwMilliseconds);
	}
	//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	void ConditionVar::notify_all()
	{
		WakeConditionVariable(&variable_);
	}

}
