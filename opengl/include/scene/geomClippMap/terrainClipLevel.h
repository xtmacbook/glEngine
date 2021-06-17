#include "decl.h"
#include <type.h>
#include <vector>
#include <terrainRasterLevel.h>
#include "smartPointer.h"
#include "texture.h"
#include "flashBuffer.h"

using namespace math;
class CameraBase;
class Shader;

namespace scene
{
	class RasterUpdater;
	struct RenderShaderIndex;
	class FootPrints;

	class LIBENIGHT_EXPORT TerrainClipLevel :public TerrainRasterLevel
	{
	public:
		TerrainClipLevel(int campSize);
		virtual ~TerrainClipLevel();

		virtual void init(const V3f&center);
		virtual void render(const RenderShaderIndex*,Shader *, FootPrints*);
		virtual void update(const CameraBase* camera, RasterUpdater *) = 0;
		virtual	void updateExtentAndTextureOrigin();
		virtual	void updateTextureOrigin(int xstrip, int ystrip);

		const RasterExtent&		nextExtend()const;
		void					currentExtend(const RasterExtent&);
		const RasterExtent&		currentExtend()const;
		const V2i&				originTexture()const;

		void					setCenter(const V2f&);
		V2f						center()const;

		int						campSize()const;
		void					setScale(int);
		void					setLevelOffset(const V2f&);

		V2i						levelSize()const;
		V2f						leftTop()const; //origin for local level
		void					updateCenter(const V3f&eye, int *rows_to_update, int *cols_to_update);

		::Texture*				heigtTexture();
	protected:

		V2f						level_center_;
		int						scale_;			//rect  //current lay scale
		V2f						level_offset_; // from the level-1

		RasterExtent			currentExtent_ = { 1,0,0,1 };
		RasterExtent			nextExtent_;
		V2i						originInTextures_;

		int						campSize_;

		base::SmartPointer<::Texture>	 HeightTexture_;

		base::SmartPointer<FlashBuffer<char > > titles_data_;
	};

	class LIBENIGHT_EXPORT TerrainClipOutLevel :public TerrainClipLevel
	{
	public:

		TerrainClipOutLevel(int campSize);

		virtual ~TerrainClipOutLevel();

		virtual void render(const RenderShaderIndex*,Shader *, FootPrints*);

		virtual void update(const CameraBase* camera, RasterUpdater *);

	private:
		void updateBlocks(const CameraBase* eye);
		void updateFixUps(const CameraBase* eye);
		void updateTrims(const CameraBase* eye);
		void updateDegenerateTriangels(const CameraBase* eye);

		virtual void renderBlocks(Shader *, const RenderShaderIndex*index,FootPrints*);
		virtual void renderFixUps(Shader *, const RenderShaderIndex*index, FootPrints*);
		virtual void renderTrims(Shader *, const RenderShaderIndex*index, FootPrints*);
		virtual void renderDegenerateTriangels(Shader *, const RenderShaderIndex*index, FootPrints*);

	private:

		std::vector<BlockRenderState>   blockModelState_;

		std::vector<BlockRenderState>   fixUPVerticalModelState_;
		std::vector<BlockRenderState>   fixUPHorizontalModelState_;

		std::vector<BlockRenderState>	interiorVerticalTrimModelState_;
		std::vector<BlockRenderState>	interiorHorizontalTrimModelState_;

		std::vector<BlockRenderState>	outTopDegenerateTrianglesmModelState_;
		std::vector<BlockRenderState>	outRightDegenerateTrianglesmModelState_;
		std::vector<BlockRenderState>	outButtomDegenerateTrianglesmModelState_;
		std::vector<BlockRenderState>	outLeftDegenerateTrianglesmModelState_;

	};

	class LIBENIGHT_EXPORT TerrainClipInnermostLevel : public TerrainClipLevel
	{
	public:
		TerrainClipInnermostLevel(int campSize);
		virtual ~TerrainClipInnermostLevel();

		virtual void update(const CameraBase* camera, RasterUpdater*);
		virtual void render(const RenderShaderIndex*,Shader *, FootPrints*);
	private:
		BlockRenderState   state_;
	};
}
