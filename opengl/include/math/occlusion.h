//
//  occlusion.h
//
//  Created by xt on 20/08/25.
//  Copyright  2020 xt. All rights reserved.
//

#ifndef OPENGL_OCCLUSION_HEAD_H
#define OPENGL_OCCLUSION_HEAD_H

#include "type.h"
#include "decl.h"

namespace math
{
	void LIBENIGHT_EXPORT getPosAndNegPointProjPlane(const  V4f*plane, const V3f&boxMin, const V3f&boxMax, V3f * out);
	void LIBENIGHT_EXPORT getPosAndNegPointProjPlane(const  float*plane, const V3f&boxMin, const V3f&boxMax, V3f * out);
}

#endif
