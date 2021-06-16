
#ifndef __MUTEX_H__
#define __MUTEX_H__
#include "decl.h"
#include <Windows.h>

namespace base {

class LIBENIGHT_EXPORT SameProcessMutex
{
public:

	SameProcessMutex();
	~SameProcessMutex();

	void lock();
	void unlock();

	bool try_lock();

	CRITICAL_SECTION    cs_;
};

class LIBENIGHT_EXPORT Mutex_guard
{
protected:

	SameProcessMutex &_mx;

public:
	explicit Mutex_guard(SameProcessMutex &mx) : _mx(mx) { _mx.lock(); }

	~Mutex_guard() { _mx.unlock(); }

private:
	void operator = (const Mutex_guard&);
};

class LIBENIGHT_EXPORT ConditionVar
{
public:
	ConditionVar();
	void  wait(SameProcessMutex&, int dwMilliseconds);
	void  notify_all();

private:
	CONDITION_VARIABLE  variable_;
};

}  

#endif 
