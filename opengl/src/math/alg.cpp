#include "alg.h"

namespace math
{
	float  bezier_basis_matrix[]=
	{
		-1.0f, 3.0f, -3.0f ,1.0f,
		3.0f,-6.0f, 3.0f, 0.0f,
		-3.0f, 3.0f, 0.0f, 0.0f,
		1.0f, 0.0f ,0.0f ,0.0f
	};
	V3f evalBezierCurve(const V3f *P, const float &t)
	{
		return evalBezierCurve(P[0], P[1], P[2], P[3], t);
	}
	V3f evalBezierCurve(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t)
	{
		float b0 = (1 - t) * (1 - t) * (1 - t);
		float b1 = 3 * t * (1 - t) * (1 - t);
		float b2 = 3 * t * t * (1 - t);
		float b3 = t * t * t;

		return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
	}

	V3f derivBezier(const V3f *P, const float &t)
	{
		return derivBezier(P[0], P[1], P[2], P[3],t);
	}

	V3f  derivBezier(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t)
	{
		return -3 * (1 - t) * (1 - t) * p0 +
			(3 * (1 - t) * (1 - t) - 6 * t * (1 - t)) * p1 +
			(6 * t * (1 - t) - 3 * t * t) * p2 +
			3 * t * t * p3;
	}

	V3f dVBezier(const V3f *controlPoints, const float &u, const float &v)
	{
		V3f uCurve[4];
		for (int i = 0; i < 4; ++i) {
			uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);
		}

		return derivBezier(uCurve, v);
	}

	V3f dUBezier(const V3f *controlPoints, const float &u, const float &v)
	{
		V3f P[4];
		V3f vCurve[4];
		for (int i = 0; i < 4; ++i) {
			P[0] = controlPoints[i];
			P[1] = controlPoints[4 + i];
			P[2] = controlPoints[8 + i];
			P[3] = controlPoints[12 + i];
			vCurve[i] = evalBezierCurve(P, v);
		}

		return derivBezier(vCurve, u);
	}

	V3f decasteljau(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t)
	{
		V3f P12 = (1 - t) * p0 + t * p1;
		V3f P23 = (1 - t) * p1 + t * p2;
		V3f P34 = (1 - t) * p2 + t * p3;
		// compute two points along segments P1P2-P2P3 and P2P3-P3P4
		V3f P1223 = (1 - t) * P12 + t * P23;
		V3f P2334 = (1 - t) * P23 + t * P34;

		// finally compute P
		return (1 - t) * P1223 + t * P2334; // P
	}

	V3f decasteljau(const V3f *P, const float &t)
	{
		return decasteljau(P[0], P[1], P[2], P[3], t);
	}

}
