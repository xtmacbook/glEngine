//
//  comm.cpp
//  OpenGL
//
//  Created by xt on 15/7/24.
//  Copyright (c) 2015年 xt. All rights reserved.
//

#include "baseMesh.h"
#include "gls.h"
#include "helpF.h"

 

Mesh_SP createArrow(const V3f& color)
{
	Vertex origin;
	origin.Position = V3f((0.0, 0.0, 0.0));
	origin.Normal = color;

	unsigned int sample = 36;
	float detadegree = 360.0 / sample;

	Mesh_SP ms = new Mesh;
	ms->rmode() = GL_TRIANGLE_FAN;
	ms->RFVF() = FVF_XYZ;
	ms->RFVF() |= FVF_NORMAL;

	ms->addVertex(origin);

	for (int i = 0; i <= sample; i++)
	{
		float radian = math::degreeToRadin(i * detadegree);

		Vertex vp;
		vp.Position.z = math::sinR(radian);
		vp.Position.y = math::cosR(radian);
		vp.Position.x = -2.0f;
		vp.Normal = color;

		ms->addVertex(vp);
	}

	ms->computeBox();

	return ms;
}

IRenderNode_SP getLine(const V3f& s, const V3f& e, const V3f& color, bool update)
{
	RenderNode_SP rn = new RenderNode;
	base::SmartPointer<DLineMeshGeoemtry> mg = new DLineMeshGeoemtry(s, e, color, update);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getPoints(const V3f& s, const V3f& color, bool update)
{
	RenderNode_SP rn = new RenderNode;
	base::SmartPointer<DPointsMeshGeoemtry> mg = new DPointsMeshGeoemtry(s, color, update);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}


LIBENIGHT_EXPORT IRenderNode_SP getRay(const V3f& s, const V3f& e, const V3f& color, bool update)
{
	Mesh_SP mesh = createArrow(V3f(1.0, 0.0, 0.0));
	Matrixf tran = math::translateR(Matrixf(1.0), e);
	V3f ray = e - s;
	math::normalizeVec3(ray);
	Matrixf rot = math::mfq(math::qftv(V3f(1.0, 0.0, 0.0), ray));
	mesh->Rmatrix() = tran * rot * math::scaleR(Matrixf(1.0f), V3f(0.05, 0.05, 0.05));
	RenderNode_SP rn = new RenderNode;
	base::SmartPointer<DLineMeshGeoemtry> mg = new DLineMeshGeoemtry(s, e, color, update);
	mg->addMesh(mesh);
	mg->initGeometry();
	if (rn) rn->setGeometry(mg);
	return rn;
}

DLineMeshGeoemtry::DLineMeshGeoemtry(const V3f& s, const V3f& e, const V3f& color, bool update /*= false*/) :MeshGeometry(update)
{
	Vertex start, end;
	start.Position = s;
	start.Normal = color;
	end.Position = e;
	end.Normal = color;

	Mesh_SP ms = new Mesh;
	ms->rmode() = GL_LINES;
	ms->RFVF() = FVF_XYZ;
	ms->RFVF() |= FVF_NORMAL;
	ms->addVertex(start);
	ms->addVertex(end);
	ms->computeBox();
	meshs_.push_back(ms);

}

void DLineMeshGeoemtry::updateGeometry()
{

}

void DLineMeshGeoemtry::pushPoint(const V3f&)
{

}

void DLineMeshGeoemtry::popPoint()
{

}

DPointsMeshGeoemtry::DPointsMeshGeoemtry(const V3f& s, const V3f& color, bool update /*= false*/)
	:MeshGeometry(update)
{
	Vertex start;
	start.Position = s;
	start.Normal = color;

	Mesh_SP ms = new Mesh;
	ms->rmode() = GL_POINTS;
	ms->RFVF() = FVF_XYZ;
	ms->RFVF() |= FVF_NORMAL;
	ms->addVertex(start);
	ms->computeBox();
	meshs_.push_back(ms);
}

void DPointsMeshGeoemtry::updateGeometry()
{

}

void DPointsMeshGeoemtry::pushPoint(const V3f&)
{

}

void DPointsMeshGeoemtry::popPoint()
{

}
