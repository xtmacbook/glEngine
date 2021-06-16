//
//  camer.cpp
//  OpenGL
//
//  Created by xt on 15/8/9.

//  Copyright (c) 2015ƒÍ xt. All rights reserved.


#include "helpF.h"
#include "camera.h"
#include "windowManager.h"
#include "scene.h"
#include "gls.h"
#include "glu.h"
using namespace math;

#include <iostream>

Camera::Camera(int behavior):
	mode_(NIL_MODE),
	behavior_(behavior),
	
	track_quat_(Quatf(0.0,0.0,0.0,1.0)),
	curquat_(Quatf(0.0,0.0,0.0,1.0)),
	
	world_up_(V3f(0.0,1.0,0.0)), 
    eye_pos_(V3f(0.0,0.0,0.0)),
    prev_eye_pos_(V3f(0.0,0.0,0.0)),
    focus_pos_(V3f(0.0,0.0,0.0)),

    viewMatrix_(Matrixf(1.0)),
    old_viewMatrix_(Matrixf(1.0)),
   
	obj_local_(Matrixf(1.0)),
	mouse_start_(V2f(0.0, 0.0))
{

}

Camera::~Camera()
{

}

Matrixf Camera::getViewMatrix() const
{
	return viewMatrix_;
}

Matrixf Camera::getProjectionMatrix() const
{
	return perspective(_fov, _screen_ratio, _near_z, _far_z);
}


void Camera::update()
{

}

void Camera::setBehavior(int flag)
{
    behavior_ = flag;
}


void Camera::setObject(const Matrixf& m)
{
	obj_local_ = m;
}

void Camera::positionCamera(float positionX, float positionY, float positionZ, float centerX, 
	float centerY, float centerZ, float upX, float upY, float upZ)
{
	eye_pos_ = V3f(positionX, positionY, positionZ);
	focus_pos_ = V3f(centerX, centerY, centerZ);
	world_up_ = V3f(upX, upY, upZ);
	normalizeVec3(world_up_);

	viewMatrix_ = lookAt(eye_pos_,focus_pos_,world_up_);

	if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
	{
		track_quat_ = Quatf();
		curquat_ = qfm(viewMatrix_);
		curquat_ = normalize(curquat_);
	}
	initPos_ = true;
}

void Camera::setEyePos(const V3f&eye_pos)
{
	eye_pos_ = eye_pos;
	if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
	{
		viewMatrix_ = lookAt(eye_pos_, focus_pos_, V3f(0.0,1.0,0.0));
		track_quat_ = Quatf();
		curquat_ = qfm(viewMatrix_);
		curquat_ = normalize(curquat_);
	}
}

void Camera::setFousePos(const V3f&focus_pos)
{
	focus_pos_ = focus_pos;
	if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
	{
		viewMatrix_ = lookAt( eye_pos_, focus_pos_, V3f(0.0, 0.0, 1.0));
		curquat_ = qfm(viewMatrix_);
		curquat_ = normalize(curquat_);
	}
	track_quat_ = Quatf();
}

V3f Camera::getPosition()const
{
	return eye_pos_;
}

math::V3f Camera::getViewDir() const
{
	V3f tv = focus_pos_ - eye_pos_;
	return math::normalize(tv);
}

void Camera::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_Z)
		{
			(behavior_ & ZOOM) ? behavior_ &= ~ZOOM : behavior_ |= ZOOM;
		}
		else if (key == GLU_KEY_R)
		{
			(behavior_ & ROTATE) ? behavior_ &= ~ROTATE : behavior_ |= ROTATE;
		}
		else if (key == GLU_KEY_P)
		{
			(behavior_ & PAN) ? behavior_ &= ~PAN : behavior_ |= PAN;
		}

		else if (key == GLU_KEY_F)
		{
			(behavior_ & DONT_TRANSLATE_FOCUS) ? behavior_ &= ~DONT_TRANSLATE_FOCUS : behavior_ |= DONT_TRANSLATE_FOCUS;
		}
		else if (key == GLU_KEY_L)
		{
			(behavior_ & PAN_RELATIVE) ? behavior_ &= ~PAN_RELATIVE : behavior_ |= PAN_RELATIVE;
		}

		else if (key == GLU_KEY_W)
		{ 
			V3f dir(viewMatrix_[2][0], viewMatrix_[2][1], viewMatrix_[2][2]);
			eye_pos_ -= dir;
			viewMatrix_ = lookAt(eye_pos_, focus_pos_, world_up_);
		}

		else if (key == GLU_KEY_S)
		{
			V3f dir(viewMatrix_[2][0], viewMatrix_[2][1], viewMatrix_[2][2]);
			eye_pos_ += dir;
			viewMatrix_ = lookAt(eye_pos_, focus_pos_, world_up_);
		}
		else if (key == GLU_KEY_A)
		{
			V3f dir(viewMatrix_[0][0], viewMatrix_[0][1], viewMatrix_[0][2]);
			eye_pos_ -= dir;
			focus_pos_ -= dir;
			viewMatrix_ = lookAt(eye_pos_, focus_pos_, world_up_);
		}
		else if (key == GLU_KEY_D)
		{
			V3f dir(viewMatrix_[0][0], viewMatrix_[0][1], viewMatrix_[0][2]);
			eye_pos_ += dir;
			focus_pos_ += dir;
			viewMatrix_ = lookAt(eye_pos_, focus_pos_, world_up_);
		}
	}
	else if (action == GLU_RELEASE)
	{
	}
}

