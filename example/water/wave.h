
//
//  Wave.h
//  openGLTest
//
//  Created by xt on 19/3/14.
//  Copyright © 2019 xt. All rights reserved.
//
#ifndef __OpenGL__Wave__
#define __OpenGL__Wave__

#include <string>
#include "type.h"

using namespace math;
class Wave
{
public:
	Wave();

	Wave(const std::string & name, float amplitude, float steepness,
		float length, float speed, const V2f & direction);
	//update the vertex by Gerstner equation
	std::pair<V3f, V3f>	updateGerstner(float x,float y,float t)const;

	void			setWaveLength(float l);
	void			setSpeed(float s);
	void			setWaveDirection(float d);
//private:
	std::string		name_; // wave's name
	float			length_;//! The wave's lenght.
	float			speed_;//! The wave's speed.
	float			directionRadians_;//! The direction of the wave in radians.
	float			frequency_;//! The frequency of the wave.
	float			phaseConstant_;//! The wave phase constant.
	float			amplitude_;//! The amplitude of the wave.
	float			steepness_;//! The steepness of the wave (a.k.a. peak sharpness)
	V2f				direction_;//! The direction the wave travels in.
	
	bool			active_;

	static unsigned num_wavesCreated_;//! The number of waves that have been created.

};

#endif
