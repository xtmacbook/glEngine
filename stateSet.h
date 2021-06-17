
#ifndef __OpenGL__StateSet__
#define __OpenGL__StateSet__

#include "BaseObject.h"
#include "smartPointer.h"

class LIBENIGHT_EXPORT SampleObject :public base::BaseObject
{
public:

	SampleObject();

	virtual ~SampleObject();

	void sampleParameteri(unsigned int pname, unsigned int val);

	void bindTexture(unsigned int texture);

private:
	bool initGl();
	unsigned int sampler_state_;
};

#endif