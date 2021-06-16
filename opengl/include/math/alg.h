//
//  alg.h
//  openGLTest softchina Huawei
//
//  Created by xt on 20/05/29.
//  Copyright  2020 xt. All rights reserved.
//

#ifndef OPENGL_ALG_HEAD_H
#define OPENGL_ALG_HEAD_H
#include "decl.h"
#include "type.h"
namespace math
{

	/************************************************************************/
	/* [-1 3 -3 1
	3 -6 3 0
	-3 3 0 0
	1 0 0 0 ]
	// https://www.scratchapixel.com/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot
	*/
	/************************************************************************/

	float LIBENIGHT_EXPORT bezier_basis_matrix[];

	V3f LIBENIGHT_EXPORT evalBezierCurve(const V3f *P, const float &t);
	V3f LIBENIGHT_EXPORT evalBezierCurve(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t);
	
	V3f LIBENIGHT_EXPORT derivBezier(const V3f *P, const float &t);
	V3f LIBENIGHT_EXPORT derivBezier(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t);

	V3f LIBENIGHT_EXPORT dVBezier(const V3f *controlPoints, const float &u, const float &v);
	V3f LIBENIGHT_EXPORT dUBezier(const V3f *controlPoints, const float &u, const float &v);

	V3f LIBENIGHT_EXPORT decasteljau(const V3f *P, const float &t);
	V3f LIBENIGHT_EXPORT decasteljau(const V3f& p0, const V3f& p1, const V3f& p2, const V3f& p3, const float& t);


}

#endif
