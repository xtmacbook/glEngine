/************************************************************************/
/* 2020.08.26 19:21 xiangtao                                                                     */
/************************************************************************/
#include "decl.h"
#include "rasterUpdater.h"
#include "smartPointer.h"
#include "IGeometry.h"

class Texture;
class Quad;
class Shader;
class FrameBufferObject;

struct HeightMap
{
	unsigned int width_;
	unsigned int height_;
	void * data_;
	::Texture	*	heightTextureObj_ = nullptr;
};

namespace base
{
	template<class T> class SmartPointer;
}

namespace scene
{
	class TerrainRasterLevel;
	
	namespace internal
	{
		class Pass
		{
		public:
			Pass(int w,int h);
			virtual ~Pass();
			virtual void initStateAndObjs(const char*shde);
			virtual void initUniform() = 0;
			void	 setQuad(CommonGeometry*);
			virtual void draw(::Texture*, ::Texture *frameBufferText);
			Shader *	shader_;
			CommonGeometry*		quad_;

			int texture0_;
			int viewportOrthographicMatrix_;
			FrameBufferObject* fbuffer_;
			int width_;
			int height_;
		};

		class UpdatePass :public Pass
		{
		public:
			UpdatePass(int w, int h);
			virtual void initUniform();
			int sourceOrigin_;
			int updateSize_;
			int destinationOffset_;
		};

		class SamplePass :public Pass
		{
		public:
			SamplePass(int w, int h);
			virtual void initUniform();
			int sourceOrigin_;
			int updateSize_;
			int destinationOffset_;
			int oneOverTextureSize_;

		};

		class ComputeNormalPass :public Pass
		{
		public:
			ComputeNormalPass(int w, int h);
			virtual void initUniform();
			int updateSize_;
			int origin_;
			int oneOverHeightMapSize_;
			int  heightExaggeration_;
			int  postDelta_;
		};
	}

	class LIBENIGHT_EXPORT TerrainUpdater :public RasterUpdater
	{
	public:
		TerrainUpdater(int w, int h);
		virtual ~TerrainUpdater();

		void	initPass(int w, int h);

		bool	loadTerrainFile(const char*file);
		void	initTextureObj();
		void	bindHeightMap(bool flag);
		bool	initDiffMap(const char* file);

		void	renderTileToLevelTexture(const RasterTileRegion*, TerrainRasterLevel *, const TileData*);
		void	splitUpdateToAvoidWrapping(const RasterExtent*, const TerrainRasterLevel*, std::vector<RasterExtent>&);
		void	update(const RasterExtent* update, TerrainRasterLevel*, const RasterLevel*);
		virtual void	applyNewTile(RasterTile * title, TerrainRasterLevel*);

		void	updateTerrain(TerrainRasterLevel*);

		HeightMap				heightMap_;
		::Texture*				diffuseTexture_ = nullptr;
		bool					need_update_ = true;

		internal::UpdatePass*		updatePass_ = nullptr;
		internal::SamplePass*		samplePass_ = nullptr;
		internal::ComputeNormalPass*		normalPass_ = nullptr;

		base::SmartPointer<::CommonGeometry>			quad_;
	};

}
