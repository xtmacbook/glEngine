
#include "IGeometry.h"
#include "resource.h"
#include "meshLoad.h"
#include "log.h"
#include "shader.h"
#include "helpF.h"
#include "gls.h"
#include "mesh.h"

static GLfloat vertices[] = {
	// Back face
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
	// Front face
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
	// Left face
-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
	// Right face
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
	// Bottom face
-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
	// Top face
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
};
static GLfloat planeVertices[] = {
	// Positions            // Normals           // Texture Coords
25.0f, -0.5f,  25.0f,  0.0f,  1.0f,  0.0f,  25.0f, 0.0f,
-25.0f, -0.5f, -25.0f,  0.0f,  1.0f,  0.0f,  0.0f,  25.0f,
-25.0f, -0.5f,  25.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,

25.0f, -0.5f,  25.0f,  0.0f,  1.0f,  0.0f,  25.0f, 0.0f,
25.0f, -0.5f, -25.0f,  0.0f,  1.0f,  0.0f,  25.0f, 25.0f,
-25.0f, -0.5f, -25.0f,  0.0f,  1.0f,  0.0f,  0.0f,  25.0f
};
static GLfloat quadVertices[] = {
	// positions   // texCoords
-1.0f,  1.0f,  0.0f, 1.0f,
-1.0f, -1.0f,  0.0f, 0.0f,
1.0f, -1.0f,  1.0f, 0.0f,

-1.0f,  1.0f,  0.0f, 1.0f,
1.0f, -1.0f,  1.0f, 0.0f,
1.0f,  1.0f,  1.0f, 1.0f
};

CommonGeometry::CommonGeometry(bool update /*= false*/):beupdte_(update)
{
	box_.init();
	modelMatrix_ = Matrixf(1.0);
	glGenVertexArrays(1, &vao_);
}

void CommonGeometry::bindVAO()
{
	glBindVertexArray(vao_);
}

void CommonGeometry::unBindVAO()
{
	glBindVertexArray(0);
}


void CommonGeometry::updateGeometry(const CameraBase* camera)
{

}

CommonGeometry::~CommonGeometry()
{
	glDeleteVertexArrays(1, &vao_);
}

const base::BoundingBox& CommonGeometry::boundingBox() const
{
	return box_;
}

void CommonGeometry::computeBoundingBox(void *)
{

}

math::Matrixf CommonGeometry::getModelMatrix()const
{
	return modelMatrix_;
}


void CommonGeometry::updateModelMatix(const Matrixf&m)
{
	modelMatrix_ = m;
}

Cub::Cub(bool update):CommonGeometry(update)
{
	glGenBuffers(1, &vbo_);
}

Cub::~Cub()
{
	glDeleteBuffers(1, &vbo_);
}

void Cub::updateGeometry()
{
	
}

