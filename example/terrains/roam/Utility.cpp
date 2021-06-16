//
// Utility.cpp
// Bryan Turner
//
// Parts of the code in this file were borrowed from numerous public sources &
//   literature.  I reserve NO rights to this code and give a hearty thank-you to all the
//   excellent sources used in this project.  These include, but are not limited to:
//
//   Longbow Digital Arts Programming Forum (www.LongbowDigitalArts.com)
//   Gamasutra Features (www.Gamasutra.com)
//   GameDev References (www.GameDev.net)
//   C. Cookson's ROAM implementation (C.J.Cookson@dcs.warwick.ac.uk OR cjcookson@hotmail.com)
//   OpenGL Super Bible (Waite Group Press)
//   And many more...
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gls.h>
#include "utility.h"
#include "landscape.h"

// Observer and Follower modes
#define FOLLOW_MODE (0)
#define OBSERVE_MODE (1)
#define DRIVE_MODE (2)
#define FLY_MODE (3)

// Perspective & Window defines
#define FOV_ANGLE     (90.0f)
#define NEAR_CLIP     (1.0f)
#define FAR_CLIP      (2500.0f)

// --------------------------------------
// GLOBALS
// --------------------------------------
Landscape gLand;

// Texture
GLuint gTextureID=1;


// Misc. Globals
int gDrawMode   = DRAW_USE_TEXTURE;
int gStartX=-1, gStartY;
int gNumTrisRendered;
long gStartTime, gEndTime;
unsigned char *gHeightMap;
unsigned char *gHeightMaster;
int gNumFrames;

// Beginning frame varience (should be high, it will adjust automatically)
float gFrameVariance = 50;

// Desired number of Binary Triangle tessellations per frame.
// This is not the desired number of triangles rendered!
// There are usually twice as many Binary Triangle structures as there are rendered triangles.
int gDesiredTris = 10000;

// ---------------------------------------------------------------------
// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
//
void ReduceToUnit(float vector[3])
	{
	float length;
	
	// Calculate the length of the vector		
	length = sqrtf((vector[0]*vector[0]) + 
				   (vector[1]*vector[1]) +
				   (vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}

// ---------------------------------------------------------------------
// Points p1, p2, & p3 specified in counter clock-wise order
//
void calcNormal(float v[3][3], float out[3])
	{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
	}

// ---------------------------------------------------------------------
// Load the Height Field from a data file
//
void loadTerrain(int size, const char*fileName, unsigned char **dest)
{
	FILE *fp;

	// Optimization:  Add an extra row above and below the height map.
	//   - The extra top row contains a copy of the last row in the height map.
	//   - The extra bottom row contains a copy of the first row in the height map.
	// This simplifies the wrapping of height values to a trivial case. 
	gHeightMaster = (unsigned char *)malloc( size * size * sizeof(unsigned char) + size * 2 );
	
	// Give the rest of the application a pointer to the actual start of the height map.
	*dest = gHeightMaster + size;

	fp = fopen(fileName, "rb");

	// TESTING: READ A TREAD MARKS MAP...
	if ( fp == NULL )
	{
		exit(0);
	}

	if (fp == NULL)
	{
		// Oops!  Couldn't find the file.
		
		// Clear the board.
		memset( gHeightMaster, 0, size * size + size * 2 );
		return;
	}
	fread(gHeightMaster + size, 1, (size * size), fp);
	fclose(fp);

	// Copy the last row of the height map into the extra first row.
	memcpy( gHeightMaster, gHeightMaster + size * size, size );

	// Copy the first row of the height map into the extra last row.
	memcpy( gHeightMaster + size * size + size, gHeightMaster + size, size );
}

// ---------------------------------------------------------------------
// Free the Height Field array
//
void freeTerrain()
{
	if ( gHeightMaster )
		free( gHeightMaster );
}

// ---------------------------------------------------------------------
// Switch GL Contexts when moving between draw modes to improve performance.
//
void SetDrawModeContext()
{
	switch (gDrawMode)
	{
	case DRAW_USE_TEXTURE:
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case DRAW_USE_LIGHTING:
		glEnable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case DRAW_USE_FILL_ONLY:
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	default:
	case DRAW_USE_WIREFRAME:
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	}
}

// ---------------------------------------------------------------------
// Initialize the ROAM implementation
//
int roamInit(unsigned char *map)
{
	// Perform some bounds checking on the #define statements
	if ( gDesiredTris > POOL_SIZE )
		return -1;

	if ( POOL_SIZE < 100 )
		return -1;

// ----------- TEXTURE INITIALIZATION -----------
	//glBindTexture(GL_TEXTURE_2D, gTextureID);
	//
	//unsigned char *pTexture = (unsigned char *)malloc(TEXTURE_SIZE*TEXTURE_SIZE*3);
	//unsigned char *pTexWalk = pTexture;

	//if ( !pTexture )
	//	exit(0);

	//// Create a random stipple pattern for the texture.  Only use the Green channel.
	//// This could easily be modified to load in a bitmap or other texture source.
	//for ( int x = 0; x < TEXTURE_SIZE; x++ )
	//	for ( int y = 0; y < TEXTURE_SIZE; y++ )
	//	{
	//		int color = (int)(128.0+(40.0 * rand())/RAND_MAX);
	//		if ( color > 255 )  color = 255;
	//		if ( color < 0 )    color = 0;
	//		*(pTexWalk++) = 0;
	//		*(pTexWalk++) = color;	// Only use the Green chanel.
	//		*(pTexWalk++) = 0;
	//	}

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGB, GL_UNSIGNED_BYTE, pTexture);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//free(pTexture);

	//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

// ----------- LANDSCAPE INITIALIZATION -----------
	//gLand.Init(map);

	return 0;
}
