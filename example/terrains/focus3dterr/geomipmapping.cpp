#include "geomipmapping.h"
#include "dynamicMesh.h"
#include <camera.h>

extern void clearColor();
extern void calPatchColor(int iLOD);
extern V3f getPatchColor(int iLOD);

bool Cgeomipmapping::init(int patchSize)
{
	int x, z;
	int iLOD;
	int iDivisor;
	int iPatch;

	if (m_iSize == 0)
		return false;

	//initiate the patch information
	m_iPatchSize = patchSize;
	m_iNumPatchesPerSide = m_iSize / m_iPatchSize;
	m_pPatches = new Sgeomm_patch[m_iNumPatchesPerSide * m_iNumPatchesPerSide];
	if (m_pPatches == NULL)
		return false;

	//figure out the maximum level of detail for a patch
	iDivisor = m_iPatchSize - 1;
	iLOD = 0;
	while (iDivisor > 2)
	{
		iDivisor = iDivisor >> 1;
		iLOD++;
	}

	//the max amount of detail
	m_iMaxLOD = iLOD;

	//initialize the patch values
	for (z = 0; z < m_iNumPatchesPerSide; z++)
	{
		for (x = 0; x < m_iNumPatchesPerSide; x++)
		{
			iPatch = getPatchNumber(x, z);

			//initialize the patches to the lowest level of detail
			m_pPatches[iPatch].m_iLOD = m_iMaxLOD;
			m_pPatches[iPatch].m_bVisible = true;
		}
	}

	return true;
}

MeshGeometry_Sp Cgeomipmapping::initGeometry()
{
	base::SmartPointer<DynamicMeshGeoemtry> mg = new DynamicMeshGeoemtry(2,1248 * 1248,1248 * 1248);
	mg->RFVF() |= FVF_XYZ;
	mg->RFVF() |= FVF_NORMAL; //for calor
	mg->RFVF() |= FVF_TEXT0;
	mg->initGeometry();

	Mesh_SP mesh = new Mesh;
	mesh->rmode() = GL_TRIANGLE_FAN;
	mesh->RFVF() |= FVF_XYZ;
	mg->RFVF() |= FVF_NORMAL; //for calor
	mesh->RFVF() |= FVF_TEXT0;
	mesh->call_ = DRAW_ARRAYS;
	mg->addMesh(mesh);

	return mg;
}
 
void Cgeomipmapping::updateGeometry(CameraBase*camera, CommonGeometry_Sp geo)
{
	clearColor();

	updatePatchLod(camera, true);

	for (int z = 0; z < m_iNumPatchesPerSide; z++)
		for (int x = 0; x < m_iNumPatchesPerSide; x++)
			calPatchColor(m_pPatches[getPatchNumber(x, z)].m_iLOD);


	DynamicMeshGeoemtry* dGeo = (DynamicMeshGeoemtry*)(geo.addr());
	Mesh_SP mesh = dGeo->meshs_[0];
	mesh->clear();
	int x, z;


	for (z = 0; z < m_iNumPatchesPerSide; z++)
	{
		for (x = 0; x < m_iNumPatchesPerSide; x++)
		{
			int iLOD = m_pPatches[getPatchNumber(x, z)].m_iLOD;
			
			V3f color =  getPatchColor(iLOD);
			if (m_pPatches[getPatchNumber(x, z)].m_bVisible)
			{
				updatePatch(x, z,mesh, color);
				m_iPatchesPerFrame++;
			}
		}
	}
}

void Cgeomipmapping::render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture->getTexture());
}

