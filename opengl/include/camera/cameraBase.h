
#ifndef _camera_base_h_
#define _camera_base_h_

#include "decl.h"
#include "type.h"
#include "BaseObject.h"
#include "smartPointer.h"
using namespace math;

class LIBENIGHT_EXPORT CameraBase :public base::BaseObject
{
public:

	enum FRUSTUMINBOXSTATE
	{
		INSIDE,
		OUTSIDE,
		INTERSECT
	};

	CameraBase();
	virtual ~CameraBase();

	virtual Matrixf				getViewMatrix()const = 0;
	virtual Matrixf				getProjectionMatrix()const = 0;
	virtual void				update() = 0;
	virtual void                positionCamera(float positionX, float positionY, float positionZ,
								float centerX, float centerY, float centerZ,
								float upX, float upY, float upZ) = 0;

	virtual void				setEyePos(const V3f&) = 0;
	virtual V3f					getPosition()const = 0;
	virtual V3f					getViewDir()const = 0;
	virtual void				processKeyboard(int key, int st, int action, int mods, float deltaTime) = 0;
	virtual void				mouse_move(const V2f & pt, const float & z_scale) = 0;
	virtual void				mouse_down(int button, const V2f& pt, int state) = 0;
	virtual void				mouse_up(int button, const V2f& pt, int state) = 0;
	virtual void				mouse_scroll(double xoffset, double yoffset) = 0;

	void						calculateViewFrustum(void);
	bool						cubeFrustumTest(float x, float y, float z, float size)const;
	FRUSTUMINBOXSTATE			aabbFrustumTest(const V3f&min,const V3f&max)const;

	void						setFov(float fov);
	void						setClipPlane(const float near, const float far);
	void						setWindowSize(float width, float heigh);

	void						getViewFrustum(float *l, float *r, float *t, float * b, float*n, float*f)const;

	bool						initPos_ = false;
protected:
	float						m_viewFrustum[6][4];
	float						_near_z;
	float						_far_z;
	float						_fov;
	unsigned int				_screen_width;
	unsigned int				_screen_height;
	float						_screen_ratio;

};

#endif
