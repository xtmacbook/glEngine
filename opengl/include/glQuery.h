

#ifndef __OpenGL__glQuery__
#define __OpenGL__glQuery__

#include "decl.h"
#include "type.h"


class LIBENIGHT_EXPORT GLQuery
{
public:
	static float getCurrentTime();

	GLQuery();

	void			genQueries();

	void			swapQueryBuffers();

	void			begin();
	void			end();

	//Milliseconds
	math::uint64	time();

	void			deleteQuery();

private:
	unsigned int queryID_[2][1];
	unsigned int queryBackBuffer_;
	unsigned int queryFrontBuffer_;
	bool		first_ = true;
};
#endif
