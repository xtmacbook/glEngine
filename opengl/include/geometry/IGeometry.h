//
//  baseGeometry.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018年 xt. All rights reserved.
//

#ifndef baseGeometry_h
#define baseGeometry_h

#include "BaseObject.h"
#include "SmartPointer.h"
#include "boundingBox.h"
#include "mesh.h"
#include "common.h"
#include "renderMesh.h"

#include <string>
#include <vector>


class Shader;
class CameraBase;

struct DrawInfo
{
	Shader* draw_shader_ = nullptr;
	bool needBind_ = true;
	Matrixf	matrix_ = Matrixf(1.0f);
};

class LIBENIGHT_EXPORT CommonGeometry :public base::BaseObject
{
public:
	CommonGeometry(bool update = false);

	virtual ~CommonGeometry();

	void         bindVAO();
	void         unBindVAO();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry() = 0;
	virtual void drawGeoemtry(const DrawInfo&) = 0;
	virtual void computeBoundingBox(void *);
	virtual Matrixf				getModelMatrix()const;
	virtual void				updateModelMatix(const Matrixf&);

	const base::BoundingBox& boundingBox()const;
protected:
	unsigned int  vao_;
	base::BoundingBox  box_;
	Matrixf modelMatrix_;
	bool   beupdte_;
};

class LIBENIGHT_EXPORT Cub :public CommonGeometry
{
public:
	Cub(bool update = false);
	virtual ~Cub();
	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

private:
	unsigned int vbo_;
};

class LIBENIGHT_EXPORT Quad :public CommonGeometry
{
public:
	Quad(bool update = false);
	~Quad();

	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	unsigned int vbo_;

};

class LIBENIGHT_EXPORT ObjGeometry :public CommonGeometry
{
public:
	ObjGeometry(const char*file);
	~ObjGeometry();
	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

private:
	unsigned int vbo_[3];//vertex texture normal
	std::string file_;
	int vertices_size_ = 0;
};

class LIBENIGHT_EXPORT MeshGeometry :public CommonGeometry
{
public:
	MeshGeometry(bool update = false);
	~MeshGeometry();

	virtual void preRender();
	virtual void postRender();
	virtual void updateGeometry(const CameraBase* camera);
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
	virtual void computeBoundingBox(void *);

	void		 addMesh(Mesh_SP);
	
	void         clear();

	std::vector<Mesh_SP > meshs_;
	std::vector<IRenderMeshObj_SP> mesh_obj_;

protected:
	virtual IRenderMeshObj_SP createRenderMeshObj();
	virtual void setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj);
	virtual void drawMesh(int , Matrixf,Shader *, const Mesh_SP mesh, IRenderMeshObj_SP& obj);
	virtual int  setupVertexAttribute(const int);
	virtual void setupOtherBufferAndVA(int idx);
	
private:
	int vertices_size_ = 0;
};

typedef base::SmartPointer<CommonGeometry> CommonGeometry_Sp;
typedef base::SmartPointer<MeshGeometry>	MeshGeometry_Sp;

#endif /* baseGeometry_h */
