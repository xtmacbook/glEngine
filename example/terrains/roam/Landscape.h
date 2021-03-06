
#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include "Patch.h"

// ---------------------------------------------------------------------
// Various Pre-Defined map sizes & their #define counterparts:

#define MAP_1024

#ifdef MAP_2048

// ------- 2048x2048 MAP -------
#define MAP_SIZE (2048)
#define NUM_PATCHES_PER_SIDE (32)

#else
#ifdef MAP_1024

// ------- 1024x1024 MAP -------
#define MAP_SIZE (256)
#define NUM_PATCHES_PER_SIDE (16)

#else

// ------- 512x512 MAP -------
#define MAP_SIZE (512)
#define NUM_PATCHES_PER_SIDE (8)

#endif
#endif

// ---------------------------------------------------------------------
// Scale of the terrain ie: 1 unit of the height map == how many world units (meters)?
// 1.0f == 1 meter resolution
// 0.5f == 1/2 meter resolution
// 0.25f == 1/4 meter resolution
// etc..
#define MULT_SCALE (0.5f)

// How many TriTreeNodes should be allocated?
#define POOL_SIZE (25000)

// Some more definitions
#define PATCH_SIZE (MAP_SIZE/NUM_PATCHES_PER_SIDE)
#define TEXTURE_SIZE (128)

// Drawing Modes
#define DRAW_USE_TEXTURE   (0)
#define DRAW_USE_LIGHTING  (1)
#define DRAW_USE_FILL_ONLY (2)
#define DRAW_USE_WIREFRAME (3)

// Rotation Indexes
#define ROTATE_PITCH (0)
#define ROTATE_YAW   (1)
#define ROTATE_ROLL	 (2)


#define SQR(x) ((x) * (x))
#define MAX(a,b) ((a < b) ? (b) : (a))
#define DEG2RAD(a) (((a) * M_PI) / 180.0f)
#define M_PI (3.14159265358979323846f)

// External variables and functions:
extern GLuint gTextureID;
extern int gDrawMode;
extern float gFrameVariance;
extern int gDesiredTris;
extern int gNumTrisRendered;

extern void calcNormal(float v[3][3], float out[3]);
extern void ReduceToUnit(float vector[3]);

//
// Landscape Class
// Holds all the information to render an entire landscape.
//
class CameraBase;
class Landscape
{
protected:
	unsigned char *m_HeightMap;										// HeightMap of the Landscape
	Patch m_Patches[NUM_PATCHES_PER_SIDE][NUM_PATCHES_PER_SIDE];	// Array of patches

	static int	m_NextTriNode;										// Index to next free TriTreeNode
	static TriTreeNode m_TriPool[POOL_SIZE];						// Pool of TriTree nodes for splitting

	static int GetNextTriNode() { return m_NextTriNode; }
	static void SetNextTriNode( int nNextNode ) { m_NextTriNode = nNextNode; }

public:
	static TriTreeNode *AllocateTri();

	virtual void Init(unsigned char *hMap);
	virtual void Reset(const CameraBase*);
	virtual void Tessellate(const CameraBase*);
	virtual void Render(const CameraBase*);

};


#endif
