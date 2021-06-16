#include "cameraBase.h"
#include "helpF.h"
#include "util.h"
#include "occlusion.h"

#define FRUSTUM_RIGHT  0
#define FRUSTUM_LEFT   1
#define FRUSTUM_BOTTOM 2
#define FRUSTUM_TOP    3
#define FRUSTUM_FAR    4
#define FRUSTUM_NEAR   5

CameraBase::CameraBase():
	_fov(80.0),
	_screen_ratio(1.0),
	_screen_width(-1),
	_screen_height(-1)
{

}

CameraBase::~CameraBase()
{

}

void CameraBase::calculateViewFrustum(void)
{
	float clip[16];
	float fNorm;

	Matrixf viewProj = getProjectionMatrix() * getViewMatrix();
	clip[0] = viewProj[0][0];
	clip[1] = viewProj[0][1];
	clip[2] = viewProj[0][2];
	clip[3] = viewProj[0][3];

	clip[4] = viewProj[1][0];
	clip[5] = viewProj[1][1];
	clip[6] = viewProj[1][2];
	clip[7] = viewProj[1][3];

	clip[8] = viewProj[2][0];
	clip[9] = viewProj[2][1];
	clip[10] = viewProj[2][2];
	clip[11] = viewProj[2][3];

	clip[12] = viewProj[3][0];
	clip[13] = viewProj[3][1];
	clip[14] = viewProj[3][2];
	clip[15] = viewProj[3][3];

	/* Extract the numbers for the RIGHT plane */
	m_viewFrustum[FRUSTUM_RIGHT][0] = clip[3] - clip[0];
	m_viewFrustum[FRUSTUM_RIGHT][1] = clip[7] - clip[4];
	m_viewFrustum[FRUSTUM_RIGHT][2] = clip[11] - clip[8];
	m_viewFrustum[FRUSTUM_RIGHT][3] = clip[15] - clip[12];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_RIGHT][0]) +
		SQR(m_viewFrustum[FRUSTUM_RIGHT][1]) +
		SQR(m_viewFrustum[FRUSTUM_RIGHT][2]));
	m_viewFrustum[FRUSTUM_RIGHT][0] /= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][1] /= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][2] /= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][3] /= fNorm;

	/* Extract the numbers for the LEFT plane */
	m_viewFrustum[FRUSTUM_LEFT][0] = clip[3] + clip[0];
	m_viewFrustum[FRUSTUM_LEFT][1] = clip[7] + clip[4];
	m_viewFrustum[FRUSTUM_LEFT][2] = clip[11] + clip[8];
	m_viewFrustum[FRUSTUM_LEFT][3] = clip[15] + clip[12];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_LEFT][0]) +
		SQR(m_viewFrustum[FRUSTUM_LEFT][1]) +
		SQR(m_viewFrustum[FRUSTUM_LEFT][2]));
	m_viewFrustum[FRUSTUM_LEFT][0] /= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][1] /= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][2] /= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][3] /= fNorm;

	/* Extract the BOTTOM plane */
	m_viewFrustum[FRUSTUM_BOTTOM][0] = clip[3] + clip[1];
	m_viewFrustum[FRUSTUM_BOTTOM][1] = clip[7] + clip[5];
	m_viewFrustum[FRUSTUM_BOTTOM][2] = clip[11] + clip[9];
	m_viewFrustum[FRUSTUM_BOTTOM][3] = clip[15] + clip[13];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_BOTTOM][0]) +
		SQR(m_viewFrustum[FRUSTUM_BOTTOM][1]) +
		SQR(m_viewFrustum[FRUSTUM_BOTTOM][2]));
	m_viewFrustum[FRUSTUM_BOTTOM][0] /= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][1] /= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][2] /= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][3] /= fNorm;

	/* Extract the TOP plane */
	m_viewFrustum[FRUSTUM_TOP][0] = clip[3] - clip[1];
	m_viewFrustum[FRUSTUM_TOP][1] = clip[7] - clip[5];
	m_viewFrustum[FRUSTUM_TOP][2] = clip[11] - clip[9];
	m_viewFrustum[FRUSTUM_TOP][3] = clip[15] - clip[13];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_TOP][0]) +
		SQR(m_viewFrustum[FRUSTUM_TOP][1]) +
		SQR(m_viewFrustum[FRUSTUM_TOP][2]));
	m_viewFrustum[FRUSTUM_TOP][0] /= fNorm;
	m_viewFrustum[FRUSTUM_TOP][1] /= fNorm;
	m_viewFrustum[FRUSTUM_TOP][2] /= fNorm;
	m_viewFrustum[FRUSTUM_TOP][3] /= fNorm;

	/* Extract the FAR plane */
	m_viewFrustum[FRUSTUM_FAR][0] = clip[3] - clip[2];
	m_viewFrustum[FRUSTUM_FAR][1] = clip[7] - clip[6];
	m_viewFrustum[FRUSTUM_FAR][2] = clip[11] - clip[10];
	m_viewFrustum[FRUSTUM_FAR][3] = clip[15] - clip[14];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_FAR][0]) +
		SQR(m_viewFrustum[FRUSTUM_FAR][1]) +
		SQR(m_viewFrustum[FRUSTUM_FAR][2]));
	m_viewFrustum[FRUSTUM_FAR][0] /= fNorm;
	m_viewFrustum[FRUSTUM_FAR][1] /= fNorm;
	m_viewFrustum[FRUSTUM_FAR][2] /= fNorm;
	m_viewFrustum[FRUSTUM_FAR][3] /= fNorm;

	/* Extract the NEAR plane */
	m_viewFrustum[FRUSTUM_NEAR][0] = clip[3] + clip[2];
	m_viewFrustum[FRUSTUM_NEAR][1] = clip[7] + clip[6];
	m_viewFrustum[FRUSTUM_NEAR][2] = clip[11] + clip[10];
	m_viewFrustum[FRUSTUM_NEAR][3] = clip[15] + clip[14];

	//normalize the plane
	fNorm = sqrtf(SQR(m_viewFrustum[FRUSTUM_NEAR][0]) +
		SQR(m_viewFrustum[FRUSTUM_NEAR][1]) +
		SQR(m_viewFrustum[FRUSTUM_NEAR][2]));
	m_viewFrustum[FRUSTUM_NEAR][0] /= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][1] /= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][2] /= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][3] /= fNorm;
}

