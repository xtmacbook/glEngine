//
//  Header.h
//  openGLTest
//
//  Created by xt on 17/8/28.
//  Copyright © 2017年 xt. All rights reserved.
//

#ifndef OPENGL_M_HEAD_H
#define OPENGL_M_HEAD_H

#include "type.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/quaternion.hpp"

namespace math
{
	template<class REAL>
	struct  Const
	{
		typedef REAL Real;
		typedef const REAL cReal;

		static inline const REAL pi() throw() { return REAL(3.141592653589793238462643383279502884197169399375105820974944592); }
		static inline const REAL tpi() throw() { return REAL(6.28318530718f); }
		static inline const REAL pi_2() throw() { return pi() / REAL(2.0); }
		static inline const REAL pi_180() throw() { return pi() / REAL(180.0); }
		static inline const REAL c180_pi() throw() { return REAL(180.0) / pi(); }
		static inline const REAL infinity() throw() { return std::numeric_limits<REAL>::infinity(); }
		static inline const REAL epsilon() throw() { return std::numeric_limits<REAL>::epsilon(); }
		static inline const REAL near_epsilon() throw() { return REAL(10e-5); }
		static inline const REAL zero() throw() { return REAL(0.0); }
		static inline const REAL max_value() throw() { return std::numeric_limits<REAL>::max(); }
		static inline const REAL min_value() throw() { return std::numeric_limits<REAL>::min(); }
	};
}

namespace math
{
	const double EPSLION = 0.00001;

	template<class REAL>
	math::Matrix4<REAL>						translateR(const math::Matrix4<REAL>&m, const math::Vector3<REAL>&t)
	{
		return glm::translate(m, t);
	}

	template<class REAL>
	math::Matrix4<REAL>						scaleR(const math::Matrix4<REAL>&m, const math::Vector3<REAL>&t)
	{
		return glm::scale(m, t);
	}

	template<class REAL>
	math::Matrix4<REAL>						rotateR(const math::Matrix4<REAL>&m,const REAL& angle, const math::Vector3<REAL>&t)
	{
		return glm::rotate(m,angle, t);
	}

	template<class REAL>
	bool									clipTest(const REAL& p, const REAL& q, REAL& u1, REAL& u2)
	{
		if (p < 0.0)
		{
			const REAL r = q / p;
			if (r > u2)
				return false;
			else
			{
				if (r > u1)
				{
					u1 = r;
				}
				return true;
			}
		}
		else
		{
			if (p > 0.0)
			{
				const REAL r = q / p;
				if (r < u1)
					return false;
				else
				{
					if (r < u2)
					{
						u2 = r;
					}
					return true;
				}
			}
			else
				return q >= 0.0;
		}
	}

	template <class GenType>
	GenType									normalize(GenType const &X)
	{
		return  glm::normalize(X);
	}

	template<typename genType>
	typename genType::value_type const *	value_ptr(genType const& v)
	{
		return glm::value_ptr(v);
	}


	template<class REAL>
	math::Matrix4<REAL>						lookAt(
		math::Vector3<REAL>const& eye, math::Vector3<REAL>const& center, math::Vector3<REAL>const& up)
	{
		return glm::lookAt(eye, center, up);
	}
	template<class REAL>
	math::Matrix4<REAL>						perspective(REAL fovy, REAL aspect, REAL zNear, REAL zFar)
	{
		return glm::perspective(fovy, aspect, zNear, zFar);
	}

	template <class REAL>
	math::Matrix4<REAL>						ortho(REAL left, REAL right, REAL bottom, REAL top, REAL zNear, REAL zFar)
	{
		return	glm::ortho(left, right, bottom, top,zNear, zNear);
	}

	template <class REAL>
	void									normalizeVec3(math::Vector3<REAL>&V)
	{
		REAL len = glm::length(V);
		V /= len;
	}

	template <class REAL>
	REAL									length(math::Vector3<REAL>&V)
	{
		return glm::length(V);
	}

	template <class REAL>
	REAL									length2(math::Vector3<REAL>&V)
	{
		return V.x * V.x + V.y * V.y + V.z + V.z;
	}

	template <class REAL>
	REAL									distance(const math::Vector3<REAL>& p0, const math::Vector3<REAL>& p1)
	{
		return glm::distance(p0,p1);
	}

	template<class REAL>
	math::Matrix4<REAL>						inverse(const math::Matrix4<REAL>&m)
	{
		return glm::inverse(m);
	}

	template<class REAL>
	math::Matrix3<REAL>						inverse(const math::Matrix3<REAL>&m)
	{
		return glm::inverse(m);
	}

	template <class REAL>
	void									inverst(math::Matrix4<REAL>&m)
	{
		math::Matrix4<REAL> t;
		t = inverse(m);
		m = t; 
	}
	template<class REAL>
	math::Matrix3<REAL>						transpose(math::Matrix3<REAL>&m)
	{
		return glm::transpose(m);
	}
	
	template <class REAL>
	math::Matrix4<REAL>						transpose(math::Matrix4<REAL>&m)
	{
		return glm::transpose(m);
	}
	
	template <class REAL>
	REAL									dotVec3(const math::Vector3<REAL>&a, const math::Vector3<REAL>&b)
	{
		return glm::dot(a, b);
	}
	template <class REAL>
	REAL									dotVec(const math::Vector4<REAL>&a, const math::Vector4<REAL>&b)
	{
		return glm::dot(a, b);
	}
	template <class REAL>
	math::Vector3<REAL>						crossVec3(const math::Vector3<REAL>&a, const math::Vector3<REAL>&b)
	{
		return glm::cross(a, b);
	}

