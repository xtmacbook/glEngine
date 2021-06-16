#include "mesh.h"
#include "renderMesh.h"
#include "gls.h"
#include "log.h"

IRenderMeshObj::~IRenderMeshObj()
{
	clear();
	if (vao_)
		glDeleteVertexArrays(1, &vao_);
	if (pos_vbo_)
		glDeleteBuffers(1, &pos_vbo_);
	if (indices_vbo_)
		glDeleteBuffers(1, &indices_vbo_);
}

void IRenderMeshObj::preRender(Shader*)
{

}

void IRenderMeshObj::postRender(Shader*)
{

}

void IRenderMeshObj::draw()
{
	CHECK_GL_ERROR;

	if (DRAW_ARRAYS == call_)
		glMultiDrawArrays(mesh_->mode_, first_, count_, drawcount_);
	else if (DRAW_ARRAYS_INDIRECT == call_)
		glMultiDrawArraysIndirect(mesh_->mode_, mesh_->aIndirectCom_,drawcount_, stride_);
	else if (DRAW_ARRAYS_INSTANC == call_)
		glDrawArraysInstancedBaseInstance(mesh_->mode_, first_[0], count_[0], instanceCount_, baseInstance_);
	else if (DRAW_ELEMENTS == call_)
		glMultiDrawElements(mesh_->mode(), (GLsizei*)count_, type_, (const GLvoid**)indices_, drawcount_);
	else if (DRAW_ELEMENTS_BASE_VERTEX == call_)
		glMultiDrawElementsBaseVertex(mesh_->mode_, const_cast<int *>(count_), type_, const_cast<void **>(indices_), drawcount_, basevertex_);
	else if (DRAW_ELEMENT_INSTANCE_BASE_VERTEX == call_)
		glDrawElementsInstancedBaseVertex(mesh_->mode_, count_[0], type_, indices_[0], instanceCount_, basevertex_[0]);
	else if (DRAW_ELEMENT_INSTANCE_BASE_INSTANCE == call_)
		glDrawElementsInstancedBaseInstance(mesh_->mode_, count_[0], type_, indices_[0], instanceCount_, baseInstance_);
	else if (DRAW_ELEMENT_INDIRECT == call_)
		glMultiDrawElementsIndirect(mesh_->mode_, type_, (GLvoid*)0, drawcount_, stride_);
	else
	{
		PRINT_ERROR("no draw api enable");
	}
	CHECK_GL_ERROR;

}

void IRenderMeshObj::clear()
{

	if (first_ != NULL)
	{
		delete[]first_;
		first_ = NULL;

		delete[] count_;
		count_ = NULL;
	}

	if (indices_ != NULL)
	{
		for (int i = 0; i < drawcount_; i++)
		{
			uint16* tmp = (uint16*)indices_[i];
			delete[] tmp;
			tmp = NULL;
		}

		delete[] count_;
		count_ = NULL;
	}
	
	drawcount_ = 0;
	instanceCount_ = 0;
	baseInstance_ = 0;
}
