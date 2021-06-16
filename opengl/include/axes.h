#include "IGeometry.h"
#include "IRenderNode.h"
#include "shader.h"
#ifndef _axes_h_
#define _axes_h_

class AexsGeometry :public MeshGeometry
{
public:
	AexsGeometry();
	virtual void initGeometry();
};

IRenderNode_SP createAxesRenderNode();
Shader_SP   createAxesRenderNodeShader();
#endif
