
#ifndef _GEO_MIP_MAPPING_
#define _GEO_MIP_MAPPING_

#include "terrain.h"

struct Sgeomm_patch
{
	float m_fDistance;
	int   m_iLOD;
	bool  m_bVisible;
};

struct Sgeomm_neighbor
{
	bool m_bLeft;
	bool m_bUp;
	bool m_bRight;
	bool m_bDown;
};

/**
 * The geomipmapping implementation is based on a 2N +1 pixel squareheight map.This means that you can¡¯t use the midpoint displacement
fractal height map generator to generate heightmaps. Stick with the fault formation generator for all of your heightmaps.
 */

 // patch size is 17 x 17 my suggestion of 17 ¡Á 17 vertices

class CameraBase;
class Cgeomipmapping :public Cterrain
{
public:

	bool					init(int patchSize);
	virtual MeshGeometry_Sp initGeometry();
	void					updateGeometry(CameraBase*, CommonGeometry_Sp);
	virtual void			render() ;

	inline int				getPatchNumber(int PX, int PZ)
	{
		return ((PZ*m_iNumPatchesPerSide) + PX);
	}

private:
	void					updatePatchLod(const CameraBase*,bool bCullPatches);
	void					updatePatch(int x, int z, Mesh_SP, V3f color);
	void					updateFan(float cX, float cZ, float fSize, Sgeomm_neighbor neighbor, Mesh_SP, V3f color);
private:
	Sgeomm_patch* m_pPatches;
	int			  m_iPatchSize;
	int			  m_iNumPatchesPerSide;
	int			  m_iMaxLOD;
	int			  m_iPatchesPerFrame;	//the number of rendered patches per second

};

#endif
