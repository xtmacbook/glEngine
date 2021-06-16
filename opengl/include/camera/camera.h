//
//  camera.h
//  OpenGL
//
//  Created by xt on 15/8/9.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__camer__
#define __OpenGL__camer__

#include "boundingBox.h"
#include "cameraBase.h"

#include <stdio.h>

using namespace math;
using namespace base;

class Scene;

class LIBENIGHT_EXPORT Camera :public CameraBase
{
public:
	
	typedef enum 
	{
		CAMERA				= 0x00000001,
		OBJECT				= 0x00000002,
		CAMERA_THIRD_PERSON = 0x00000004,

		ZOOM			    = 0x00000010,
		PAN					= 0x00000020,
		ROTATE				= 0x00000040,
		
		ALL					= 0x00000070,
		DONT_TRANSLATE_FOCUS= 0x00000100,
		PAN_RELATIVE		= 0x00000200,

	}Behavior;

	typedef enum 
	{
		NIL_MODE,
		TUMBLE_MODE,
		DOLLY_MODE,
		PAN_MODE 
	} Mode;

	Camera(int behavior = CAMERA | ALL | PAN_RELATIVE);

	virtual ~Camera();

	virtual Matrixf				getViewMatrix()const;
	virtual Matrixf				getProjectionMatrix()const;
	virtual void				update()override;
	
	virtual void                positionCamera(float positionX, float positionY, float positionZ,
									float centerX, float centerY, float centerZ,
									float upX, float upY, float upZ);

	virtual void				setEyePos(const V3f&);
	void						setFousePos(const V3f&);
	void						setBehavior(int flag);
	void						setObject(const Matrixf& m);

	virtual V3f					getPosition()const;
	virtual V3f					getViewDir()const override;

	virtual void				processKeyboard(int key, int st, int action, int mods, float deltaTime);
	virtual void				mouse_move(const V2f & pt, const float & z_scale);
	virtual void				mouse_down(int button,const V2f& pt, int state);
	virtual void				mouse_up(int button,const V2f& pt, int state);
	virtual void				mouse_scroll(double xoffset,double yoffset);

    bool						isAABBVisible_E(const AABB&)const;
protected:
	virtual void				mouse_move_tumble(const V2f&pt);
	virtual void				mouse_move_plan(const V2f&pt);
	virtual void				mouse_move_dolly(const V2f&pt);
private:

protected:
	Mode						mode_;
	long						behavior_;

	glm::quat					track_quat_;
	glm::quat					curquat_;

	V2f							mouse_start_;

	V3f							eye_pos_;
	V3f							focus_pos_;
	V3f							world_up_;

	V3f							prev_eye_pos_;
	V3f							prev_focus_pos_;


	Matrixf						viewMatrix_;
	Matrixf						old_viewMatrix_;
	
	Matrixf						obj_local_;

};

#endif
