

#include "terrainClipUpdater.h"
#include "footPrints.h"
#include "terrainClipLevel.h"
#include "rasterLevel.h"
#include "rasterLoader.h"
#include "rasterSource.h"

#include "texture.h"
#include "gls.h"
#include "log.h"
#include "helpF.h"
#include <png.h>
#include "shader.h"
#include "framebuffers.h"
#include "IGeometry.h"

namespace scene
{
	namespace internal
	{
		static GLfloat quadVertices[] = {
			// positions   // texCoords
			0.0f,  1.0f,  0.0f, 1.0f,
			0.0f, 0.0f,  0.0f, 0.0f,
			1.0f, 0.0f,  1.0f, 0.0f,

			0.0f,  1.0f,  0.0f, 1.0f,
			1.0f, 0.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		class  QuadLocal :public ::CommonGeometry
		{
		public:
			QuadLocal(bool update = false);
			~QuadLocal();

			virtual void updateGeometry();
			virtual void initGeometry();
			virtual void drawGeoemtry(const DrawInfo&);
			virtual void computeBoundingBox(void *);

			unsigned int vbo_;

		};

		QuadLocal::QuadLocal(bool update) :CommonGeometry(update)
		{
			glGenBuffers(1, &vbo_);
		}

		QuadLocal::~QuadLocal()
		{
			glDeleteBuffers(1, &vbo_);
		}


		void QuadLocal::updateGeometry()
		{

		}

		void QuadLocal::initGeometry()
		{
			bindVAO();

			glBindBuffer(GL_ARRAY_BUFFER, vbo_);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

			glBindVertexArray(0);
		}

		void QuadLocal::computeBoundingBox(void*)
		{

		}

		void QuadLocal::drawGeoemtry(const DrawInfo&di)
		{
			GLint  location = di.draw_shader_->getVariable("model");
			if (location != -1)
			{
				Matrixf transform = di.matrix_* getModelMatrix();
				di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
			}

			if (di.needBind_) glBindVertexArray(vao_);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}


		Pass::Pass(int w, int h) :width_(w), height_(h)
		{
			shader_ = new Shader;
			fbuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
		}

		Pass::~Pass()
		{
			if (shader_)
			{
				delete shader_;
				shader_ = nullptr;
			}
			if (fbuffer_)
			{
				delete fbuffer_;
				fbuffer_ = nullptr;
			}
		}

		void Pass::setQuad(CommonGeometry*q)
		{
			quad_ = q;
		}

		void Pass::draw(::Texture * texture, ::Texture *frameBufferText)
		{
			glActiveTexture(GL_TEXTURE0);
			texture->bind();

			std::vector<Texture*> fts;
			fts.push_back(frameBufferText);

			fbuffer_->bindObj(true);
			fbuffer_->clearBuffer();
			fbuffer_->colorTextureAttachments(fts);

			DrawInfo info;
			info.draw_shader_ = shader_;
			quad_->drawGeoemtry(info);
			fbuffer_->bindObj(false);
			texture->unBind();

			CHECK_GL_ERROR;
		}

		void Pass::initStateAndObjs(const char*shader)
		{

			std::string code = Shader::loadMultShaderInOneFile(shader);

			shader_->getShaderFromMultCode(Shader::VERTEX, "base", code);
			shader_->getShaderFromMultCode(Shader::FRAGMENT, "base", code);
			shader_->linkProgram();
			shader_->checkProgram();

			texture0_ = shader_->getVariable("texture0");
			viewportOrthographicMatrix_ = shader_->getVariable("viewportOrthographicMatrix");
			initUniform();

			math::Matrixf orthMat = math::ortho(0.0f, width_ * 1.0f, 0.0f, height_* 1.0f, 0.0f, 1.0f);
			shader_->turnOn();
			shader_->setInt(texture0_, 0);
			shader_->setMatrix4(viewportOrthographicMatrix_, 1, 0, math::value_ptr(orthMat));
			shader_->turnOff();
		}

		void UpdatePass::initUniform()
		{
			sourceOrigin_ = shader_->getVariable("sourceOrigin");
			updateSize_ = shader_->getVariable("updateSize");
			destinationOffset_ = shader_->getVariable("destinationOffset");
		}

		void SamplePass::initUniform()
		{
			sourceOrigin_ = shader_->getVariable("sourceOrigin");
			updateSize_ = shader_->getVariable("updateSize");
			destinationOffset_ = shader_->getVariable("destinationOffset");
			oneOverTextureSize_ = shader_->getVariable("oneOverTextureSize");
		}

		void ComputeNormalPass::initUniform()
		{
			origin_ = shader_->getVariable("origin");
			updateSize_ = shader_->getVariable("updateSize");
			oneOverHeightMapSize_ = shader_->getVariable("oneOverHeightMapSize");
			heightExaggeration_ = shader_->getVariable("heightExaggeration");
			postDelta_ = shader_->getVariable("postDelta");
		}

		UpdatePass::UpdatePass(int w, int h) :Pass(w, h)
		{
		}

		SamplePass::SamplePass(int w, int h) : Pass(w, h)
		{
		}

		ComputeNormalPass::ComputeNormalPass(int w, int h) : Pass(w, h)
		{

		}
	}

