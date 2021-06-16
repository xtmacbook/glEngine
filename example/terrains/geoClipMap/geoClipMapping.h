
#include "footPrints.h"
#include "smartPointer.h"
#include <vector>

using namespace math;
using namespace  scene;

class Shader;
class CameraBase;
//The degenerate triangles which connect level N and N + 1 are drawn using vertices from level N. 
//Trim regions which connect level N and N + 1 are drawn using vertices from level N + 1.

/************************************************************************/
/*  shadow 
	blend
	frustum culling
	terrain compression
	terrain synthesis (noise)

	mask weights (¨C1/16,
	9/16, 9/16, ¨C1/16) [Dyn et al 1987]. This upsampling
*/
/************************************************************************/

namespace scene
{
	class TerrainClipOutLevel;
	class TerrainClipInnermostLevel;
	class RasterResource;
	class RasterUpdater;
	class TerrainClipLevel;
	class TerrainUpdater;
}

class MapLayerManager
{
public:

	MapLayerManager(int cmapSize, int level, RasterResource*);
	virtual ~MapLayerManager();
	void setShaderLocations(const RenderShaderIndex*);
	void init();
	void render(Shader*, FootPrints *);
	void update(const CameraBase* eye, RasterUpdater *);
	int getCmapSize()const;
	int getLevelNum()const;
	inline const RenderShaderIndex* getShaderIndex()const { return shaderIndex_; }
	void  initCorsestLevel(RasterUpdater *);

	TerrainClipLevel*		getLevel(const unsigned int& level);
	

private:
	void updateBlockMinMaxVal();
private:
	int								num_level_;
	int								cmapSize_;
	TerrainClipOutLevel **			layers_ = nullptr;
	TerrainClipInnermostLevel *		innermostLayer_ = nullptr;
	const RenderShaderIndex*		shaderIndex_ = nullptr;
	RasterResource*					resource_ = nullptr;
};

class ClipmapTerrain
{
public:

	ClipmapTerrain(int cmapSize, int level);
	virtual ~ClipmapTerrain();

	bool  init( );
	void  update(const CameraBase*);
	void  render(const CameraBase*);

private:
	bool initShader();

	/*inline float getTrueHeightAtPoint(int iX, int iZ)
	{
		return ((float*)heightMap_.data_)[iZ * heightMap_.width_ + iX];
	}*/

	Shader*							baseShader_ = nullptr;
	RenderShaderIndex				state_;
	FootPrints						footprints_;
	base::SmartPointer<TerrainUpdater> terrainUpdater_ = nullptr;
	MapLayerManager*				layerManager_;
	int								cmapSize_;
	int								level_;
};
