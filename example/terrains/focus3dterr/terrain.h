
#ifndef _terrain_h_
#define  _terrain_h_
#include <IGeometry.h>
#include <texture.h>

#define TRN_NUM_TILES 5

enum ETILE_TYPES
{
	LOWEST_TILE = 0,	//sand, dirt, etc.
	LOW_TILE,			//grass
	HIGH_TILE,			//mountainside
	HIGHEST_TILE		//tip of mountain
};

struct Sheight_data
{
	unsigned char* m_ucpData;	//the height data
	int m_iSize;				//the height size (must be a power of 2)
};

struct Strn_texture_regions
{
	int m_iLowHeight;			//lowest possible height (0%)
	int m_iOptimalHeight;		//optimal height (100%)
	int m_iHighHeight;			//highest possible height (0%)
};

struct Strn_texture_tiles
{
	Strn_texture_regions m_regions[TRN_NUM_TILES];	//texture regions
	Image_SP textureTiles[TRN_NUM_TILES];				//texture tiles
	int iNumTiles;
};

class Cterrain
{
public:
	Cterrain();
	virtual ~Cterrain();

	virtual MeshGeometry_Sp initGeometry() = 0;
	virtual void		    initHightMap();
	virtual void		    initTexture();

	virtual void			render() = 0;

	bool loadHeightMap(const char* szFilename, int iSize);
	bool saveHeightMap(char* szFilename);
	bool unloadHeightMap(void);

	bool makeTerrainFault(int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter);
	bool makeTerrainPlasma(int iSize, float fRoughness);
	//fractal terrain generation
	void normalizeTerrain(float* fpHeightData);
	void filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter);
	void filterHeightField(float* fpHeightData, float fFilter);

	//texture map generation
	void getTexCoords(Image_SP texture, unsigned int* x, unsigned int* y);
	float regionPercent(int tileType, unsigned char ucHeight);
	unsigned char interpolateHeight(int x, int z, float fHeightToTexRatio);
	/**
	 * using the terrain's heightmap to generate a texture that will coincide with it.
	 */
	void generateTextureMap(unsigned int uiSize);
	void bindTexture();

	bool loadTile(ETILE_TYPES tileType,const  char* szFilename);

	
	inline int getNumVertsPerFrame(void)
	{
		return m_iVertsPerFrame;
	}

	inline int getNumTrisPerFrame(void)
	{
		return m_iTrisPerFrame;
	}

	inline void setHeightScale(float fScale)
	{
		m_fHeightScale = fScale;
	}

	inline float rangedRandom(float f1, float f2)
	{
		return (f1 + (f2 - f1)*((float)rand()) / ((float)RAND_MAX));
	}

	inline void setHeightAtPoint(unsigned char ucHeight, int iX, int iZ)
	{
		m_heightData.m_ucpData[(iZ*m_iSize) + iX] = ucHeight;
	}

	inline unsigned char getTrueHeightAtPoint(int iX, int iZ)
	{
		return (m_heightData.m_ucpData[(iZ*m_iSize) + iX]);
	}

	inline float getScaledHeightAtPoint(int iX, int iZ)
	{
		float height  = ((float)(m_heightData.m_ucpData[(iZ*m_iSize) + iX])*m_fHeightScale);
		return height;
	}

	int							m_iSize;				//the size of the heightmap, must be a power of two
	Strn_texture_tiles			m_tiles;
	Texture_Sp					m_texture;

protected:
	Sheight_data		m_heightData;			//the height data
	float				m_fHeightScale;			//scaling variable
	int					m_iVertsPerFrame;		//stat variables
	int					m_iTrisPerFrame;
};

#endif
