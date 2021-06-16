
#include "footPrints.h"
namespace scene
{
	void TerrainBlock::initGeometry(int size)
	{
		//init vert
		for (int z = 0; z < size; z++)
		{
			for (int x = 0; x < size; x++)
			{
				TerrVertex vertex;
				vertex.position_ = V3f(x, 0.0, z);
				vertex.color_ = V3f(0.5, 0.5, 0.5);
				vertices_.push_back(vertex);
			}
		}
		//init indices
		for (int z = size - 1; z > 0; z--)
		{
			for (int x = size - 1; x > -1; x--)
			{
				INDICESTYPE v0i = z * size + x;
				INDICESTYPE v1i = v0i - size;

				if ((x == size - 1) && (z != size - 1))
					indices_.push_back(v0i);

				indices_.push_back(v0i);
				indices_.push_back(v1i);

				if ((x == 0) && (z != (1)))
					indices_.push_back(v1i);

			}
		}
	}

	void TerrainBlock::initOffset(int cmapsize)
	{

		short m = (cmapsize + 1) / 4;

		int a = 0;
		int b = m - 1;
		int c = 2 * m;
		int d = 3 * m - 1;

		block_offset_from_level_center_[0] = V2i(a, 0);
		block_offset_from_level_center_[1] = V2i(b, 0);
		block_offset_from_level_center_[2] = V2i(c, 0);
		block_offset_from_level_center_[3] = V2i(d, 0);

		block_offset_from_level_center_[4] = V2i(a, b);
		block_offset_from_level_center_[7] = V2i(d, b);

		block_offset_from_level_center_[8] = V2i(a, c);
		block_offset_from_level_center_[11] = V2i(d, c);

		block_offset_from_level_center_[12] = V2i(a, d);
		block_offset_from_level_center_[13] = V2i(b, d);
		block_offset_from_level_center_[14] = V2i(c, d);
		block_offset_from_level_center_[15] = V2i(d, d);
	}

	math::V2i TerrainBlock::block_offset_from_level_center_[16];

	void FootPrints::init(int cmapSize)
	{
		cmapSize_ = cmapSize;

		TerrainBlock::initOffset(cmapSize_);
		FixUp::initOffset(cmapSize_);
		InteriorTrim::initOffset(cmapSize_);

		int m = (int)((cmapSize_ + 1) / 4);
		block_.init(m);
		fixUP_.init(m);
		trim_.init(m);
		center_.init(m);
		degenerates_.init(m);
	}

	void FixUp::initGeometry(int size)
	{
		////vertical
		for (int x = 0; x < 3; x++)
		{
			for (int z = 0; z < size; z++)
			{
				TerrVertex vertex;
				vertex.position_ = V3f(x, 0.0, z);
				vertex.color_ = V3f(1.0, 0.5, 0.5);
				vertices_.push_back(vertex);
			}
		}

		for (int x = 2; x > 0; x--)
		{
			for (int z = 0; z < size; z++)
			{
				INDICESTYPE v0i = x * size + z;
				INDICESTYPE v1i = v0i - size;

				if ((z == 0) && (x != 2))
					indices_.push_back(v0i);

				indices_.push_back(v0i);
				indices_.push_back(v1i);

				if ((z == size - 1) && (x != (1)))
					indices_.push_back(v1i);

			}
		}

		int verticalIs = vertices_.size();
		//Horizontal
		for (int z = 0; z < 3; z++)
		{
			for (int x = 0; x < size; x++)
			{
				TerrVertex vertex;
				vertex.position_ = V3f(x, 0.0, z);
				vertex.color_ = V3f(1.0, 0.5, 0.5);
				vertices_.push_back(vertex);
			}
		}

		for (int z = 2; z > 0; z--)
		{
			for (int x = size - 1; x > -1; x--)
			{
				INDICESTYPE v0i = z * size + x;
				INDICESTYPE v1i = v0i - size;

				if ((z != 2) && (x == size - 1))
					indices_.push_back(v0i + verticalIs);

				indices_.push_back(v0i + verticalIs);
				indices_.push_back(v1i + verticalIs);

				if ((z != 1) && (x == 0))
					indices_.push_back(v1i + verticalIs);

			}
		}

	}

	void FixUp::initOffset(int cmapsize)
	{
		short m = (cmapsize + 1) / 4;
		local_offset_[0] = V2i(2 * m - 2, 0);//vertical 0
		local_offset_[1] = V2i(2 * m - 2, 3 * m - 1);//vertical 1
		local_offset_[2] = V2i(0, 2 * m - 2); //Horizontal 0
		local_offset_[3] = V2i(3 * m - 1, 2 * m - 2);//Horizontal 1
	}

	math::V2i FixUp::local_offset_[4];

