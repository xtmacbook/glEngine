
#include "geoClipMapping.h"
#include  "terrainClipLevel.h"
#include "terrainClipUpdater.h"
#include "rasterSource.h"
#include "rasterLevel.h"

#include "camera.h"
#include "helpF.h"
#include "resource.h"
#include "cameraBase.h"
#include "shader.h"
#include "gls.h"
#include "log.h"

/************************************************************************
 https://arm-software.github.io/opengl-es-sdk-for-android/terrain.html
	https://mikejsavage.co.uk/blog/geometry-clipmaps.html 
	https://github.com/keattikorn/DigitalRune
************************************************************************/
using namespace math;

/*
MapLayerManager
*/
MapLayerManager::MapLayerManager(int cmapSize, int level, RasterResource *r) :cmapSize_(cmapSize),
resource_(r), num_level_(level)
{
}

MapLayerManager::~MapLayerManager()
{
	if (layers_)
	{
		for (int i = 0; i < num_level_ - 1; i++)
		{
			TerrainClipOutLevel * layer = layers_[i];
			delete layer;
		}
		delete[] layers_;
	}
	if (innermostLayer_)
		delete innermostLayer_;
}


void MapLayerManager::setShaderLocations(const RenderShaderIndex*index)
{
	shaderIndex_ = index;
}

/************************************************************************/
/* the terrain plane is xz plane

---------->x
|
|
|
|
|
\ /
z

*/
/************************************************************************/
void MapLayerManager::init()
{
	V3f center(0.0f, 0.0f, 0.0f);

	layers_ = new TerrainClipOutLevel*[num_level_ - 1]; //0->num_level_-1

	for (int i = 0; i < num_level_ - 1; i++)
		layers_[i] = new TerrainClipOutLevel(cmapSize_);

	innermostLayer_ = new TerrainClipInnermostLevel(cmapSize_);

	TerrainClipLevel * layer;
	for (int i = 0; i < num_level_; i++)
	{
		layer = getLevel(i);
		layer->setRasterLevel(resource_->level(i));
		if (i != 0) layer->setFiner(getLevel(i - 1));
		if (i != num_level_ - 1) layer->setCoarser(getLevel(i + 1));
	}

	V2f* level_centers = new V2f[num_level_];
	int sx = (int)floor(center.x);
	int sz = (int)floor(center.z);

	for (int level = 0; level < num_level_; level++)
	{

		layer = getLevel(level);
		layer->setLevel(level);
		layer->setScale(pow(2, level));

		int mod;
		if (level % 2 == 0) mod = 1;
		else mod = -1;

		if (level == 0)
		{
			level_centers[level].x = sx;
			level_centers[level].y = sz;
		}
		else
		{
			int scale = (int)pow(2, level - 1);
			level_centers[level].x = level_centers[level - 1].x + mod *  scale;
			level_centers[level].y = level_centers[level - 1].y - mod *  scale;
		}

		layer->setCenter(level_centers[level]);

		V2f level_off(0.0f, 0.0f);
		if (level != 0)
		{
			level_off.x = 0.5*(level_centers[level].x - level_centers[level - 1].x);
			level_off.y = 0.5*(level_centers[level].y - level_centers[level - 1].y);
		}
		layer->setLevelOffset(level_off);
	}


	for (int level = 0; level < num_level_; level++)
		getLevel(level)->init(center);

	delete[] level_centers;
}

void MapLayerManager::render(Shader*shader, FootPrints * footPrint)
{
	TerrainClipLevel * layer;
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->render(shaderIndex_, shader, footPrint);
	}
}

bool test_first = true;

void MapLayerManager::update(const CameraBase* camera, scene::RasterUpdater * updater)
{
	TerrainClipLevel * layer;

	//udpate center level
	for (int level = 0; level < num_level_; level++)
	{
		int rows_update, cols_update;
		getLevel(level)->updateCenter(camera->getPosition(), &rows_update, &cols_update);
	}

	//update level offset
	for (int level = 1; level < num_level_; level++)
	{
		V2f level_off(0.0f, 0.0f);
		level_off = getLevel(level)->center() - getLevel(level - 1)->center();
		getLevel(level)->setLevelOffset(level_off);
	}

	//update visiable FootPrints
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->update(camera, updater);
	}

	//update extent and texture origin
	for (int level = 0; level < num_level_; level++)
	{
		getLevel(level)->updateExtentAndTextureOrigin();
	}

	//apply new raster Datas
	updater->applyNewData();

	if (test_first)
	{
		test_first = !test_first;
		layer = getLevel(num_level_ - 1);
		updater->requestTileResidency(layer,layer->getRaster(),&layer->nextExtend());
	}

	//new required and update textures
	for (int level = 0; level < num_level_; level++)
	{
		//require new data
		layer = getLevel(level);
		//updater->requestTileResidency(layer,layer->getRaster(),&layer->nextExtend());
		//update textures

	}
}


