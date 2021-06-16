#include "terrainRasterLevel.h"

namespace scene
{
	TerrainRasterLevel::TerrainRasterLevel()
	{
	}

	TerrainRasterLevel::~TerrainRasterLevel()
	{
	}
	
	void TerrainRasterLevel::setLevel(int level)
	{
		level_ = level;
	}


	int TerrainRasterLevel::level() const
	{
		return level_;
	}

	void TerrainRasterLevel::setRasterLevel(RasterLevel*level)
	{
		raster_ = level;
	}

	scene::RasterLevel* TerrainRasterLevel::getRaster()
	{
		return raster_;
	}

	void TerrainRasterLevel::setFiner( TerrainRasterLevel*finer)
	{
		finerLevel_ = finer;
	}

	void TerrainRasterLevel::setCoarser( TerrainRasterLevel*coarser)
	{
		coarserLevel_ = coarser;
	}

	TerrainRasterLevel* TerrainRasterLevel::finerLevel() const
	{
		return finerLevel_;
	}

	TerrainRasterLevel* TerrainRasterLevel::coarserLevel() const
	{
		return coarserLevel_;
	}
}

