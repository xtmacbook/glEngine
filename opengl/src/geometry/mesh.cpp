#include "mesh.h"
#include "log.h"
#include <algorithm>

Mesh::Mesh()
{

}
Mesh::~Mesh()
{

}

base::AABB IMesh::getBOX() const
{
	return box_;
}

void IMesh::setBox(const AABB&box)
{
	box_ = box;
}

void Mesh::computeBox()
{
	std::vector<std::vector<Vertex> > ::const_iterator iter = vertices_.cbegin();
	for (; iter != vertices_.cend(); iter++)
	{
		const std::vector<Vertex>& v = *iter;
		
		for_each(v.cbegin(), v.cend(), [&](const Vertex&vv) {
			box_.expandBy(matrix_ * V4f(vv.Position, 1.0));
		});
	}
}

void Mesh::addVertex(int index,const Vertex&v)
{
	if (index >= vertices_.size()) vertices_.push_back(std::vector<Vertex>());
	vertices_[index].push_back(v);
}

void Mesh::addVertex(const Vertex&v)
{
	addVertex(0, v);
}

void Mesh::addIndices(const uint16 i)
{
	addIndices(0,i);
}

void Mesh::addIndices(int index, const uint16 i)
{
	if (index >= indices_.size()) indices_.push_back(std::vector<uint16>());
	indices_[index].push_back(i);
}

void Mesh::clear()
{
	vertices_.clear();
	indices_.clear();
	box_.init();
	//fvf_ = FVF_NONE;
}


TMesh::TMesh()
{

}

TMesh::TMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{

}
