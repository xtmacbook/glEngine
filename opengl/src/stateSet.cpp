
#include "stateSet.h"
#include "gls.h"

SampleObject::SampleObject():sampler_state_(0)
{
	initGl();
}

SampleObject::~SampleObject()
{
	if (sampler_state_)
	{
		glDeleteSamplers(1, &sampler_state_);
	}
}

void SampleObject::sampleParameteri(unsigned int pname, unsigned int val)
{
	glSamplerParameteri(sampler_state_, pname, val);
}

void SampleObject::bindTexture(unsigned int texture)
{
	glBindSampler(texture, sampler_state_);
}

bool SampleObject::initGl()
{
	glGenSamplers(1, &sampler_state_);

	return true;
}
