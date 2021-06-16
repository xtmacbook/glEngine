#ifndef Opengl_IRenderNode_h
#define Opengl_IRenderNode_h

#include "baseScene.h"
#include "smartPointer.h"
#include "boundingBox.h"
#include "BaseObject.h"

using namespace base;
class CameraBase;

struct LIBENIGHT_EXPORT IRenderNode :public BaseObject
{
	virtual void			render(Shader*, PassInfo&) = 0;
	virtual void			update(const CameraBase*) = 0;
	virtual AABB			getBox()const = 0;
	virtual Matrixf			getModelMatix()const = 0;

	virtual	void	        	bindVAO() = 0;
	virtual void			unBindVAO() = 0;

};

typedef SmartPointer<IRenderNode> IRenderNode_SP;


#endif
