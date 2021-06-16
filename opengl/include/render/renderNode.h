#ifndef Opengl_RenderNode_h
#define Opengl_RenderNode_h

#include "IRenderNode.h"
#include "BaseObject.h"
#include "IGeometry.h"
#include "texture.h"

using namespace base;

class LIBENIGHT_EXPORT RenderNode :public IRenderNode
{
public:
	virtual void				render(Shader*, PassInfo&);
	virtual void				update(const CameraBase*);

	virtual AABB				getBox()const;
	virtual Matrixf				getModelMatix()const;

	void					setGeometry(CommonGeometry_Sp);
	CommonGeometry_Sp			getGeometry();

	virtual	void				bindVAO() ;
	virtual void				unBindVAO() ;
	
	void					    loadTextures();
	static	IRenderNode_SP			loadFromFile(const char*file);

	CommonGeometry_Sp			geometry_;

	std::vector<Texture_Sp>		textures_;
};
typedef SmartPointer<RenderNode> RenderNode_SP;

#endif
