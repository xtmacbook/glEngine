#include "terrain.h"
#include "log.h"
#include <gls.h>

#include <engineLoad.h>
#include <resource.h>
#include <stdio.h>
#include <string>

float fDetail = 0.05f;
float roughness = 1.5f;

Cterrain::Cterrain()
{

}


Cterrain::~Cterrain()
{

}

void Cterrain::initHightMap()
{
	if (0) //file
	{
		std::string heightFile = get_media_BasePath() + "heightmap/height128.raw";
		setHeightScale(0.05f);
		loadHeightMap(heightFile.c_str(), 128);
	}
	if (1) // fault
	{
		makeTerrainFault(513, 64, 0, 255, fDetail);;
		setHeightScale(0.15f);
	}
	if (0) // plasm
	{
		makeTerrainPlasma(128, roughness);
		setHeightScale(0.25f);
	}
}

void Cterrain::initTexture()
{
	loadTile(LOWEST_TILE, "terrain/lowestTile.tga");
	loadTile(LOW_TILE, "terrain/lowTile.tga");
	loadTile(HIGH_TILE, "terrain/highTile.tga");
	loadTile(HIGHEST_TILE, "terrain/highestTile.tga");

	generateTextureMap(256);
}

bool Cterrain::loadHeightMap(const char* szFilename, int iSize)
{
	FILE* pFile;

	//check to see if the data has been set
	if (m_heightData.m_ucpData)
		unloadHeightMap();

	//open the RAW height map dataset
	pFile = fopen(szFilename, "rb");
	if (pFile == NULL)
	{
		std::string error = "Could not load " + std::string(szFilename);
		Log::printMessage(error.c_str());
		return false;
	}

	m_heightData.m_ucpData = new unsigned char[iSize*iSize];

	if (m_heightData.m_ucpData == NULL)
	{
		std::string error = "Could not allocate memory for: " + std::string(szFilename);
		Log::printMessage(error.c_str());
		return false;
	}

	//read the heightmap into context
	fread(m_heightData.m_ucpData, 1, iSize*iSize, pFile);

	//Close the file
	fclose(pFile);

	//set the m_iSize data
	m_iSize = iSize;

	std::string msg = "Load success hightmap: " + std::string(szFilename);
	Log::printMessage(msg.c_str());
	return true;
}

bool Cterrain::saveHeightMap(char* szFilename)
{
	return true;
}

bool Cterrain::unloadHeightMap(void)
{
	return true;
}

bool Cterrain::makeTerrainFault(int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter)
{
	float* fTempBuffer;
	int iCurrentIteration;
	int iHeight;
	int iRandX1, iRandZ1;
	int iRandX2, iRandZ2;
	int iDirX1, iDirZ1;
	int iDirX2, iDirZ2;
	int x, z;
	int i;

	if (m_heightData.m_ucpData)
		unloadHeightMap();

	m_iSize = iSize;

	m_heightData.m_ucpData = new unsigned char[m_iSize*m_iSize];
	fTempBuffer = new float[m_iSize*m_iSize];

	if (m_heightData.m_ucpData == NULL)
	{
		Log::printMessage("Could not allocate memory for height map!");
		return false;
	}

	if (fTempBuffer == NULL)
	{
		Log::printMessage("Could not allocate memory for height map!");
		return false;
	}

	//clear the height fTempBuffer
	for (i = 0; i < m_iSize*m_iSize; i++)
		fTempBuffer[i] = 0;

	for (iCurrentIteration = 0; iCurrentIteration < iIterations; iCurrentIteration++)
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		iHeight = iMaxDelta - ((iMaxDelta - iMinDelta)*iCurrentIteration) / iIterations;

		//pick two points at random from the entire height map
		iRandX1 = rand() % m_iSize;
		iRandZ1 = rand() % m_iSize;

		//check to make sure that the points are not the same
		do
		{
			iRandX2 = rand() % m_iSize;
			iRandZ2 = rand() % m_iSize;
		} while (iRandX2 == iRandX1 && iRandZ2 == iRandZ1);


		//iDirX1, iDirZ1 is a vector going the same direction as the line
		iDirX1 = iRandX2 - iRandX1;
		iDirZ1 = iRandZ2 - iRandZ1;

		for (z = 0; z < m_iSize; z++)
		{
			for (x = 0; x < m_iSize; x++)
			{
				//iDirX2, iDirZ2 is a vector from iRandX1, iRandZ1 to the current point (in the loop)
				iDirX2 = x - iRandX1;
				iDirZ2 = z - iRandZ1;

				//if the result of ( iDirX2*iDirZ1 - iDirX1*iDirZ2 ) is "up" (above 0), 
				//then raise this point by iHeight
				if ((iDirX2*iDirZ1 - iDirX1*iDirZ2) > 0)
					fTempBuffer[(z*m_iSize) + x] += (float)iHeight;
			}
		}

		//erode terrain
		filterHeightField(fTempBuffer, fFilter);
	}

	normalizeTerrain(fTempBuffer);

	for (z = 0; z < m_iSize; z++)
	{
		for (x = 0; x < m_iSize; x++)
			setHeightAtPoint((unsigned char)fTempBuffer[(z*m_iSize) + x], x, z);
	}

	if (fTempBuffer)
	{
		delete[] fTempBuffer;
	}

	return true;
}

