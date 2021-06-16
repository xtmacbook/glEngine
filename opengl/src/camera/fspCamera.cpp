#include "fspCamera.h"
#include "helpF.h"
#include "glu.h"
#include "util.h"
#include "Inputmanager.h"

using namespace math;


FSPCamera::FSPCamera():mouse_down_(false),
	yaw_(0.0f),
	pitch_(0.0f)
{

}

FSPCamera::~FSPCamera()
{

}

Matrixf FSPCamera::getViewMatrix() const
{
	return view_;
}

Matrixf FSPCamera::getProjectionMatrix() const
{
	return perspective(_fov, _screen_ratio, _near_z, _far_z);
}

void FSPCamera::update()
{

	// Normalize the strafe vector

	if (InputManager::instance()->getInputKey(GLU_KEY_W) == GLU_PRESS)
	{
		walk(walk_speed);
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_S) == GLU_PRESS)
	{
		walk(-walk_speed);
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_A) == GLU_PRESS)
	{
		strafe(-walk_speed);
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_D) == GLU_PRESS)
	{
		strafe(walk_speed);
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_LEFT) == GLU_PRESS )
	{
		rotateY(math::degreeToRadin(5.0f));
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_RIGHT) == GLU_PRESS)
	{
		rotateY(math::degreeToRadin(-5.0f));
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_UP) == GLU_PRESS)
	{
		pitch(math::degreeToRadin(-5.0f));
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_DOWN) == GLU_PRESS)
	{
		pitch(math::degreeToRadin(5.0f));
	}

	if (InputManager::instance()->getInputKey(GLU_KEY_PAGE_UP) == GLU_PRESS)
	{
		upPos(walk_speed);
	}
	if (InputManager::instance()->getInputKey(GLU_KEY_PAGE_DOWN) == GLU_PRESS)
	{
		upPos(-walk_speed);
	}
}

void FSPCamera::positionCamera(float positionX, float positionY, float positionZ, float centerX, 
	float centerY, float centerZ, float upX, float upY, float upZ)
{
	const float DEG2RAD = acos(-1) / 180.0f;

	position_ = V3f(positionX, positionY, positionZ);
	viewDir_ = V3f(centerX - positionX, centerY - positionY, centerZ - positionZ);

	V3f nvd = math::normalize(viewDir_);
	pitch_ = asin(nvd.y);
	yaw_ = -acos(nvd.x / (cos(pitch_)));  // PI/180; //Beware cos(pitch)==0, catch this exception!

	pitch_ = 0.0;
	yaw_ = degreeToRadin(90.0);

	float pg = radinToDegree(pitch_);
	float yg = radinToDegree(yaw_);
	//float degree = yaw_ / DEG2RAD;

	updateViewMatrix();

	initPos_ = true;
}

void FSPCamera::positionCamera(const V3f&pos, float yaw, float patch, const V3f&globUP)
{
	position_ = pos;
	yaw_ = degreeToRadin(yaw);
	pitch_ = degreeToRadin(patch);
	globUP_ = globUP;

	updateViewDir();
	updateStrafeDir();
	updateViewMatrix();

	initPos_ = true;
}

void FSPCamera::setEyePos(const V3f &eye)
{
	position_ = eye;
	updateViewMatrix();
}

V3f FSPCamera::getPosition() const
{
	return position_;
}

math::V3f FSPCamera::getViewDir() const
{
	return math::normalize(viewDir_);
}

void FSPCamera::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_KP_ADD)
		{
			walk_speed += 0.1;
		}
		if (key == GLU_KEY_KP_SUBTRACT)
		{
			walk_speed -= 0.1;
			if (walk_speed < 0) walk_speed = 0.01;
		}
	}
}