	TerrainUpdater::TerrainUpdater(int w, int h) :
		RasterUpdater() {
		initPass(w, h);
	}

	TerrainUpdater::~TerrainUpdater()
	{
		if (updatePass_)
		{
			delete updatePass_; updatePass_ = nullptr;
		}
		if (normalPass_)
		{
			delete normalPass_; normalPass_ = nullptr;
		}
		if (samplePass_)
		{
			delete samplePass_; samplePass_ = nullptr;
		}
	}


	void TerrainUpdater::initPass(int w, int h)
	{
		quad_ = new internal::QuadLocal;
		quad_->initGeometry();

		updatePass_ = new internal::UpdatePass(w,h);
		normalPass_ = new internal::ComputeNormalPass(w,h);
		samplePass_ = new internal::SamplePass(w,h);

		updatePass_->setQuad(quad_.addr());
		normalPass_->setQuad(quad_.addr());
		samplePass_->setQuad(quad_.addr());

		updatePass_->initStateAndObjs("terrain/clipmappings.glsl");
		normalPass_->initStateAndObjs("terrain/clipmapComputeNormals.glsl");
		samplePass_->initStateAndObjs("terrain/clipmapUpsample.glsl");

		//makecurrent
		base::source_location l("rasterUpdate", "rasterUpdate", 47);
		loader_->start(l);
	}

	bool TerrainUpdater::loadTerrainFile(const char*heightFile)
	{
		unsigned int				mCX;
		unsigned int				mCZ;
		void *						mData;

		png_image image_png;
		memset(&image_png, 0, sizeof(image_png));
		image_png.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_file(&image_png, heightFile))
		{
			size_t pixel_size = 0;

			// check file format
			switch (image_png.format)
			{
			case PNG_FORMAT_GRAY:
				pixel_size = 1;
				break;
			case PNG_FORMAT_LINEAR_Y:
				pixel_size = 2;
				break;
			case PNG_FORMAT_RGB:
				pixel_size = 3;
				break;
			case PNG_FORMAT_RGBA:
				pixel_size = 4;
				break;
			default:
				return false;
			}

			size_t size = PNG_IMAGE_SIZE(image_png); // get image size
			png_bytep buffer_png = (png_bytep)malloc(size);

			if (png_image_finish_read(&image_png, NULL/*background*/, buffer_png, 0/*row_stride*/, nullptr/*colormap*/))
			{

				mCX = image_png.width;
				mCZ = image_png.height;

				mData = malloc(image_png.height * image_png.width * pixel_size);
				memcpy(mData, buffer_png, image_png.height * image_png.width * pixel_size);

				png_image_free(&image_png);
				free(buffer_png);

				heightMap_.data_ = new float[mCX * mCZ];
				heightMap_.width_ = mCX;
				heightMap_.height_ = mCZ;

				if (image_png.format == PNG_FORMAT_LINEAR_Y)
				{
					for (unsigned int z = 0; z < mCZ; z++)
					{
						unsigned short * src = (unsigned short*)mData + z * mCX;
						for (unsigned int x = 0; x < mCX; x++)
						{
							((float*)heightMap_.data_)[(mCZ - z - 1) * mCX + x] = (float)(src[x]) * (0.1f / 40.0f);
						}
					}
				}

				free(mData);
			}

		}
		return false;
	}