void Cgeomipmapping::updatePatchLod(const CameraBase*camera,bool bCullPatches)
{
	float fX, fY, fZ;
	float fScaledSize;
	int x, z;
	int iPatch;

	fScaledSize = m_iPatchSize;

	for (z = 0; z < m_iNumPatchesPerSide; z++)
	{
		for (x = 0; x < m_iNumPatchesPerSide; x++)
		{
			iPatch = getPatchNumber(x, z);

			//compute patch center (used for distance determination
			fX = (x*m_iPatchSize) + (m_iPatchSize / 2.0f);
			fZ = (z*m_iPatchSize) + (m_iPatchSize / 2.0f);
			fY = getScaledHeightAtPoint((int)fX, (int)fZ);

			//only scale the X and Z values, the Y value has already been scaled

			//check to see if the user wanted to cull the non-visible patches
			if (bCullPatches)
			{
				//do a frustum test against the patch
				if (camera->cubeFrustumTest(fX, fY, fZ, m_iPatchSize))
					m_pPatches[iPatch].m_bVisible = true;

				//the patch is not visible
				else
					m_pPatches[iPatch].m_bVisible = false;
			}

			//make all patches visible
			else
				m_pPatches[iPatch].m_bVisible = true;

			//only finish updating if the patch is visible
			if (m_pPatches[iPatch].m_bVisible)
			{
				//get the distance from the camera to the patch

				float dis = (fX - camera->getPosition()[0]) * (fX - camera->getPosition()[0]) + (fY - camera->getPosition()[1]) * (fY - camera->getPosition()[1]) + (fZ - camera->getPosition()[2]) * (fZ - camera->getPosition()[2]);

				m_pPatches[iPatch].m_fDistance = 
					sqrtf(dis);

				//BAD way to determine patch LOD, we will be fixing this code a bit later in the chapter
				if (m_pPatches[iPatch].m_fDistance < 20)
					m_pPatches[iPatch].m_iLOD = 0;

				else if (m_pPatches[iPatch].m_fDistance < 30)
					m_pPatches[iPatch].m_iLOD = 1;

				else if (m_pPatches[iPatch].m_fDistance < 40)
					m_pPatches[iPatch].m_iLOD = 2;

				else if (m_pPatches[iPatch].m_fDistance >= 50)
					m_pPatches[iPatch].m_iLOD = 3;
			}
		}
	}
}

void Cgeomipmapping::updatePatch(int PX, int PZ, Mesh_SP mesh,V3f color)
{
	Sgeomm_neighbor patchNeighbor;
	Sgeomm_neighbor fanNeighbor;
	float fSize;
	float fHalfSize;
	float x, z;
	int iPatch = getPatchNumber(PX, PZ);
	int iDivisor;
	int iLOD;

	//find out information about the patch to the current patch's left, if the patch is of a
	//greater detail or there is no patch to the left, we can render the mid-left vertex
	if (m_pPatches[getPatchNumber(PX - 1, PZ)].m_iLOD <= m_pPatches[iPatch].m_iLOD || PX == 0)
		patchNeighbor.m_bLeft = true;
	else
		patchNeighbor.m_bLeft = false;

	//find out about the upper patch
	if (m_pPatches[getPatchNumber(PX, PZ + 1)].m_iLOD <= m_pPatches[iPatch].m_iLOD || PZ == m_iNumPatchesPerSide)
		patchNeighbor.m_bUp = true;
	else
		patchNeighbor.m_bUp = false;

	//find out about the right patch
	if (m_pPatches[getPatchNumber(PX + 1, PZ)].m_iLOD <= m_pPatches[iPatch].m_iLOD || PX == m_iNumPatchesPerSide)
		patchNeighbor.m_bRight = true;
	else
		patchNeighbor.m_bRight = false;

	//find out about the lower patch
	if (m_pPatches[getPatchNumber(PX, PZ - 1)].m_iLOD <= m_pPatches[iPatch].m_iLOD || PZ == 0)
		patchNeighbor.m_bDown = true;
	else
		patchNeighbor.m_bDown = false;

	//we need to determine the distance between each triangle-fan that
	//we will be rendering
	iLOD = m_pPatches[getPatchNumber(PX, PZ)].m_iLOD + 1;
	fSize = (float)m_iPatchSize;
	iDivisor = m_iPatchSize - 1;
	
	
	//find out how many fan divisions we are going to have
	while (--iLOD > -1)
		iDivisor = iDivisor >> 1;

	//the size between the center of each triangle fan
	fSize /= iDivisor;

	//half the size between the center of each triangle fan (this will be
	//the size between each vertex)
	fHalfSize = fSize / 2.0f;
	for (z = fHalfSize; ((int)(z + fHalfSize)) < m_iPatchSize + 1; z += fSize)
	{
		for (x = fHalfSize; ((int)(x + fHalfSize)) < m_iPatchSize + 1; x += fSize)
		{
			//if this fan is in the left row, we may need to adjust it's rendering to
			//prevent cracks
			if (x == fHalfSize)
				fanNeighbor.m_bLeft = patchNeighbor.m_bLeft;
			else
				fanNeighbor.m_bLeft = true;

			//if this fan is in the bottom row, we may need to adjust it's rendering to
			//prevent cracks
			if (z == fHalfSize)
				fanNeighbor.m_bDown = patchNeighbor.m_bDown;
			else
				fanNeighbor.m_bDown = true;

			//if this fan is in the right row, we may need to adjust it's rendering to
			//prevent cracks
			if (x >= (m_iPatchSize - fHalfSize))
				fanNeighbor.m_bRight = patchNeighbor.m_bRight;
			else
				fanNeighbor.m_bRight = true;

			//if this fan is in the top row, we may need to adjust it's rendering to
			//prevent cracks
			if (z >= (m_iPatchSize - fHalfSize))
				fanNeighbor.m_bUp = patchNeighbor.m_bUp;
			else
				fanNeighbor.m_bUp = true;

			//render the triangle fan
			updateFan((PX*m_iPatchSize) + x, (PZ*m_iPatchSize) + z,
				fSize, fanNeighbor,mesh,color);
		}
	}
}

