 
#include <stdio.h>
#include "brute_force.h"
#include "common.h"
#include <mesh.h>
#include <resource.h>
#include <shader.h>
#include <gls.h>



void Cbrute_force_terrain::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getTexture());
}

MeshGeometry_Sp Cbrute_force_terrain::initGeometry()
{
	float fTexLeft, fTexBottom, fTexTop;

	base::SmartPointer<MeshGeometry> mg = new MeshGeometry;
	for (int iZ = 0; iZ < m_iSize - 1; iZ++)
	{
		//loop through the X-axis of the terrain
		//this is where the triangle strip is constructed
		Mesh_SP mesh = new Mesh;
		mesh->rmode() = GL_TRIANGLE_STRIP;
		mesh->RFVF() |= FVF_XYZ;
		mesh->RFVF() |= FVF_NORMAL;
		mesh->RFVF() |= FVF_TEXT0;

		for (int iX = 0; iX < m_iSize - 1; iX++)
		{
			//calculate the texture coordinates
			fTexLeft = (float)iX / m_iSize;
			fTexBottom = (float)iZ / m_iSize;
			fTexTop = (float)(iZ + 1) / m_iSize;

			Vertex vertex0;
			//use height-based coloring (high-points are
			//light, low points are dark)
			unsigned char ucColor = getTrueHeightAtPoint(iX, iZ);

			vertex0.Normal = V3f(ucColor,ucColor,ucColor);
			vertex0.Position = V3f((float)iX, getScaledHeightAtPoint(iX, iZ), (float)iZ);
			vertex0.TexCoords = V2f(fTexLeft, fTexBottom);

			Vertex vertex1;
			ucColor = getTrueHeightAtPoint(iX, iZ + 1);
			vertex1.Normal = V3f(ucColor, ucColor, ucColor);
			vertex1.Position = V3f((float)iX, getScaledHeightAtPoint(iX, iZ + 1), (float)iZ + 1);
			vertex1.TexCoords = V2f(fTexLeft, fTexTop);

			mesh->addVertex(vertex0);
			mesh->addVertex(vertex1);

			m_iVertsPerFrame += 2;

			//there are no triangles being rendered on the first X-loop, they just start the
			//triangle strip off
			if (iX != 0)
				m_iTrisPerFrame += 2;
		}
		mesh->computeBox();

		mg->addMesh(mesh);
	}
	mg->initGeometry();
	return mg;
}