	void TerrainUpdater::initTextureObj()
	{
		heightMap_.heightTextureObj_ = new ::Texture();
		heightMap_.heightTextureObj_->height_ = heightMap_.height_;
		heightMap_.heightTextureObj_->width_ = heightMap_.width_;

		heightMap_.heightTextureObj_->internalformat_ = GL_R32F;
		heightMap_.heightTextureObj_->format_ = GL_RED;

		heightMap_.heightTextureObj_->target_ = GL_TEXTURE_2D;
		heightMap_.heightTextureObj_->type_ = GL_FLOAT;
		{
			heightMap_.heightTextureObj_->createObj();
			heightMap_.heightTextureObj_->bind();
			heightMap_.heightTextureObj_->mirrorRepeat();
			heightMap_.heightTextureObj_->filterLinear();
			V4f borderColor(0.0);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (GLfloat *)&borderColor);
			if (!heightMap_.heightTextureObj_->context(heightMap_.data_))
			{
			}
			CHECK_GL_ERROR;
		}

	}

	void TerrainUpdater::bindHeightMap(bool flag)
	{
		(flag) ? heightMap_.heightTextureObj_->bind() : heightMap_.heightTextureObj_->unBind();
	}

	bool TerrainUpdater::initDiffMap(const char* file)
	{
		diffuseTexture_ = new ::Texture(file);
		diffuseTexture_->target_ = GL_TEXTURE_2D;
		if (diffuseTexture_->loadData())
		{
			diffuseTexture_->createObj();
			diffuseTexture_->bind();
			diffuseTexture_->mirrorRepeat();
			diffuseTexture_->filterLinear();
			if (!diffuseTexture_->context(NULL))
			{
				return false;
			}
			CHECK_GL_ERROR;
		}
	}

	void TerrainUpdater::renderTileToLevelTexture(const RasterTileRegion*region, TerrainRasterLevel *level, const TileData*data)
	{
		TerrainClipLevel * tcl = dynamic_cast<TerrainClipLevel*>(level);

		const RasterExtent*nextExtent = &tcl->nextExtend();
		int clipmapSize = (nextExtent->right_ - nextExtent->left_ + 1);
		V2i originInTextures = tcl->originTexture();

		int destWest = (originInTextures.x + (region->title()->extent()->left_ + region->left() - nextExtent->left_)) % clipmapSize;
		int destSouth = (originInTextures.y + (region->title()->extent()->buttom_ + region->buttom() - nextExtent->buttom_)) % clipmapSize;

		int width = region->right() - region->left() + 1;
		int height = region->top() - region->buttom() + 1;

		updatePass_->shader_->turnOn();
		updatePass_->shader_->setFloat2(updatePass_->updateSize_, width, height);
		updatePass_->shader_->setFloat2(updatePass_->sourceOrigin_, region->left(), region->buttom());
		updatePass_->shader_->setFloat2(updatePass_->destinationOffset_, destWest, destSouth);
		glDisable(GL_DEPTH_TEST);
		//updatePass_->draw(data->texture_,tcl->heigtTexture());
		updatePass_->shader_->turnOff();
		glEnable(GL_DEPTH_TEST);
	}

	void TerrainUpdater::splitUpdateToAvoidWrapping(const RasterExtent*update, const TerrainRasterLevel*level, std::vector<RasterExtent>&extents)
	{
		const TerrainClipLevel * tcl = dynamic_cast<const TerrainClipLevel*>(level);

		const V2i& origin = tcl->originTexture();
		const RasterExtent& extent = tcl->nextExtend();

		int clipmapSizeX = extent.right_ - extent.left_ + 1;
		int clipmapSizeY = extent.top_ - extent.buttom_ + 1;

		int left = (origin.x + (update->left_ - extent.left_)) % clipmapSizeX;
		int right = (origin.x + (update->right_ - extent.left_)) % clipmapSizeX;
		int buttom = (origin.y + (update->buttom_ - extent.buttom_)) % clipmapSizeY;
		int top = (origin.y + (update->top_ - extent.buttom_)) % clipmapSizeY;

		if (right < left && top < buttom)
		{
			// Horizontal AND vertical wrap
			extents.push_back({ update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->top_
			});

			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->buttom_,
			});

			extents.push_back({
				update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});

			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});
		}
		else if (right < left)
		{
			// Horizontal wrap
			extents.push_back({
				update->left_,
				extent.left_ + (clipmapSizeX - origin.x - 1),
				update->top_,
				update->buttom_

			});
			extents.push_back({
				extent.left_ + clipmapSizeX - origin.x,
				update->right_,
				update->top_,
				update->buttom_
			});
		}
		else if (top < buttom)
		{
			// Vertical wrap
			extents.push_back({
				update->left_,
				update->right_,
				extent.buttom_ + (clipmapSizeY - origin.y - 1),
				update->buttom_
			});
			extents.push_back({
				update->left_,
				update->right_,
				update->top_,
				extent.buttom_ + clipmapSizeY - origin.y
			});
		}
		else
		{
			extents.push_back(*update);
		}

	}

	void TerrainUpdater::update(const RasterExtent* update, TerrainRasterLevel*rlevel, const RasterLevel*level)
	{
		std::vector<RasterExtent> extents;
		splitUpdateToAvoidWrapping(update, rlevel, extents);

		for (int i = 0; i < extents.size(); i++)
		{
			const RasterExtent&extent = extents[i];
			std::vector<RasterTileRegion> tileRegions;
			level->getTilesInExtent(extent.left_, extent.buttom_, extent.right_, extent.top_, tileRegions);

			for (int j = 0; j < tileRegions.size(); j++)
			{
				const RasterTileRegion&region = tileRegions[j];

				RasterDataQeq::TEXTUREITER loadTexture = detail_.loadedTiles_.find(region.title()->identifier());
				if (loadTexture != detail_.loadedTiles_.end())
				{
					const TileData* data = loadTexture->second;
					renderTileToLevelTexture(&region, rlevel, data);
				}
			}
		}
	}

	void TerrainUpdater::applyNewTile(RasterTile * title, TerrainRasterLevel*level)
	{
		TerrainClipLevel * tcl = dynamic_cast<TerrainClipLevel*>(level);
		const RasterExtent& nextExtent = tcl->nextExtend();
		const RasterLevel* rasterLevel = title->level();

		RasterExtent intersection;
		intersection.left_ = math::Max(nextExtent.left_, title->extent()->left_ - 1);
		intersection.buttom_ = math::Max(nextExtent.buttom_, title->extent()->buttom_ - 1);
		intersection.right_ = math::Min(nextExtent.right_, title->extent()->right_ + 1);
		intersection.top_ = math::Min(nextExtent.top_, title->extent()->top_ + 1);

		if (intersection.right_ - intersection.left_ > 0 && intersection.top_ - intersection.buttom_ > 0)
		{
			update(&intersection, level, rasterLevel);

			// Recurse on child tiles if they're NOT loaded.  Unloaded children will use data from this tile.
			TerrainRasterLevel* finer = level->finerLevel();
			if (finer != nullptr)
			{
				applyIfNotLoaded(finer, title->southwestChild());
				applyIfNotLoaded(finer, title->southeastChild());
				applyIfNotLoaded(finer, title->northwestChild());
				applyIfNotLoaded(finer, title->northeastChild());
			}
		}
	}


	void TerrainUpdater::updateTerrain(TerrainRasterLevel*l)
	{

		TerrainClipLevel * level = dynamic_cast<TerrainClipLevel*>(l);

		int deltaX = level->nextExtend().left_ - level->currentExtend().left_;
		int deltaY = level->nextExtend().buttom_ - level->currentExtend().buttom_;

		if (deltaX == 0 && deltaY == 0)
			return;

		int minLongitude = deltaX > 0 ? level->currentExtend().right_ + 1 : level->nextExtend().left_;
		int maxLongitude = deltaX > 0 ? level->nextExtend().right_ : level->currentExtend().left_ - 1;
		int minLatitude = deltaY > 0 ? level->currentExtend().top_ + 1 : level->nextExtend().buttom_;
		int maxLatitude = deltaY > 0 ? level->nextExtend().top_ : level->currentExtend().buttom_ - 1;

		int width = maxLongitude - minLongitude + 1;
		int height = maxLatitude - minLatitude + 1;


		if (level->currentExtend().left_ > level->currentExtend().right_ || // initial update
			width >= level->campSize() || height >= level->campSize()) // complete update
		{
			// Initial or complete update.
			width = level->campSize();
			height = level->campSize();
			deltaX = level->campSize();
			deltaY = level->campSize();
			minLongitude = level->nextExtend().left_;
			maxLongitude = level->nextExtend().right_;
			minLatitude = level->nextExtend().buttom_;
			maxLatitude = level->nextExtend().top_;
		}

		if (height > 0)
		{
			RasterExtent  horizontalUpdate(
				level->nextExtend().left_,
				level->nextExtend().right_,
				maxLatitude,
				minLatitude);

			update(&horizontalUpdate, level, level->getRaster());
		}

		if (width > 0)
		{
			RasterExtent  verticalUpdate(
				minLongitude,
				maxLongitude,
				level->nextExtend().top_,
				level->nextExtend().buttom_);

			update(&verticalUpdate, level, level->getRaster());
		}

		level->currentExtend(level->nextExtend());
	}

}
