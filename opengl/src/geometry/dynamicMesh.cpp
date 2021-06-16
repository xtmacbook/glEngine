#include "dynamicMesh.h"
#include "log.h"

#include <algorithm>

DynamicMeshGeoemtry::DynamicMeshGeoemtry(int flag, unsigned verticesSize, unsigned indiceSize) :MeshGeometry(true),
buffer_mode_(flag),
vertices_buffer_pool_size_(verticesSize),
indices_buffer_pool_size_(indiceSize)
{
}

DynamicMeshGeoemtry::~DynamicMeshGeoemtry()
{
	if (vertexBuffer_)
	{
		delete vertexBuffer_;
		vertexBuffer_ = NULL;
	}
	if (indicesBuffer_)
	{
		delete indicesBuffer_;
		indicesBuffer_ = NULL;
	}
	if (vao_)
		glDeleteVertexArrays(1, &vao_);
}

void DynamicMeshGeoemtry::postRender()
{
	if (vertexBuffer_) vertexBuffer_->lockBuffer();
	if (indicesBuffer_) indicesBuffer_->lockBuffer();
}

void DynamicMeshGeoemtry::initGeometry()
{
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	vertexBuffer_ = new FlashBuffer<Vertex>(vertices_buffer_pool_size_);
	vertexBuffer_->setTarget(GL_ARRAY_BUFFER);
	vertexBuffer_->create_buffers(buffer_mode_);

	if (have_indices_)
	{
		indicesBuffer_ = new FlashBuffer<unsigned>(indices_buffer_pool_size_);
		indicesBuffer_->setTarget(GL_ELEMENT_ARRAY_BUFFER);
		indicesBuffer_->create_buffers(buffer_mode_);
	}

	vertexBuffer_->bind();
	setupVertexAttribute(RFVF());
	vertexBuffer_->unbind();
	glBindVertexArray(0);
}

void DynamicMeshGeoemtry::updateGeometry(const CameraBase* camera)
{
	current_v_index_ = 0;
	current_i_index_ = 0;

	for (int i = 0; i < meshs_.size(); i++)
	{
		IRenderMeshObj_SP irmsp;
		if (first_)
		{
			irmsp = createRenderMeshObj();
			irmsp->call_ = meshs_[i]->call_;
			mesh_obj_.push_back(irmsp);
		}
		else
			irmsp = mesh_obj_[i];

		irmsp->clear();

		setupMesh(meshs_[i], irmsp);
	}
	computeBoundingBox(NULL);
	if (first_) first_ = !first_;
}

void DynamicMeshGeoemtry::clearMesh()
{
	meshs_.clear();
}

void DynamicMeshGeoemtry::cpu2GpuTime(uint64*vt, uint64*it)
{
	if (vertexBuffer_) *vt = vertexBuffer_->timestamp();
	else *vt = 0;
	if (indicesBuffer_) *it = indicesBuffer_->timestamp();
	else *it = 0;
}

void DynamicMeshGeoemtry::swapQueryTime()
{
	if (vertexBuffer_) vertexBuffer_->swapQueryTime();
	if (indicesBuffer_) indicesBuffer_->swapQueryTime();
}

void DynamicMeshGeoemtry::setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj)
{

	obj->mesh_ = mesh;
	obj->call_ = mesh->call_;
	obj->type_ = mesh->type_;
	if (obj->type_ == 0) obj->type_ = GL_UNSIGNED_SHORT;

	Vertex* vertexData = NULL;
	if (DRAW_ARRAYS == mesh->call_)
	{
		int vertexSize = 0;
		const std::vector<std::vector<Vertex> >& vs = mesh->vertices_;
		int multVertexSize = vs.size();

		obj->first_ = new int[multVertexSize];
		obj->count_ = new int[multVertexSize]; //delete
		obj->drawcount_ = multVertexSize;

		for (int i = 0; i < multVertexSize; i++)
		{
			vertexSize += obj->count_[i] = vs[i].size();
		}

		int vnum = vertexSize;
		int vsize = vnum * sizeof(Vertex);

		if (current_v_index_ + vertexSize > vertexBuffer_->blockSize())
		{
			Log::instance()->printMessage("DynamicMeshGeoemtry buffer is not enough!\n");
			exit(1);
		}

		vertexBuffer_->mapBuffer();
		Vertex* ver_data = vertexBuffer_->getBufferPtr();
		ver_data += current_v_index_;

		//copy from cpu to gpu
		if (vsize > 0)
		{
			for (int i = 0; i < multVertexSize; i++)
			{
				const std::vector<Vertex>& vd = vs[i];
				memcpy(ver_data, &vd[0], obj->count_[i] * sizeof(Vertex));
				ver_data += obj->count_[i];
				obj->first_[i] = (i == 0) ? current_v_index_ : obj->first_[i - 1] + obj->count_[ i - 1];
			}

		}
		vertexBuffer_->setDataSize((vertexSize + current_v_index_) * sizeof(Vertex));

		vertexBuffer_->flush_data();
		if (have_indices_) indicesBuffer_->flush_data();

		obj->vao_ = vao_;
		obj->pos_vbo_ = vertexBuffer_->VBO();

		obj->mesh_ = mesh;
		current_v_index_ += vnum;

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
			std::copy(vi.cbegin(), vi.cend(), (uint16*)(obj->indices_[i]));
		}

		const std::vector<Vertex>& vertexs = vs[0];
		glGenVertexArrays(1, &obj->vao_);
		glBindVertexArray(obj->vao_);

		glGenBuffers(1, &obj->pos_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, obj->pos_vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertexs.size() * sizeof(Vertex), &vertexs[0], GL_STATIC_DRAW);

		setupVertexAttribute(mesh->CFVF());

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		CHECK_GL_ERROR;
	}
	else if (DRAW_ELEMENTS_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_VERTEX == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INSTANCE_BASE_INSTANCE == mesh->call_) {

	}
	else if (DRAW_ELEMENT_INDIRECT == mesh->call_) {

	}


	

	CHECK_GL_ERROR;
}
