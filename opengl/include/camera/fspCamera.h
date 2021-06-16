//
//  camera.h
//  OpenGL
//
//  Created by xt on 20/05/14.
//  Copyright (c) 2020  xt. All rights reserved.
//

#ifndef __OpenGL__fsp_camer__
#define __OpenGL__fsp_camer__

#include "cameraBase.h"

class LIBENIGHT_EXPORT FSPCamera :public CameraBase
{
public:
	FSPCamera();
	virtual ~FSPCamera();

	virtual Matrixf getViewMatrix() const override;

	virtual Matrixf getProjectionMatrix() const override;
	virtual void				update()override;


	virtual void positionCamera(const V3f&pos, float yaw, float patch, const V3f&globUP);

	virtual void positionCamera(float positionX, float positionY, float positionZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) override;

	virtual void setEyePos(const V3f &) override;

	virtual V3f getPosition() const override;

	virtual V3f getViewDir()const override;

	virtual void processKeyboard(int key, int st, int action, int mods, float deltaTime) override;

	virtual void mouse_move(const V2f & pt, const float & z_scale) override;

	virtual void mouse_down(int button, const V2f & pt, int state) override;

	virtual void mouse_up(int button, const V2f & pt, int state) override;

	virtual void mouse_scroll(double xoffset, double yoffset) override;

	V3f			getUpDir()const;
	V3f			getRightDir()const;

private:
	void		walk(float d);
	void        strafe(float d);
	void		pitch(float angle);
	void		rotateY(float angle);
	void		updateViewMatrix();

	void		upPos(float);

	void		updateViewDir();
	void		updateStrafeDir();
private:
	V3f			position_;
	V3f			viewDir_;
	V3f		    vStrafe_;
	V3f			globUP_;


	float		yaw_;
	float		pitch_;
	bool		mouse_down_;
	Matrixf		view_;

	float		walk_speed = 1.0;
};

#endif