void Camera::mouse_move(const V2f&  pt, const float & z_scale)
{
	switch (mode_)
	{
	case TUMBLE_MODE:
		mouse_move_tumble(pt);
		break;
	case PAN_MODE:
		mouse_move_plan(pt);
		break;
	case DOLLY_MODE:
		mouse_move_dolly(pt);
		break;
	default:
		break;
	}
	
}

void Camera::mouse_down(int button,const V2f& pt, int state)
{
    old_viewMatrix_ = viewMatrix_;

    mouse_start_.x = pt.x;
    mouse_start_.y = pt.y;

    track_quat_ = curquat_;
    prev_eye_pos_ = eye_pos_;
    prev_focus_pos_ = focus_pos_;

    if(button == GLU_MOUSE_BUTTON_LEFT)
        mode_ = TUMBLE_MODE;
	else if (button == GLU_MOUSE_BUTTON_MIDDLE)
        mode_ = PAN_MODE;
    else if(button == GLU_MOUSE_BUTTON_RIGHT)
        mode_ = DOLLY_MODE;
}

void Camera::mouse_up(int button,const V2f& pt, int state)
{
	old_viewMatrix_ = viewMatrix_;
	mouse_start_.x = pt.x;
	mouse_start_.y = pt.y;
	track_quat_ = curquat_;
	mode_ = NIL_MODE;

}

void Camera::mouse_scroll(double xoffset, double yoffset)
{
}

bool Camera::isAABBVisible_E(const AABB&)const
{
    return  true;
}

void Camera::mouse_move_tumble(const V2f&pt)
{

	V2f pt1((2 * mouse_start_.x - _screen_width) / _screen_width,
				(_screen_height - 2 * mouse_start_.y) / _screen_height);

	V2f pt2((2 * pt.x - _screen_width) / _screen_width,
				(_screen_height - 2 * pt.y) / _screen_height);


	if (behavior_ & CAMERA_THIRD_PERSON)
	{
		pt2.x -= pt1.x;
		pt2.y -= pt1.y;
		pt1.x = pt1.y = 0;
	}

	Quatf tmpquat;

	math::trackball(tmpquat, pt1, pt2, (0.8));

	if (behavior_ & ROTATE)
	{
		Quatf cam_rot, inv_cam_rot;
		
		cam_rot = qfm(obj_local_);
		inv_cam_rot = conj(cam_rot);

		if (behavior_ & CAMERA_THIRD_PERSON) 
		{
			tmpquat = conj(tmpquat);
			curquat_ = track_quat_ * inv_cam_rot * tmpquat * cam_rot;
		}
		else if (behavior_ & CAMERA)
			// Setting the camera in first person is equivalent to having _camera equal to _m in the third person camera formula
			curquat_ = tmpquat * track_quat_;
		else if (behavior_ & OBJECT)
			// In object mode _curquat is the inverse of _curquat in camera mode
			curquat_ = inv_cam_rot * tmpquat * cam_rot * track_quat_;

		if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
		{
			Matrixf M = mfq(curquat_);
			world_up_ = V3f(M[0][1], M[1][1], M[2][1]);
			V3f z(M[0][2], M[1][2], M[2][2]);
			z *= length(prev_eye_pos_ - focus_pos_);
			eye_pos_ = z + focus_pos_;
			viewMatrix_ = lookAt( eye_pos_, focus_pos_, world_up_);
		}
		else if (behavior_ & OBJECT)
		{
			Matrixf tmp = mfq(curquat_);

			viewMatrix_[0][0] = tmp[0][0];
			viewMatrix_[0][1] = tmp[0][1];
			viewMatrix_[0][2] = tmp[0][2];

			viewMatrix_[1][0] = tmp[1][0];
			viewMatrix_[1][1] = tmp[1][1];
			viewMatrix_[1][2] = tmp[1][2];

			viewMatrix_[2][0] = tmp[2][0];
			viewMatrix_[2][1] = tmp[2][1];
			viewMatrix_[2][2] = tmp[2][2];
		}
	}

}