void FSPCamera::mouse_move(const V2f & pt, const float & z_scale)
{
	if (!mouse_down_) return;
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = _screen_width >> 1;				// This is a binary shift to get half the width
	int middleY = _screen_height >> 1;				// This is a binary shift to get half the height
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
	static float currentRotX = 0.0f;

	GetCursorPos(&mousePos);

	if ((mousePos.x == middleX) && (mousePos.y == middleY)) return;

	SetCursorPos(middleX, middleY);

	angleY = (float)((middleX - mousePos.x)) / 500.0f;
	angleZ = (float)((middleY - mousePos.y)) / 500.0f;

	static float lastRotX = 0.0f;
	lastRotX = currentRotX; 
	currentRotX += angleZ;

	if (currentRotX > 1.0f)
	{
		currentRotX = 1.0f;

		if (lastRotX != 1.0f)
		{
			 
		}
	}
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
	else if (currentRotX < -1.0f)
	{
		currentRotX = -1.0f;

		if (lastRotX != -1.0f)
		{
			 
		}
	}
	else
	{
		 
	}

	 
}

void FSPCamera::mouse_down(int button, const V2f & pt, int state)
{
	if (button == GLU_MOUSE_BUTTON_LEFT)
		mouse_down_ = true;
}

void FSPCamera::mouse_up(int button, const V2f & pt, int state)
{
	if (button == GLU_MOUSE_BUTTON_LEFT)
		mouse_down_ = false;
}

void FSPCamera::mouse_scroll(double xoffset, double yoffset)
{
}

void FSPCamera::walk(float speed)
{
	// Get the current view vector (the direction we are looking)
	const V3f& viewDir = getViewDir();
	position_.x += viewDir.x * speed;
	position_.y += viewDir.y * speed;
	position_.z += viewDir.z * speed;

	updateViewMatrix();
}

void FSPCamera::strafe(float speed)
{
	position_ += vStrafe_ * speed;
	updateViewMatrix();
}

void FSPCamera::pitch(float angle)
{
	angle = -angle;

	const float limit = 89.0 * PI_180;

	this->pitch_ += angle;

	if (this->pitch_ < -limit)
	{
		this->pitch_ = -limit;
	}

	if (this->pitch_ > limit)
	{
		this->pitch_ = limit;
	}
	updateViewDir();
	updateStrafeDir();
	updateViewMatrix();
}


void FSPCamera::rotateY(float angle)
{
	yaw_ += angle;
	updateViewDir();
	updateStrafeDir();
	updateViewMatrix();
}

void FSPCamera::updateViewMatrix()
{
	V3f right = getRightDir();// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	V3f up = normalize(crossVec3(right, viewDir_));
	view_ = lookAt(position_, position_ + viewDir_, up);
}

void FSPCamera::upPos(float speed)
{
	const V3f& upDir = getUpDir();
	position_.x += upDir.x * speed;
	position_.y += upDir.y * speed;
	position_.z += upDir.z * speed;

	updateViewMatrix();
}

void FSPCamera::updateViewDir()
{
	/*Matrixf yaw(1.0);
	Matrixf pitch(1.0);

	yaw = rotateR(yaw, yaw_, V3f(0.0, 1.0, 0.0));
	pitch = rotateR(pitch, pitch_, V3f(1.0, 0.0, 0.0));

	Matrixf rm = pitch * yaw;

	rm = inverse(rm);

	viewDir_.x = -rm[2][0];
	viewDir_.y = -rm[2][1];
	viewDir_.z = -rm[2][2];
*/
	
	viewDir_.x = -sin(this->yaw_) * cos(this->pitch_);
	viewDir_.y = sin(this->pitch_);
	viewDir_.z = -cos(this->yaw_) * cos(this->pitch_);
	
	normalizeVec3(viewDir_);
}

void FSPCamera::updateStrafeDir()
{
	vStrafe_ = getRightDir();
}

math::V3f FSPCamera::getUpDir() const
{
	V3f right = getRightDir();// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	return normalize(crossVec3(right, viewDir_));
}

math::V3f FSPCamera::getRightDir() const
{
	return normalize(crossVec3(viewDir_, globUP_));
}
