//
//  comm.h
//  OpenGL
//
//  Created by xt on 15/7/24.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#ifndef __OpenGL__comm__
#define __OpenGL__comm__
#include "common.h"
#include "shader.h"
#include "renderNode.h"
#include "IGeometry.h"

#include <map>

class LIBENIGHT_EXPORT DLineMeshGeoemtry :public MeshGeometry
{
public:
	DLineMeshGeoemtry(const V3f& s, const V3f& e, const V3f& color, bool update = false);

	virtual void updateGeometry();
	void pushPoint(const V3f&);
	void popPoint();
};

class LIBENIGHT_EXPORT DPointsMeshGeoemtry :public MeshGeometry
{
public:
	DPointsMeshGeoemtry(const V3f& s, const V3f& color, bool update = false);

	virtual void updateGeometry();
	void pushPoint(const V3f&);
	void popPoint();
};

LIBENIGHT_EXPORT IRenderNode_SP getLine(const V3f& s, const V3f& e, const V3f& color, bool);
LIBENIGHT_EXPORT IRenderNode_SP getPoints(const V3f& s, const V3f& color, bool);
LIBENIGHT_EXPORT IRenderNode_SP getRay(const V3f& s, const V3f& end, const V3f& color, bool);

#endif /* defined(__OpenGL__comm__) */