void Cgeomipmapping::updateFan(float cX, float cZ, float fSize, Sgeomm_neighbor neighbor, Mesh_SP mesh, V3f color)
{
	float fTexLeft, fTexBottom, fMidX, fMidZ, fTexRight, fTexTop;
	float fHalfSize = fSize / 2.0f;

	//calculate the texture coordinates
	fTexLeft = ((float)fabs(cX - fHalfSize) / m_iSize);
	fTexBottom = ((float)fabs(cZ - fHalfSize) / m_iSize);
	fTexRight = ((float)fabs(cX + fHalfSize) / m_iSize);
	fTexTop = ((float)fabs(cZ + fHalfSize) / m_iSize);

	fMidX = ((fTexLeft + fTexRight) / 2);
	fMidZ = ((fTexBottom + fTexTop) / 2);

	unsigned int current_vertexNum = mesh->size();
	float x, z;
	Vertex v;

	x = cX; z = cZ;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.Normal = color;
	v.TexCoords = V2f(fMidX, fMidZ);
	mesh->addVertex(current_vertexNum,v);

	//render the LOWER-LEFT vertex
	x = cX - fHalfSize; z = cZ - fHalfSize;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.Normal = color;
	v.TexCoords = V2f(fTexLeft, fTexBottom);
	mesh->addVertex(current_vertexNum,v);

	//only render the next vertex if the left patch is NOT of a lower LOD
	if (neighbor.m_bLeft)
	{
		x = cX - fHalfSize; z = cZ;
		v.Normal = color;
		v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
		v.TexCoords = V2f(fTexLeft, fMidZ);
		mesh->addVertex(current_vertexNum,v);
	}

	//render the UPPER-LEFT vertex
	x = cX - fHalfSize; z = cZ + fHalfSize;
	v.Normal = color;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.TexCoords = V2f(fTexLeft, fTexTop);
	mesh->addVertex(current_vertexNum,v);

	//only render the next vertex if the upper patch is NOT of a lower LOD
	if (neighbor.m_bUp)
	{
		x = cX; z = cZ + fHalfSize;
		v.Normal = color;
		v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
		v.TexCoords = V2f(fMidX, fTexTop);
		mesh->addVertex(current_vertexNum,v);
	}

	//render the UPPER-RIGHT vertex
	x = cX + fHalfSize; z = cZ + fHalfSize;
	v.Normal = color;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.TexCoords = V2f(fTexRight, fTexTop);
	mesh->addVertex(current_vertexNum,v);

	//only render the next vertex if the right patch is NOT of a lower LOD
	if (neighbor.m_bRight)
	{
		//render the MID-RIGHT vertex
		x = cX + fHalfSize; z = cZ ;
		v.Normal = color;
		v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
		v.TexCoords = V2f(fTexRight, fMidZ);
		mesh->addVertex(current_vertexNum,v);
	}

	//render the LOWER-RIGHT vertex
	x = cX + fHalfSize; z = cZ - fHalfSize;
	v.Normal = color;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.TexCoords = V2f(fTexRight, fTexBottom);
	mesh->addVertex(current_vertexNum,v);

	//only render the next vertex if the bottom patch is NOT of a lower LOD
	if (neighbor.m_bDown)
	{
		//render the LOWER-MID vertex
		x = cX; z = cZ - fHalfSize;
		v.Normal = color;
		v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
		v.TexCoords = V2f(fMidX, fTexBottom);
		mesh->addVertex(current_vertexNum,v);
	}

	//render the LOWER-LEFT vertex
	x = cX - fHalfSize; z = cZ - fHalfSize;
	v.Normal = color;
	v.Position = V3f(x, getScaledHeightAtPoint(x, z), z);
	v.TexCoords = V2f(fTexLeft, fTexBottom);
	mesh->addVertex(current_vertexNum,v);
}
