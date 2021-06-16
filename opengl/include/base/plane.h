
//
//  stream.h
//  OpenGL
//
//  Created by xt on 20/1/09.
//  Copyright  2019  xt. All rights reserved.
//

#ifndef __OpenGL__Plane__
#define __OpenGL__Plane__

#include "decl.h"
#include "type.h"
#include <stdio.h> 

using namespace math;
//////////////////////////////////////////////////////////////////////
namespace base
{
	template<typename F> struct Plane
	{

		//plane-equation: n.x*x + n.y*y + n.z*z + d > 0 is in front of the plane
		Vector3<F> n;   //!< Normal.
		F           d;   //!< Distance.

						 //----------------------------------------------------------------
		inline Plane() {};

		inline Plane(const Plane<F>& p) { n = p.n; d = p.d; }
		inline Plane(const Vector3<F>& normal, const F& distance) { n = normal; d = distance; }

		//! set normal and dist for this plane and  then calculate plane type
		inline void set(const Vector3<F>& vNormal, const F fDist)
		{
			n = vNormal;
			d = fDist;
		}

		inline void setPlane(const Vector3<F>& normal, const Vector3<F>& point)
		{
			n = normal;
			d = -(point | normal);
		}
		inline static Plane<F> createPlane(const Vector3<F>& normal, const Vector3<F>& point)
		{
			return Plane<F>(normal, -(point | normal));
		}

		inline Plane<F> operator-(void) const { return Plane<F>(-n, -d); }

		//! Constructs the plane by tree given Vec3s (=triangle) with a right-hand (anti-clockwise) winding.
		//! Example 1:
		//!   Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
		//!   Plane_tpl<F>  plane;
		//!   plane.SetPlane(v0,v1,v2);
		//! Example 2:
		//!   Vec3 v0(1,2,3),v1(4,5,6),v2(6,5,6);
		//!   Plane_tpl<F>  plane=Plane_tpl<F>::CreatePlane(v0,v1,v2);
		inline void setPlane(const Vector3<F>& v0, const Vector3<F>& v1, const Vector3<F>& v2)
		{
			n = ((v1 - v0) % (v2 - v0));//vector cross-product
			normalizeVec3(n);
			d = -(n | v0);                               //calculate d-value
		}
		inline static Plane<F> CreatePlane(const Vector3<F>& v0, const Vector3<F>& v1, const Vector3<F>& v2)
		{
			Plane<F> p;
			p.setPlane(v0, v1, v2);
			return p;
		}

		//! Computes signed distance from point to plane.
		//! This is the standard plane-equation: d=Ax*By*Cz+D.
		//! The normal-vector is assumed to be normalized.
		//! Example:
		//!   Vec3 v(1,2,3);
		//!   Plane_tpl<F>  plane=CalculatePlane(v0,v1,v2);
		//!   F distance = plane|v;
		inline F            operator|(const Vector3<F>& point) const { return (n | point) + d; }
		inline F            distFromPlane(const Vector3<F>& vPoint) const { return (n * vPoint + d); }

		inline Plane<F> operator-(const Plane<F>& p) const { return Plane<F>(n - p.n, d - p.d); }
		inline Plane<F> operator+(const Plane<F>& p) const { return Plane<F>(n + p.n, d + p.d); }
		inline void         operator-=(const Plane<F>& p) { d -= p.d; n -= p.n; }
		inline Plane<F> operator*(F s) const { return Plane<F>(n * s, d * s); }
		inline Plane<F> operator/(F s) const { return Plane<F>(n / s, d / s); }

		//! check for equality between two planes
		friend  bool operator==(const Plane<F>& p1, const Plane<F>& p2)
		{
			if (fabsf(p1.n.x - p2.n.x) > 0.0001f) return (false);
			if (fabsf(p1.n.y - p2.n.y) > 0.0001f) return (false);
			if (fabsf(p1.n.z - p2.n.z) > 0.0001f) return (false);
			if (fabsf(p1.d - p2.d) < 0.01f) return(true);
			return (false);
		}

		Vector3<F> MirrorVector(const Vector3<F>& i) { return n * (2 * (n | i)) - i; }
		Vector3<F> MirrorPosition(const Vector3<F>& i) { return i - n * (2 * ((n | i) + d)); }
	};

	typedef Plane<f32>  Plane32;    //!< Always 32 bit.

}
#endif