void Camera::mouse_move_plan(const V2f&pt)
{
	V2f pt_delta;
	Matrixf inv_m;
	if (behavior_ & PAN_RELATIVE)
	{
		V2f pt1((2.0 * mouse_start_.x - _screen_width) / _screen_width,
			(_screen_height - 2.0 * mouse_start_.y) / _screen_height);
		V2f pt2((2.0 * pt.x - _screen_width) / _screen_width,
			(_screen_height - 2.0 * pt.y) / _screen_height);
	 
		pt_delta.x = (pt1 - pt2).x * 5;
		pt_delta.y = (pt1 - pt2).y * 5;
	}
	else
	{
		pt_delta.x = pt.x * 2.0 / _screen_width - 1.0;
		pt_delta.y = (_screen_height - pt.y) * 2.0 / _screen_height - 1.0;
	}
	if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
	{
		if (behavior_ & CAMERA_THIRD_PERSON)
		{
			pt_delta.x = -pt_delta.x;
			pt_delta.y = -pt_delta.y;
			inv_m = inverse(obj_local_);
		}
		else
			inv_m = inverse(viewMatrix_);

		float fov2 = (float)-tan(0.5 * PI_180 * _fov) * old_viewMatrix_[3][2];

		V4f in((float)(pt_delta.x * fov2 * _screen_ratio),
			(float)(pt_delta.y * fov2),
			(float)(behavior_ & PAN_RELATIVE ? 0 : old_viewMatrix_[3][2]), 0.0
		);

		V4f offset_ = inv_m * in;
		V3f offset(offset_.x, offset_.y, offset_.z);

		if (!(behavior_ & PAN_RELATIVE))
		{
			offset = offset + prev_eye_pos_;
			offset *= -1.0;
		}

		eye_pos_ = prev_eye_pos_ + offset;
		focus_pos_ = prev_focus_pos_ + offset;
		viewMatrix_ = lookAt(eye_pos_, focus_pos_, world_up_);
	}
	else if (behavior_ & OBJECT)
	{
		V3f obj_pos = V3f(old_viewMatrix_[3][0], old_viewMatrix_[3][1], old_viewMatrix_[3][2]);
		V4f eye_obj_pos = obj_local_ * V4f(obj_pos.x, obj_pos.y, obj_pos.z, 1.0);

		float fov2 = (float)(-tan(0.5 * PI_180 * _fov) * eye_obj_pos.z);

		V4f shift(
			(float)(pt_delta.x * fov2 * _screen_ratio),
			(float)(pt_delta.y * fov2),
			(float)(behavior_ & PAN_RELATIVE ? 0 : obj_pos.z),
			0.0
		);

		inv_m  = inverse(obj_local_);
		V4f o_ = inv_m * shift;
		V3f offset = V3f(o_.x, o_.y, o_.z);

		if (behavior_ & PAN_RELATIVE)
		{
			viewMatrix_[3][0] = old_viewMatrix_[3][0] + offset.x;
			viewMatrix_[3][1] = old_viewMatrix_[3][1] + offset.y;
			viewMatrix_[3][2] = old_viewMatrix_[3][2] + offset.z;
		}
		else
		{
			viewMatrix_[3][0] = offset.x;
			viewMatrix_[3][1] = offset.y;
			viewMatrix_[3][2] = offset.z;
		}
	}
}

void Camera::mouse_move_dolly(const V2f&pt)
{
	float z_delta = 1.0 * ((pt.y - mouse_start_.y) / _screen_height * (_far_z - _near_z));
    z_delta *= 1.0;

	if (behavior_ & ZOOM)
	{
		if (behavior_ & (CAMERA | CAMERA_THIRD_PERSON))
		{
			V3f norm_z   = normalize(prev_eye_pos_ - prev_focus_pos_);

			V3f z_offset(norm_z);
			z_offset *= z_delta;

			eye_pos_ = prev_eye_pos_ + z_offset;

			if (!(behavior_ & DONT_TRANSLATE_FOCUS))
				focus_pos_ = prev_focus_pos_ + z_offset;
			viewMatrix_ = lookAt( eye_pos_, focus_pos_, world_up_);
			if (behavior_ & DONT_TRANSLATE_FOCUS)
				curquat_ = qfm(viewMatrix_);
		}
		else if (behavior_ & OBJECT)
		{
			V4f obj_pos_ = V4f(old_viewMatrix_[3][0], old_viewMatrix_[3][1], old_viewMatrix_[3][2],1.0);
			V4f eye_obj_pos_;
			Matrixf invcam;

			invcam = inverse(obj_local_);
			eye_obj_pos_ = obj_local_ * obj_pos_;
			eye_obj_pos_.z += z_delta * 0.5;

			obj_pos_ =  invcam * eye_obj_pos_;

			viewMatrix_[3][0] = obj_pos_.x;
			viewMatrix_[3][1] = obj_pos_.y;
			viewMatrix_[3][2] = obj_pos_.z;
		}
	}
}
