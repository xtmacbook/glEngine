#ifndef _DYNAMIC_MESH_H_
#define _DYNAMIC_MESH_H_

#include "IGeometry.h"
#include "flashBuffer.h"
//Ä¿Ç°Ö»ÊÊÓ¦Ò»¸ömesh
class LIBENIGHT_EXPORT DynamicMeshGeoemtry :public MeshGeometry
{
public:
	DynamicMeshGeoemtry(int flag, unsigned verticesSize, unsigned indiceSize);
	~DynamicMeshGeoemtry();
	virtual void postRender();
	virtual void initGeometry();
	virtual void updateGeometry(const CameraBase* camera);
	void		 clearMesh();
	inline int&	RFVF() { return FVF_; }
	
	void     cpu2GpuTime(uint64*, uint64*);
	void     swapQueryTime();


protected:
	virtual void setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj);
protected:

	FlashBuffer<Vertex>*			vertexBuffer_ = NULL;
	FlashBuffer<unsigned int >*		indicesBuffer_ = NULL;

	GLuint						vao_;
	int							FVF_ = FVF_XYZ;
	bool						have_indices_ = false;
	int							buffer_mode_;
	unsigned					vertices_buffer_pool_size_;
	unsigned					indices_buffer_pool_size_;
	unsigned					vertices_size_ = 0;

	unsigned					current_v_index_ = 0;
	unsigned					current_i_index_ = 0;
	bool						first_ = true;
};
typedef base::SmartPointer<DynamicMeshGeoemtry>	DMeshGeometry_Sp;

#endif
