#ifndef Opengl_rendermesh_h
#define Opengl_rendermesh_h

#include "smartPointer.h"
#include "BaseObject.h"

#include <vector>
class Shader;

using namespace base;
 
struct LIBENIGHT_EXPORT IRenderMeshObj :public base::BaseObject
{
	~IRenderMeshObj();

	unsigned int vao_ = 0;
	unsigned int pos_vbo_ = 0;
	unsigned int indices_vbo_ = 0;
	unsigned int indirect_vbo_ = 0;
	unsigned int instance_vertex_array_vbo_ = 0;

	virtual void preRender(Shader*);
	virtual void postRender(Shader*);

	virtual void draw();

	void						clear();

	unsigned int				drawcount_; //mult draw 
	unsigned int				instanceCount_;

	short						call_ = DRAW_ARRAYS;
	unsigned int				type_;//element indices

	void **						indices_ = NULL; //mult indices for glmult

	int*						first_ = NULL;
	int*						count_ = NULL;

	int *						basevertex_ = NULL; //base vertex
	unsigned int				baseInstance_;

	int							 stride_ = 0; //for indirect

	Mesh_SP						mesh_;
};

struct LIBENIGHT_EXPORT RenderMeshObj :public IRenderMeshObj
{
	std::vector<unsigned int> t_indices_; //texture indices in renderNode textures
};

typedef base::SmartPointer<IRenderMeshObj> IRenderMeshObj_SP;

#endif