	void InteriorTrim::initGeometry(int size)
	{
		////vertical
		int verticalSize = 2 * size + 1;
		for (int x = 0; x < 2; x++)
		{
			for (int z = 0; z < verticalSize; z++)
			{
				TerrVertex vertex;
				vertex.position_ = V3f(x, 0.0, z);
				vertex.color_ = V3f(0.5, 1.0, 0.5);
				vertices_.push_back(vertex);
			}
		}
		for (int i = 0; i < verticalSize; i++)
		{
			INDICESTYPE v0i = verticalSize + i;
			INDICESTYPE v1i = i;

			indices_.push_back(v0i);
			indices_.push_back(v1i);

		}

		num_vertices_indices_ = indices_.size();

		int versize = vertices_.size();

		//Horizontal
		verticalSize -= 1;
		for (int x = 0; x < verticalSize; x++)
		{
			TerrVertex vertex;
			vertex.position_ = V3f(x, 0.0, 0);
			vertex.color_ = V3f(0.5, 1.0, 0.5);
			vertices_.push_back(vertex);
			vertex.position_ = V3f(x, 0.0, 1.0);
			vertices_.push_back(vertex);
		}
		for (int i = 0; i < verticalSize; i++)
		{
			INDICESTYPE v0i = 2 * i;
			INDICESTYPE v1i = 2 * i + 1;

			indices_.push_back(v0i + versize);
			indices_.push_back(v1i + versize);
		}
	}

	void InteriorTrim::initOffset(int cmapsize)
	{
		/************************************************************************/
		/*
		|
		|							  |			 |-------------        --------------|
		|							  |			 |									 |
		|							  |			 |									 |
		|							  |			 |									 |
		-------------    -------------|			 |									 |

		0                    1                        2                          3
		*/
		/************************************************************************/

		//referce to the top left
		short m = (cmapsize + 1) / 4;
		local_offset_[0] = V4i(m - 1, m - 1, m, 3 * m - 2);
		local_offset_[1] = V4i(3 * m - 2, m - 1, m - 1, 3 * m - 2);
		local_offset_[2] = V4i(m - 1, m - 1, m, m - 1);
		local_offset_[3] = V4i(3 * m - 2, m - 1, m - 1, m - 1);
	}

	math::V4i InteriorTrim::local_offset_[4];

	TerrainElement::~TerrainElement()
	{
		destroryGlObj();
	}

	void TerrainElement::init(int m)
	{
		initGeometry(m);
		initGlObj();
	}

	void TerrainElement::initGeometry(int size)
	{

	}

	void TerrainElement::pushGLData()
	{

	}

	void OutDegenerateTriangles::initGeometry(int size)
	{
		int cmapSize = size * 4 - 1;
		for (int x = 0; x < cmapSize; x++)
		{
			TerrVertex vertex;
			vertex.position_ = V3f(x, 0.0, 0.0);
			vertex.color_ = V3f(1.0, 0.0, 0.0);
			vertices_.push_back(vertex);
		}

		for (int z = 0; z < cmapSize; z++)
		{
			TerrVertex vertex;
			vertex.position_ = V3f(cmapSize - 1, 0.0, z);
			vertex.color_ = V3f(0.0, 1.0, 0.0);
			vertices_.push_back(vertex);
		}

		for (int x = cmapSize - 1; x > -1; x--)
		{
			TerrVertex vertex;
			vertex.position_ = V3f(x, 0.0, cmapSize - 1);
			vertex.color_ = V3f(0.0, 0.0, 1.0);
			vertices_.push_back(vertex);
		}

		for (int z = cmapSize - 1; z > -1; z--)
		{
			TerrVertex vertex;
			vertex.position_ = V3f(0.0, 0.0, z);
			vertex.color_ = V3f(1.0, 1.0, 1.0);
			vertices_.push_back(vertex);
		}

		int numVertices = vertices_.size();

		for (int i = 1; i < cmapSize - 1; i += 2)
		{
			INDICESTYPE v0i = i;
			indices_.push_back(v0i - 1);
			indices_.push_back(v0i);
			indices_.push_back(v0i + 1);
		}

		for (int i = cmapSize + 1; i < 2 * cmapSize - 1; i += 2)
		{
			INDICESTYPE v0i = i;
			indices_.push_back(v0i - 1);
			indices_.push_back(v0i);
			indices_.push_back(v0i + 1);
		}

		for (int i = 2 * cmapSize + 1; i < 3 * cmapSize - 1; i += 2)
		{
			INDICESTYPE v0i = i;
			indices_.push_back(v0i - 1);
			indices_.push_back(v0i);
			indices_.push_back(v0i + 1);
		}

		for (int i = 3 * cmapSize + 1; i < 4 * cmapSize - 1; i += 2)
		{
			INDICESTYPE v0i = i;
			indices_.push_back(v0i - 1);
			indices_.push_back(v0i);
			indices_.push_back(v0i + 1);
		}
	}

	math::V2i OutDegenerateTriangles::local_offset_[4];

	void FinerEstCenter::initGeometry(int size)
	{
		int verticalSize = 4 * size - 1;

		for (int z = 0; z < verticalSize; z++)
		{
			for (int x = 0; x < verticalSize; x++)
			{
				TerrVertex vertex;
				vertex.position_ = V3f(x, 0.0, z);
				vertex.color_ = V3f(1.0, 1.0, 0.5);
				vertices_.push_back(vertex);
			}
		}

		for (int z = 0; z < verticalSize; z++)
		{
			for (int x = 0; x < verticalSize; x++)
			{
				INDICESTYPE v0i = z * verticalSize + x;
				INDICESTYPE v1i = v0i + verticalSize;

				if ((x == 0) && (z != 0))
					indices_.push_back(v0i);

				indices_.push_back(v0i);
				indices_.push_back(v1i);

				if ((x == verticalSize - 1) && (z != verticalSize - 2))
					indices_.push_back(v1i);

			}

		}
	}

}