void Cub::initGeometry()
{
	bindVAO();
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Link vertex attributes
	glBindVertexArray(vao_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	box_.init();
	int num = sizeof(vertices) / sizeof(vertices[0]);
	for (int i = 0; i < num; i += 8)
	{
		int j = i;
		box_.expandBy(vertices[j],vertices[j + 1],vertices[j + 2]);
	}
}

void Cub::computeBoundingBox(void*data)
{
	
	 
}

void Cub::drawGeoemtry(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	if (location != -1)
	{
		Matrixf transform = di.matrix_* getModelMatrix();
		di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	}
	if (di.needBind_) glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

Quad::Quad(bool update) :CommonGeometry(update)
{
	glGenBuffers(1, &vbo_);
}

Quad::~Quad()
{
	glDeleteBuffers(1, &vbo_);
}


void Quad::updateGeometry()
{

}

void Quad::initGeometry()
{
	bindVAO();

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

void Quad::computeBoundingBox(void*)
{

}

void Quad::drawGeoemtry(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	if (location != -1)
	{
		Matrixf transform = di.matrix_* getModelMatrix();
		di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	}

	if (di.needBind_) glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

ObjGeometry::ObjGeometry(const char*file) :CommonGeometry(false),
file_(file)
{

}

ObjGeometry::~ObjGeometry()
{
	glDeleteBuffers(3, vbo_);
}


void ObjGeometry::updateGeometry()
{

}

void ObjGeometry::initGeometry()
{
	std::vector < math::V3f >  out_vertices;
	std::vector < math::V2f >  out_uvs;
	std::vector < math::V3f >  out_normals;

	bool result = IO::NodeFile::loadOBJ(file_.c_str(), out_vertices,
		out_uvs, out_normals);
	if (result)
	{
		vertices_size_ = out_vertices.size();
		bindVAO();
		glGenBuffers(3, vbo_);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(math::V3f) * out_vertices.size(), &out_vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(math::V2f) * out_uvs.size(), &out_uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(math::V3f) * out_normals.size(), &out_normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glBindVertexArray(0);

		computeBoundingBox(&out_vertices[0]);
	}
}


void ObjGeometry::drawGeoemtry(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	if (location != -1)
	{
		Matrixf transform = di.matrix_* getModelMatrix();
		di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	}
	if(di.needBind_) glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, vertices_size_);
	glBindVertexArray(0);
}


void ObjGeometry::computeBoundingBox(void *data)
{
	box_.init();
	math::V3f * vertices = (math::V3f*)(data);
	if (vertices)
	{
		for (int i = 0; i < vertices_size_; i++)
		{
			box_.expandBy(vertices[i]);
		}
	}
}

MeshGeometry::MeshGeometry(bool update) :CommonGeometry(update)
{

}

MeshGeometry::~MeshGeometry()
{
	for (int i = 0; i < meshs_.size(); i++)
	{
		if (meshs_[i])
		{
			delete meshs_[i].addr();
			meshs_[i] = NULL;
		}
	}

	for (int i = 0; i < mesh_obj_.size(); i++)
	{
		if (mesh_obj_[i])
		{
			delete mesh_obj_[i].addr();
			mesh_obj_[i] = NULL;
		}
	}
}


void MeshGeometry::preRender()
{

}


void MeshGeometry::postRender()
{

}


void MeshGeometry::updateGeometry(const CameraBase* camera)
{
	if (!beupdte_) return;
}

void MeshGeometry::initGeometry()
{
	std::vector<Mesh_SP>::const_iterator iter = meshs_.cbegin();

	for (; iter != meshs_.cend(); iter++)
	{
		IRenderMeshObj_SP irmsp = createRenderMeshObj();
		setupMesh(*iter, irmsp);
		mesh_obj_.push_back(irmsp);
	}
	computeBoundingBox(NULL);
}

void MeshGeometry::drawGeoemtry(const DrawInfo&di)
{
	/**
	 *  because the meshGeometry have many mesh object,and the getModelMatrix() is the matrix for meshGeometry
	    but each of mesh object have a matrix
	 */
	preRender();

	Shader * shader = di.draw_shader_;
	GLint loc = shader->getVariable("model");
	Matrixf transform = di.matrix_ * getModelMatrix();

	for (int i = 0; i < mesh_obj_.size(); i++)
		drawMesh(loc, transform,shader,meshs_[i], mesh_obj_[i]);
	
	postRender();
}


void MeshGeometry::computeBoundingBox(void *)
{
	std::vector<base::SmartPointer<Mesh> >::const_iterator iter = meshs_.cbegin();
	for (; iter != meshs_.cend(); iter++)
	{
		box_.expandBy((*iter)->getBOX());
	}
}


void MeshGeometry::addMesh(Mesh_SP mesh)
{
	meshs_.push_back(mesh);
}

void MeshGeometry::clear()
{

}

IRenderMeshObj_SP MeshGeometry::createRenderMeshObj()
{
	return new RenderMeshObj;
}


void MeshGeometry::setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj)
{
	obj->mesh_ = mesh;
	obj->call_ = mesh->call_;
	obj->type_ = mesh->type_;
	if (obj->type_ == 0) obj->type_ = GL_UNSIGNED_SHORT;

	glGenVertexArrays(1, &obj->vao_);
	glBindVertexArray(obj->vao_);

	Vertex * vertexData = NULL;
	if (DRAW_ARRAYS == mesh->call_)
	{
		int vertexSize = 0;
		const std::vector<std::vector<Vertex> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();
		
		obj->first_ = new int[multVertexSize]; //delete
		obj->count_ = new int[multVertexSize]; //delete
		obj->drawcount_ = multVertexSize;

		for (int i = 0; i < multVertexSize; i++)
		{
			obj->first_[i] = vertexSize;
			vertexSize += obj->count_[i] = vs[i].size();
		}

		vertexData = new Vertex[vertexSize];  //delete
		Vertex* temp = vertexData;
		for (int i = 0; i < multVertexSize; i++)
		{
			std::copy(vs[i].cbegin(), vs[i].cend(), temp);
			temp += vs[i].size();
		}
		
		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexSize * sizeof(Vertex), vertexData, GL_STATIC_DRAW);

		setupVertexAttribute(mesh->CFVF());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
	else if (DRAW_ARRAYS_INDIRECT == mesh->call_) 
	{

	}
	else if (DRAW_ARRAYS_INSTANC == mesh->call_) 
	{

	}
	else if (DRAW_ELEMENTS == mesh->call_) 
	{
		//vertex data
		const std::vector<std::vector<Vertex> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();
		assert(multVertexSize == 1);

		//indices
		int multIds = mesh->indices_.size();
		obj->count_ = new int[multIds];  //delete
		obj->indices_ = (void**)new uint16 * [multIds];//uint16 type->GL_UNSIGNED_SHORT //delete
		obj->drawcount_ = multIds;

		for (int i = 0; i < multIds; i++)
		{
			obj->count_[i] = mesh->indices_[i].size();
			obj->indices_[i] = new uint16[obj->count_[i]];
			const std::vector<uint16>& vi = mesh->indices_[i];
			std::copy(vi.cbegin(), vi.cend(),(uint16*) (obj->indices_[i]));
		}

		const std::vector<Vertex>& vertexs = vs[0];

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);
		
		setupVertexAttribute(mesh->CFVF());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		CHECK_GL_ERROR;
	}
	else if (DRAW_ELEMENTS_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_INSTANCE == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INDIRECT == mesh->call_) {

		//vertex data
		const std::vector<std::vector<Vertex> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();
		assert(multVertexSize == 1);

		int multIds = mesh->indices_.size();
		assert(multIds == 1);

		const std::vector<Vertex>& vertexs = vs[0];
		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

		int idx = setupVertexAttribute(mesh->CFVF());

		const std::vector<uint16>& inds = mesh->indices_[0];
		glGenBuffers(1, &obj->indices_vbo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->indices_vbo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * inds.size(), &inds[0], GL_STATIC_DRAW);

		glGenBuffers(1, &obj->indirect_vbo_);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, obj->indirect_vbo_);
		obj->drawcount_ = mesh->numIndirect_;
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * obj->drawcount_ , mesh->eIndirectCom_, GL_STATIC_DRAW);

		setupOtherBufferAndVA(idx);
		CHECK_GL_ERROR;
	}

	glBindVertexArray(0);


	Mesh * ms = mesh;
	TMesh * tms = dynamic_cast<TMesh*>(ms);
	if (tms)
	{
		RenderMeshObj * rmo = dynamic_cast<RenderMeshObj*>(obj.addr());
		rmo->t_indices_ = tms->t_indices_;
	}


	/*if (mesh->indirectCom_.size())
	{
		glGenBuffers(1, &obj->indirect_vbo_);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, obj->indirect_vbo_);
		glBufferData(GL_DRAW_INDIRECT_BUFFER,( mesh->indirectCom_.size() * sizeof(unsigned int)), &(mesh->indirectCom_[0]), GL_STATIC_DRAW);
	}
 */

}


void MeshGeometry::drawMesh(int loc, Matrixf transform, Shader *shader,const Mesh_SP mesh, IRenderMeshObj_SP& obj)
{
	if (loc != -1)
	{
		transform *= mesh->Cmatrix();
		shader->setMatrix4(loc, 1, GL_FALSE, math::value_ptr(transform));
	}
	obj->preRender(shader);
	glBindVertexArray(obj->vao_);
	obj->draw();
	glBindVertexArray(0);
	obj->postRender(shader);
}

int MeshGeometry::setupVertexAttribute(const int fvf)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	GLuint indx = 1;
	if (fvf & FVF_NORMAL)
	{
		// vertex normals
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		indx += 1;
	}
	if (fvf & FVF_TEXT0)
	{
		// vertex texture coords
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		indx += 1;
	}
	if (fvf & FVF_TANGENT)
	{
		// vertex tangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		indx += 1;
	}
	if (fvf & FVF_BITANGENT)
	{
		// vertex bitangent
		glEnableVertexAttribArray(indx);
		glVertexAttribPointer(indx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		indx += 1;
	}

	return indx;
}

void MeshGeometry::setupOtherBufferAndVA(int idx)
{

}