bool CameraBase::cubeFrustumTest(float x, float y, float z, float size)const
{
	int i;

	//loop through and test the six vertices of the bounding box against the viewing frustum
	for (i = 0; i < 6; i++)
	{
		if (m_viewFrustum[i][0] * (x - size) + m_viewFrustum[i][1] *
			(y - size) + m_viewFrustum[i][2] *
			(z - size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x + size) + m_viewFrustum[i][1] *
			(y - size) + m_viewFrustum[i][2] *
			(z - size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x - size) + m_viewFrustum[i][1] *
			(y + size) + m_viewFrustum[i][2] *
			(z - size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x + size) + m_viewFrustum[i][1] *
			(y + size) + m_viewFrustum[i][2] *
			(z - size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x - size) + m_viewFrustum[i][1] *
			(y - size) + m_viewFrustum[i][2] *
			(z + size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x + size) + m_viewFrustum[i][1] *
			(y - size) + m_viewFrustum[i][2] *
			(z + size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x - size) + m_viewFrustum[i][1] *
			(y + size) + m_viewFrustum[i][2] *
			(z + size) + m_viewFrustum[i][3] > 0)
			continue;
		if (m_viewFrustum[i][0] * (x + size) + m_viewFrustum[i][1] *
			(y + size) + m_viewFrustum[i][2] *
			(z + size) + m_viewFrustum[i][3] > 0)
			continue;

		return false;
	}

	return true;
}


CameraBase::FRUSTUMINBOXSTATE CameraBase::aabbFrustumTest(const V3f&min, const V3f&max) const
{
	FRUSTUMINBOXSTATE result = INSIDE;
	int i;
	for (i = 0; i < 6; i++)
	{
		V3f PN[2];
		math::getPosAndNegPointProjPlane(m_viewFrustum[i], min, max, PN);
		// is the positive vertex outside?
		if (m_viewFrustum[i][0] * PN[0].x +
			m_viewFrustum[i][1] * PN[0].y + 
			m_viewFrustum[i][2] * PN[0].z + 
			m_viewFrustum[i][3] < 0)
			return OUTSIDE;

		// is the negative vertex outside?	
		if (m_viewFrustum[i][0] * PN[1].x + 
			m_viewFrustum[i][1] * PN[1].y + 
			m_viewFrustum[i][2] * PN[1].z + 
			m_viewFrustum[i][3] < 0)
			result = INTERSECT;

	}
	return result;
}

void CameraBase::setFov(float fov)
{
	_fov = fov;
}

void CameraBase::setClipPlane(const float nearr, const float farr)
{
	_near_z = nearr;
	_far_z = farr;
}

void CameraBase::setWindowSize(float width, float heigh)
{
	_screen_width = width;
	_screen_height = heigh;
	_screen_ratio = (double)width / (double)heigh;
}

void CameraBase::getViewFrustum(float *l, float *r, float *t, float * b, float*n, float*f)const
{
	*n = _near_z;
	*f = _far_z;

	*t = _near_z * tanf(math::degreeToRadin(_fov * 0.5f));
	*b = -*t;

	*l = *t * _screen_ratio;
	*r = -*l;
}
