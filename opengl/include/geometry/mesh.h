#ifndef mesh_h
#define mesh_h

#include "decl.h"
#include "type.h"
#include "texture.h"
#include "boundingBox.h"
#include "vertex.h"
#include "util.h"
#include <vector>

using namespace math;
using namespace base;

typedef struct
{
	unsigned int  count_; 
	unsigned int  instanceCount_;
	unsigned int  firstIndex_;
	unsigned int  baseVertex_;
	unsigned int  baseInstance_;
}DrawElementsIndirectCommand;

typedef  struct {
	uint  count_;
	uint  instanceCount_;
	uint  first_;
	uint  baseInstance_;
} DrawArraysIndirectCommand;

struct IMesh
{
	virtual  AABB getBOX()const ;
	virtual void setBox(const AABB&);
	inline AABB& box() { return box_; }

	virtual void computeBox() = 0;

	inline int&	RFVF() { return fvf_; }
	inline int	CFVF()const { return fvf_; }

	inline	const math::Matrixf&		 Cmatrix()const { return matrix_; }
	inline	math::Matrixf&		 Rmatrix() { return matrix_; }

	inline	const unsigned int mode()const { return mode_; }
	inline	unsigned int& rmode() { return mode_; }

	math::Matrixf				matrix_ = math::Matrixf(1.0);
	int							fvf_ = FVF_NONE;
	AABB						box_;
	unsigned int				mode_;
	int							type_ = 0;
};

class LIBENIGHT_EXPORT Mesh :public BaseObject, public IMesh
{
public:

	Mesh();

	~Mesh();

	virtual void computeBox();                                                           
	
	void	addVertex(int index, const Vertex&v);
	void	addVertex(const Vertex&v);
	void	addIndices(const uint16 i);
	void	addIndices(int index,const uint16 i);

	inline std::vector<Vertex>& RVertex(const int index) { return vertices_[index]; }
	inline const std::vector<Vertex>& CVertex(const int index)const { return vertices_[index]; }

	void						clear();
	inline unsigned int			size()const { return vertices_.size(); }

	std::vector<std::vector<Vertex> >				vertices_;
	std::vector < std::vector<uint16> >				indices_;
	
	DrawElementsIndirectCommand*					eIndirectCom_ = NULL;
	DrawArraysIndirectCommand*						aIndirectCom_ = NULL;
	short											call_ = DRAW_ARRAYS;
	uint											numIndirect_ = 0;
};


class LIBENIGHT_EXPORT TMesh :public Mesh
{
public:

	TMesh();
	TMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	std::vector<unsigned int>			t_indices_; //texture indices
};

typedef base::SmartPointer<Mesh>	Mesh_SP;
typedef base::SmartPointer<TMesh>	TMesh_SP;

#endif
