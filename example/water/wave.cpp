#include "wave.h"

#define TWOPI 6.28318530718f

Wave::Wave() :
	amplitude_(1.0f), 
	steepness_(0.5f),
	length_(10.0f), 
	speed_(1.0f), 
	direction_(1.0f, 0.0f),
	directionRadians_(0.0f), 
	frequency_(TWOPI / length_),
	phaseConstant_(speed_ * frequency_),
	active_(true)
{

}

Wave::Wave(const std::string & name, float amplitude, float steepness, 
	float length, float speed, const V2f & direction):
	name_(name),
	amplitude_(amplitude),
	steepness_(steepness),
	length_(length),
	speed_(speed),
	direction_(direction),
	directionRadians_(0.0f),
	frequency_(TWOPI / length_),
	phaseConstant_(speed_ * frequency_),
    active_(true)
{

}

std::pair<V3f,V3f> Wave::updateGerstner(float x, float y, float t)const
{
	std::pair<V3f, V3f> offset_position_normal;
	V2f grid_position(x, y);

	// finding repeated values
	float dot_result = dot(direction_, grid_position);
	float trig_eval = frequency_ * dot_result + phaseConstant_ * t;
	float sin_result = sin(trig_eval);
	float cos_result = cos(trig_eval);
	float horizontal_product = steepness_ * amplitude_ * cos_result;

	// calculation offsets
	offset_position_normal.first.x = horizontal_product * direction_.x;
	offset_position_normal.first.y = amplitude_ * sin_result;
	offset_position_normal.first.z = horizontal_product * direction_.y;

	// removing repeated multiplication
	float freq_amp = frequency_ * amplitude_;

	// calculating vertex normals
	offset_position_normal.second.x = direction_.x * freq_amp * cos_result;
	offset_position_normal.second.y = steepness_ * freq_amp * sin_result;
	offset_position_normal.second.z = direction_.y * freq_amp * cos_result;

	return offset_position_normal;
}

void Wave::setWaveLength(float l)
{
	length_ = l;
}

void Wave::setSpeed(float s)
{
	speed_ = s;
}

void Wave::setWaveDirection(float d)
{
	directionRadians_ = d;
}