int MapLayerManager::getCmapSize() const
{
	return cmapSize_;
}


int MapLayerManager::getLevelNum() const
{
	return num_level_;
}


void MapLayerManager::initCorsestLevel(scene::RasterUpdater *updater)
{
	TerrainRasterLevel * renderRasterLevel = getLevel(num_level_ - 1);
	RasterLevel * rl = renderRasterLevel->getRaster();
	RasterExtent extent(0, rl->rasterXNum() - 1, rl->rasterYNum() - 1, 0);
	updater->requestTileResidency(renderRasterLevel, renderRasterLevel->getRaster(), &extent);
}


scene::TerrainClipLevel* MapLayerManager::getLevel(const unsigned int& level)
{
	if (level == 0)
		return innermostLayer_;
	else
		return layers_[level - 1];
}

void MapLayerManager::updateBlockMinMaxVal()
{

}

ClipmapTerrain::ClipmapTerrain(int cmapSize, int level) :cmapSize_(cmapSize),
level_(level)
{

}

ClipmapTerrain::~ClipmapTerrain()
{
	if (layerManager_)
	{
		delete layerManager_;
	}
	if (baseShader_)
	{
		delete baseShader_;
		baseShader_ = nullptr;
	}
}

bool ClipmapTerrain::init()
{
	scene::RasterResource* rasterResource = new scene::TerrainResource;
	rasterResource->setNumLevel(level_);
	rasterResource->setTileWH(128, 128);
	scene::GeodeticExtent extent{ -64.0,64.0,64.0,-64.0 }; //64km * 64km
	rasterResource->setlevelZeroDeltaRasterWH((extent.right_ - extent.left_) / 4.0, (extent.top_ - extent.buttom_) / 4.0);

	rasterResource->setExtent(extent);
	if (!rasterResource->init())
	{
		printf("rasterResorce init fail!...\n");
		return false;
	}

	footprints_.init(cmapSize_);

	layerManager_ = new MapLayerManager(cmapSize_, level_, rasterResource);
	layerManager_->init();

	initShader();

	terrainUpdater_ = new TerrainUpdater(cmapSize_, cmapSize_);

	//init coarsest Level
	//layerManager_->initCorsestLevel(terrainUpdater_);

	terrainUpdater_->loadTerrainFile((get_media_BasePath() + "heightmap/ps_height_4k.png").c_str());
	terrainUpdater_->initDiffMap((get_media_BasePath() + "heightmap/ps_texture_4k.png").c_str());
	terrainUpdater_->initTextureObj();

	return true;
}

void ClipmapTerrain::update(const CameraBase*camera)
{
	layerManager_->update(camera, terrainUpdater_);
}


bool ClipmapTerrain::initShader()
{
	std::string code = Shader::loadMultShaderInOneFile("terrain/geometry_clipmappings.glsl");

	baseShader_ = new Shader;
	baseShader_->getShaderFromMultCode(Shader::VERTEX, "base", code);
	baseShader_->getShaderFromMultCode(Shader::FRAGMENT, "base", code);
	baseShader_->linkProgram();
	baseShader_->checkProgram();

	state_.uMode_ = baseShader_->getVariable("model");
	state_.uView_ = baseShader_->getVariable("view");
	state_.uProj_ = baseShader_->getVariable("projection");

	state_.ulevelOffset_ = baseShader_->getVariable("levelOffset");

	state_.ulocalOffset_ = baseShader_->getVariable("localOffset");
	state_.uScale_ = baseShader_->getVariable("scale");

	state_.heightMap_ = baseShader_->getVariable("heightmap");
	state_.diffMap_ = baseShader_->getVariable("diffmap");

	layerManager_->setShaderLocations(&state_);

	baseShader_->turnOn();
	baseShader_->setInt(state_.heightMap_, 0);
	//baseShader_->setInt(state_.diffMap_, 1);

	baseShader_->turnOff();
	return true;
}

void ClipmapTerrain::render(const CameraBase* camera)
{
	Shader* currentShader = baseShader_; //normal pass
	currentShader->turnOn();

	Matrixf modeMatrix(1.0);
	currentShader->setMatrix4(state_.uProj_, 1, GL_FALSE, math::value_ptr(camera->getProjectionMatrix()));
	currentShader->setMatrix4(state_.uView_, 1, GL_FALSE, math::value_ptr(camera->getViewMatrix()));
	currentShader->setMatrix4(state_.uMode_, 1, GL_FALSE, math::value_ptr(modeMatrix));
	glActiveTexture(GL_TEXTURE0);
	terrainUpdater_->bindHeightMap(true);

	//glActiveTexture(GL_TEXTURE1);
	//terrainUpdater_->diffuseTexture_->bind();
	layerManager_->render(currentShader, &footprints_);

	terrainUpdater_->bindHeightMap(false);

	currentShader->turnOff();

	CHECK_GL_ERROR;

}