bool Cterrain::makeTerrainPlasma(int iSize, float fRoughness)
{
	float* fTempBuffer;
	float fHeight, fHeightReducer;
	int iRectSize = iSize;
	int ni, nj;
	int mi, mj;
	int pmi, pmj;
	int i, j;
	int x, z;

	if (m_heightData.m_ucpData)
		unloadHeightMap();

	if (fRoughness<0)
		fRoughness *= -1;

	fHeight = (float)iRectSize / 2;
	fHeightReducer = (float)pow(2, -1 * fRoughness);

	m_iSize = iSize;

	//allocate the memory for our height data
	m_heightData.m_ucpData = new unsigned char[m_iSize*m_iSize];
	fTempBuffer = new float[m_iSize*m_iSize];

	//check to see if memory was successfully allocated
	if (m_heightData.m_ucpData == NULL)
	{
		Log::printMessage("Could not allocate memory for height map!");
		return false;
	}

	//check to see if memory was successfully allocated
	if (fTempBuffer == NULL)
	{
		Log::printMessage("Could not allocate memory for height map!");
		return false;
	}

	//set the first value in the height field
	fTempBuffer[0] = 0.0f;

	//being the displacement process
	while (iRectSize>0)
	{
		/*Diamond step -

		Find the values at the center of the retangles by averaging the values at
		the corners and adding a random offset:


		a.....b
		.     .
		.  e  .
		.     .
		c.....d

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)   */
		for (i = 0; i<m_iSize; i += iRectSize)
		{
			for (j = 0; j<m_iSize; j += iRectSize)
			{
				ni = (i + iRectSize) % m_iSize;
				nj = (j + iRectSize) % m_iSize;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				fTempBuffer[mi + mj*m_iSize] = (float)((fTempBuffer[i + j*m_iSize] + fTempBuffer[ni + j*m_iSize] + fTempBuffer[i + nj*m_iSize] + fTempBuffer[ni + nj*m_iSize]) / 4 + rangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		/*Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		so we don't need to calculate them

		The height m_heightData.m_ucpData wraps, so we're never left hanging.  The right side of the last
		rectangle in a row is the left side of the first rectangle in the row.  The bottom
		side of the last rectangle in a column is the top side of the first rectangle in
		the column

		.......
		.     .
		.     .
		.  d  .
		.     .
		.     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (mi,pmj)
		e = (pmi,mj)
		f = (mi,mj)
		g = (mi,j)
		h = (i,mj)*/
		for (i = 0; i<m_iSize; i += iRectSize)
		{
			for (j = 0; j<m_iSize; j += iRectSize)
			{

				ni = (i + iRectSize) % m_iSize;
				nj = (j + iRectSize) % m_iSize;

				mi = (i + iRectSize / 2);
				mj = (j + iRectSize / 2);

				pmi = (i - iRectSize / 2 + m_iSize) % m_iSize;
				pmj = (j - iRectSize / 2 + m_iSize) % m_iSize;

				//Calculate the square value for the top side of the rectangle
				fTempBuffer[mi + j*m_iSize] = (float)((fTempBuffer[i + j*m_iSize] +
					fTempBuffer[ni + j*m_iSize] +
					fTempBuffer[mi + pmj*m_iSize] +
					fTempBuffer[mi + mj*m_iSize]) / 4 +
					rangedRandom(-fHeight / 2, fHeight / 2));

				//Calculate the square value for the left side of the rectangle
				fTempBuffer[i + mj*m_iSize] = (float)((fTempBuffer[i + j*m_iSize] +
					fTempBuffer[i + nj*m_iSize] +
					fTempBuffer[pmi + mj*m_iSize] +
					fTempBuffer[mi + mj*m_iSize]) / 4 +
					rangedRandom(-fHeight / 2, fHeight / 2));
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		iRectSize /= 2;

		//reduce the height by the height reducer
		fHeight *= fHeightReducer;
	}

	//normalize the terrain for our purposes
	normalizeTerrain(fTempBuffer);

	//transfer the terrain into our class's unsigned char height buffer
	for (z = 0; z<m_iSize; z++)
	{
		for (x = 0; x<m_iSize; x++)
			setHeightAtPoint((unsigned char)fTempBuffer[(z*m_iSize) + x], x, z);
	}

	//delete temporary buffer
	if (fTempBuffer)
	{
		//delete the data
		delete[] fTempBuffer;
	}
}

void Cterrain::normalizeTerrain(float* fpHeightData)
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin = fpHeightData[0];
	fMax = fpHeightData[0];

	//find the min/max values of the height fTempBuffer
	for (i = 1; i < m_iSize*m_iSize; i++)
	{
		if (fpHeightData[i] > fMax)
			fMax = fpHeightData[i];

		else if (fpHeightData[i] < fMin)
			fMin = fpHeightData[i];
	}

	//find the range of the altitude
	if (fMax <= fMin)
		return;

	fHeight = fMax - fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for (i = 0; i < m_iSize*m_iSize; i++)
		fpHeightData[i] = ((fpHeightData[i] - fMin) / fHeight)*255.0f;
}

void Cterrain::filterHeightBand(float* fpBand, int iStride, int iCount, float fFilter)
{
	float v = fpBand[0];
	int j = iStride;
	int i;

	//go through the height band and apply the erosion filter
	for (i = 0; i < iCount - 1; i++)
	{
		fpBand[j] = fFilter*v + (1 - fFilter)*fpBand[j];

		v = fpBand[j];
		j += iStride;
	}
}

void Cterrain::filterHeightField(float* fpHeightData, float fFilter)
{
	int i;

	//erode left to right
	for (i = 0; i < m_iSize; i++)
		filterHeightBand(&fpHeightData[m_iSize*i], 1, m_iSize, fFilter);

	//erode right to left
	for (i = 0; i < m_iSize; i++)
		filterHeightBand(&fpHeightData[m_iSize*i + m_iSize - 1], -1, m_iSize, fFilter);

	//erode top to bottom
	for (i = 0; i < m_iSize; i++)
		filterHeightBand(&fpHeightData[i], m_iSize, m_iSize, fFilter);

	//erode from bottom to top
	for (i = 0; i < m_iSize; i++)
		filterHeightBand(&fpHeightData[m_iSize*(m_iSize - 1) + i], -m_iSize, m_iSize, fFilter);
}

void Cterrain::getTexCoords(Image_SP texture, unsigned int* x, unsigned int* y)
{
	unsigned int uiWidth = texture->width();
	unsigned int uiHeight = texture->height();
	int iRepeatX = -1;
	int iRepeatY = -1;
	int i = 0;

	//loop until we figure out how many times the tile has repeated (on the X axis)
	while (iRepeatX == -1)
	{
		i++;

		//if x is less than the total width, then we found a winner!
		if (*x < (uiWidth*i))
			iRepeatX = i - 1;
	}

	//prepare to figure out the repetition on the Y axis
	i = 0;

	//loop until we figure out how many times the tile has repeated (on the Y axis)
	while (iRepeatY == -1)
	{
		i++;

		//if y is less than the total height, then we have a bingo!
		if (*y < (uiHeight*i))
			iRepeatY = i - 1;
	}

	//update the given texture coordinates
	*x = *x - (uiWidth*iRepeatX);
	*y = *y - (uiHeight*iRepeatY);
}

float Cterrain::regionPercent(int tileType, unsigned char ucHeight)
{
	float fTemp1, fTemp2;
	int t = ucHeight;
	//if the height is lower than the lowest tile's height, then we want full brightness,
	//if we don't do this, the area will get darkened, and no texture will get shown
	if (m_tiles.textureTiles[LOWEST_TILE].addr() && m_tiles.textureTiles[LOWEST_TILE]->isValid())
	{
		if (tileType == LOWEST_TILE && ucHeight < m_tiles.m_regions[LOWEST_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.textureTiles[LOW_TILE].addr() &&  m_tiles.textureTiles[LOW_TILE]->isValid())
	{
		if (tileType == LOW_TILE && ucHeight < m_tiles.m_regions[LOW_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.textureTiles[HIGH_TILE].addr() &&  m_tiles.textureTiles[HIGH_TILE]->isValid())
	{
		if (tileType == HIGH_TILE && ucHeight < m_tiles.m_regions[HIGH_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.textureTiles[HIGHEST_TILE].addr() &&  m_tiles.textureTiles[HIGHEST_TILE]->isValid())
	{
		if (tileType == HIGHEST_TILE && ucHeight < m_tiles.m_regions[HIGHEST_TILE].m_iOptimalHeight)
			return 1.0f;
	}

	//height is lower than the region's boundary
	if (ucHeight < m_tiles.m_regions[tileType].m_iLowHeight)
		return 0.0f;

	//height is higher than the region's boundary
	else if (ucHeight > m_tiles.m_regions[tileType].m_iHighHeight)
		return 0.0f;

	//height is below the optimum height
	if (ucHeight < m_tiles.m_regions[tileType].m_iOptimalHeight)
	{
		//calculate the texture percentage for the given tile's region
		fTemp1 = (float)ucHeight - m_tiles.m_regions[tileType].m_iLowHeight;
		fTemp2 = (float)m_tiles.m_regions[tileType].m_iOptimalHeight - m_tiles.m_regions[tileType].m_iLowHeight;

		return (fTemp1 / fTemp2);
	}

	//height is exactly the same as the optimal height
	else if (ucHeight == m_tiles.m_regions[tileType].m_iOptimalHeight)
		return 1.0f;

	//height is above the optimal height
	else if (ucHeight > m_tiles.m_regions[tileType].m_iOptimalHeight)
	{
		//calculate the texture percentage for the given tile's region
		fTemp1 = (float)m_tiles.m_regions[tileType].m_iHighHeight - m_tiles.m_regions[tileType].m_iOptimalHeight;

		return ((fTemp1 - (ucHeight - m_tiles.m_regions[tileType].m_iOptimalHeight)) / fTemp1);
	}

	//something is seriously wrong if the height doesn't fit the previous cases
	return 0.0f;
}

unsigned char Cterrain::interpolateHeight(int x, int z, float fHeightToTexRatio)
{
	unsigned char ucLow, ucHighX, ucHighZ;
	float ucX, ucZ;
	float fScaledX = x*fHeightToTexRatio;
	float fScaledZ = z*fHeightToTexRatio;
	float fInterpolation;

	//set the middle boundary
	ucLow = getTrueHeightAtPoint((int)fScaledX, (int)fScaledZ);

	//start off by interpolating along the X axis
	//set the high boundary
	if ((fScaledX + 1) > m_iSize)
		return ucLow;
	else
		ucHighX = getTrueHeightAtPoint((int)fScaledX + 1, (int)fScaledZ);

	//calculate the interpolation (for the X axis)
	fInterpolation = (fScaledX - (int)fScaledX);
	ucX = ((ucHighX - ucLow)*fInterpolation) + ucLow;

	//interpolate along the Z axis now
	//set the high boundary
	if ((fScaledZ + 1) > m_iSize)
		return ucLow;
	else
		ucHighZ = getTrueHeightAtPoint((int)fScaledX, (int)fScaledZ + 1);

	//calculate the interpolation (for the Z axis)
	fInterpolation = (fScaledZ - (int)fScaledZ);
	ucZ = ((ucHighZ - ucLow)*fInterpolation) + ucLow;

	//calculate the overall interpolation (average of the two values)
	return ((unsigned char)((ucX + ucZ) / 2));
}

unsigned char limit(unsigned char ucValue)
{
	if (ucValue > 254)
		return 254;
	else if (ucValue < 0)
		return 0;

	return ucValue;
}

void Cterrain::generateTextureMap(unsigned int uiSize)
{
	uint8 ucRed, ucGreen, ucBlue;
	unsigned int iTempID;
	unsigned int x, z;
	unsigned int uiTexX, uiTexZ;
	float fTotalRed, fTotalGreen, fTotalBlue;
	float fBlend[4];
	float fMapRatio;
	int iLastHeight;
	int i;

	//find out the number of tiles that we have
	m_tiles.iNumTiles = 0;
	for (i = 0; i < TRN_NUM_TILES; i++)
	{
		//if the current tile is loaded, then we add one to the total tile count
		if (m_tiles.textureTiles[i].addr() && m_tiles.textureTiles[i]->isValid())
			m_tiles.iNumTiles++;
	}

	//now, re-loop through, and calculate the texture regions
	iLastHeight = -1;
	for (i = 0; i < TRN_NUM_TILES; i++)
	{
		//we only want to perform these calculations if we actually have a tile loaded
		if (m_tiles.textureTiles[i].addr() && m_tiles.textureTiles[i]->isValid())
		{
			//calculate the three height boundaries
			m_tiles.m_regions[i].m_iLowHeight = iLastHeight + 1;
			iLastHeight += 255 / m_tiles.iNumTiles;

			m_tiles.m_regions[i].m_iOptimalHeight = iLastHeight;

			m_tiles.m_regions[i].m_iHighHeight = (iLastHeight - m_tiles.m_regions[i].m_iLowHeight) + iLastHeight;
		}
	}

	Image_SP img = new Image;
	img->allocate(uiSize, uiSize, 24 / 8);
	img->levelDataPtr_ = new uint8*[1];
	img->levelDataPtr_[0] = (uint8*)img->pixels();
	img->settarget(GL_TEXTURE_2D);
	img->setinternalformat(GL_RGB8);
	img->settype(GL_UNSIGNED_BYTE);
	img->setformat(GL_RGB);
	img->settypesize(0);
	img->setfaces(1);

	//get the height map to texture map ratio (since, most of the time,
	//the texture map will be a higher resolution than the height map, so
	//we need the ratio of height map pixels to texture map pixels)
	fMapRatio = (float)m_iSize / uiSize;

	//time to create the texture data
	for (z = 0; z < uiSize; z++)
	{
		for (x = 0; x < uiSize; x++)
		{
			//set our total color counters to 0.0f
			fTotalRed = 0.0f;
			fTotalGreen = 0.0f;
			fTotalBlue = 0.0f;

			//loop through the tiles (for the third time in this function!)
			for (i = 0; i < TRN_NUM_TILES; i++)
			{
				//if the tile is loaded, we can do the calculations
				if (m_tiles.textureTiles[i].addr() && m_tiles.textureTiles[i]->isValid())
				{
					uiTexX = x;
					uiTexZ = z;

					//get texture coordinates
					getTexCoords(m_tiles.textureTiles[i], &uiTexX, &uiTexZ);

					//get the current color in the texture at the coordinates that we got
					//in GetTexCoords
					m_tiles.textureTiles[i]->getVal(uiTexX, uiTexZ, &ucRed, &ucGreen, &ucBlue);

					//get the current coordinate's blending percentage for this tile
					fBlend[i] = regionPercent(i, interpolateHeight(x, z, fMapRatio));

					//calculate the RGB values that will be used
					fTotalRed += ucRed*fBlend[i];
					fTotalGreen += ucGreen*fBlend[i];
					fTotalBlue += ucBlue*fBlend[i];
				}
			}

			//set our terrain's texture color to the one that we previously calculated
			uint8 r = limit((unsigned char)fTotalRed);
			uint8 g = limit((unsigned char)fTotalGreen);
			uint8 b = limit((unsigned char)fTotalBlue);

		 	img->setVal(x, z, &r, &g, &b);
		}
	}

	m_texture = new Texture(img);
	m_texture->createObj();
	m_texture->bind();
	m_texture->mirrorRepeat();
	m_texture->filterLinear();
	if (m_texture->context(NULL))
	{
		m_texture->unBind();
	}
}

void Cterrain::bindTexture()
{
	m_texture->bind();
}

bool Cterrain::loadTile(ETILE_TYPES tileType, const char* szFilename)
{
	Image_SP img  = IO::EngineLoad::loadImage(szFilename);
	if (img)
	{
		m_tiles.textureTiles[tileType] = img;
		return true;
	}
	return false;
}