	template <class REAL>
	REAL									radinToDegree(const REAL&r)
	{
		REAL returnVal = (r * 180.0) / math::Const<REAL>::pi();
		return returnVal;
	}

	template <class REAL>
	REAL									degreeToRadin(const REAL&d)
	{
		return (d * math::Const<REAL>::pi()) / 180.0;
	}
	template <class REAL>
	math::Vector3<REAL>						mulHomogenPoint(const math::Matrix4<REAL> m, const math::Vector3<REAL>&v)
	{
		math::Vector4<REAL> tm = m * math::Vector4<REAL>(v, 1.0);
		return math::Vector3<REAL>(tm.x / tm.w, tm.y / tm.w, tm.z / tm.w);
	}
	template <class REAL>
	math::Matrix4<REAL>						mfq(const math::Quat<REAL>&q)
	{
		return glm::mat4_cast<REAL>(q);
	}

	template< class REAL>
	math::Quat<REAL>						qfm(const math::Matrix4<REAL>&m)
	{
		return glm::quat_cast(m);
	}

	template< class REAL>
	math::Quat<REAL>						qftv(const math::Vector3<REAL>& v1, const math::Vector3<REAL>& v2)
	{
		return glm::rotation(v1,v2);
	}

	template<class REAL>
	math::Quat<REAL> 						conj(const math::Quat<REAL> & p)
	{
		math::Quat<REAL> r;
		r.x = -p.x;
		r.y = -p.y;
		r.z = -p.z;
		r.w = p.w;
		return r;
	}

	template <class REAL>
	math::Quat<REAL>						inverseQuat(const math::Quat<REAL>&q)
	{
		return  glm::inverse(q);
	}

	inline bool								closeEnough(float f1, float f2)
	{
		return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < 1e-6f;
	}

	template<class REAL>
	REAL									tb_project_to_sphere(REAL r, REAL x, REAL y)
	{
		REAL d, t, z;

		d = sqrtf(x*x + y*y);
		if (d < r * 0.70710678118654752440) {    /* Inside sphere */
			z = sqrtf(r*r - d*d);
		}
		else {           /* On hyperbola */
			t = r / (REAL)1.41421356237309504880;
			z = t*t / d;
		}
		return z;
	}

	template<class REAL>
	math::Quat<REAL> &						axis_to_quat(math::Quat<REAL>& q, const math::Vector3<REAL>& a, const REAL phi)
	{
		math::Vector3<REAL> tmp(a.x, a.y, a.z);

		tmp = normalize(tmp);
		REAL s = sinf(phi / 2.0);
		q.x = s * tmp.x;
		q.y = s * tmp.y;
		q.z = s * tmp.z;
		q.w = cosf(phi / 2.0);
		return q;
	}

	template<class REAL>
	math::Quat<REAL>&						trackball(math::Quat<REAL>& q, math::Vector2<REAL>& pt1, math::Vector2<REAL>& pt2, float trackballsize)
	{
		math::Vector3<REAL> a; // Axis of rotation
		float phi;  // how much to rotate about axis

		math::Vector3<REAL> d;
		float t;

		if ((abs(pt1.x - pt2.x)<EPSLION) && (abs(pt1.y - pt2.y)<EPSLION))
		//if (pt1.x == pt2.x && pt1.y == pt2.y)
		{
			// Zero rotation
			q = math::Quat<REAL>(1.0,0.0,0.0,0.0);
			return q;
		}

		// First, figure out z-coordinates for projection of P1 and P2 to
		// deformed sphere
		math::Vector3<REAL>  p1(pt1.x, pt1.y, tb_project_to_sphere(trackballsize, pt1.x, pt1.y));
		math::Vector3<REAL>  p2(pt2.x, pt2.y, tb_project_to_sphere(trackballsize, pt2.x, pt2.y));

		//  Now, we want the cross product of P1 and P2
		a = crossVec3(p1, p2);
		//  Figure out how much to rotate around that axis.
		d.x = p1.x - p2.x;
		d.y = p1.y - p2.y;
		d.z = p1.z - p2.z;
		t = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z) / (trackballsize);

		// Avoid problems with out-of-control values...

		if (t > 1.0)
			t = 1.0;
		if (t < -1.0)
			t = -1.0;
		phi = 2.0 * REAL(asin(t));

		axis_to_quat(q, a, phi);
		return q;
	}

	template<class REAL>
	REAL									acosR(REAL sv)
	{
		return glm::acos(sv);
	}

	template<class REAL>
	REAL									sinR(REAL sv)
	{
		return glm::sin(sv);
	}
	template<class REAL>
	REAL									cosR(REAL sv)
	{
		return glm::cos(sv);
	}

	template<class REAL>
	math::Vector3<REAL>						floor(const math::Vector3<REAL>&v)
	{
		return glm::floor(v);
	}
	template<class REAL>
	math::Vector2<REAL>						floor(const math::Vector2<REAL>&v)
	{
		return glm::floor(v);
	}
}
#endif /* Header_h */

namespace math
{
	template< class T >
	const T Min(const T& a, const T& b)
	{
		return (b < a) ? b : a;
	}

	template< class T >
	const T Max(const T& a, const T& b)
	{
		return (b > a) ? b : a;
	}
}

