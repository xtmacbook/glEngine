
#include "footPrints.h"
#include "terrainClipLevel.h"
#include "terrainClipUpdater.h"
#include "rasterLevel.h"
#include "rasterLoader.h"
#include "cameraBase.h"
#include "shader.h"

#include "gls.h"

namespace scene
{

	TerrainClipLevel::TerrainClipLevel(int campSize):campSize_(campSize) {}

	TerrainClipLevel::~TerrainClipLevel() {}

	void TerrainClipLevel::init(const V3f&center)
	{
		HeightTexture_ = new Texture;
		HeightTexture_->width_ = campSize_;
		HeightTexture_->height_ = campSize_;
		HeightTexture_->internalformat_ = GL_R16F;
		HeightTexture_->format_ = GL_RED;
		HeightTexture_->type_ = GL_HALF_FLOAT;

		HeightTexture_->createObj();
		HeightTexture_->bind();
		HeightTexture_->filterLinear();
		HeightTexture_->eMinFilter = HeightTexture_->eMagFilter = GL_NEAREST;
		HeightTexture_->iWrapR = HeightTexture_->iWrapS = HeightTexture_->iWrapT = GL_REPEAT;
		HeightTexture_->initGLObj();
		HeightTexture_->unBind();
	}

	void TerrainClipLevel::render(const RenderShaderIndex*index,Shader *, FootPrints*) {}

	void TerrainClipLevel::updateExtentAndTextureOrigin()
	{
		//level_center_ to level xindex

		V2d centerIndex;

		centerIndex.x = raster_->xtoIndex(level_center_.x);
		centerIndex.y = raster_->ytoIndex(level_center_.y);

		int half = (campSize_ - 1) / 2;
		nextExtent_.left_ = centerIndex.x - half;
		nextExtent_.right_ = centerIndex.x + half;
		nextExtent_.top_ = centerIndex.y + half;
		nextExtent_.buttom_ = centerIndex.y - half;

		updateTextureOrigin(campSize_, campSize_);
	}

	void TerrainClipLevel::updateTextureOrigin(int xstrip, int ystrip)
	{
		int deltaX = nextExtent_.left_ - currentExtent_.left_;
		int deltaY = nextExtent_.buttom_ - currentExtent_.buttom_;
		if (deltaX == 0 && deltaY == 0)
			return;

		if (currentExtent_.left_ > currentExtent_.right_ ||  // initial update
			std::abs(deltaX) >= xstrip || std::abs(deltaX) >= ystrip)      // complete update
		{
			originInTextures_ = V2i(0, 0);
		}
		else
		{
			int newOriginX = (originInTextures_.x + deltaX) % xstrip;
			if (newOriginX < 0)
				newOriginX += xstrip;
			int newOriginY = (originInTextures_.y + deltaY) % ystrip;
			if (newOriginY < 0)
				newOriginY += ystrip;
			originInTextures_ = V2i(newOriginX, newOriginY);
		}
	}


	const RasterExtent& TerrainClipLevel::nextExtend() const
	{
		return nextExtent_;
	}


	void TerrainClipLevel::currentExtend(const RasterExtent&extend)
	{
		currentExtent_ = extend;
	}

	const scene::RasterExtent& TerrainClipLevel::currentExtend() const
	{
		return currentExtent_;
	}

	const math::V2i& TerrainClipLevel::originTexture() const
	{
		return originInTextures_;
	}

	void TerrainClipLevel::setCenter(const V2f&center)
	{
		level_center_ = center;
	}

	void TerrainClipLevel::setScale(int scale)
	{
		scale_ = scale;
	}

	void TerrainClipLevel::setLevelOffset(const V2f&lf)
	{
		level_offset_ = lf;
	}

	math::V2i TerrainClipLevel::levelSize() const
	{
		int edgeLen = pow(2, level_)  * (campSize_ - 1);
		return V2i(edgeLen, edgeLen);
	}

	math::V2f TerrainClipLevel::leftTop() const
	{
		V2i size = levelSize();
		return (level_center_ - V2f(size.x * 0.5f, size.y * 0.5f));
	}

	math::V2f TerrainClipLevel::center() const
	{
		return level_center_;
	}


	int TerrainClipLevel::campSize() const
	{
		return campSize_;
	}

	void TerrainClipLevel::updateCenter(const V3f&eye, int *rows_to_update, int *cols_to_update)
	{
		*rows_to_update = 0;
		*cols_to_update = 0;

		int ex = floor(eye.x);
		int ez = floor(eye.z);

		int scale = (int)pow(2, level_);

		// if the viewpoint has moved outside the center box, shift the box until it
		// contains the viewpoint, keeping track of how many rows/cols to update
		while (ex >= level_center_.x + scale)
		{
			level_center_.x += 2 * scale;
			*cols_to_update += 2;
		}
		while (ez >= level_center_.y + scale)
		{
			level_center_.y += 2 * scale;
			*rows_to_update += 2;
		}
		while (ex < level_center_.x - scale)
		{
			level_center_.x -= 2 * scale;
			*cols_to_update -= 2;
		}
		while (ez < level_center_.y - scale)
		{
			level_center_.y -= 2 * scale;
			*rows_to_update -= 2;
		}
	}


	Texture* TerrainClipLevel::heigtTexture()
	{
		return HeightTexture_.addr();
	}

	/*
	TerrainCMapLayer
	*/

	TerrainClipOutLevel::TerrainClipOutLevel(int campSize):TerrainClipLevel(campSize)
	{
	}

	TerrainClipOutLevel::~TerrainClipOutLevel()
	{

	}

	void TerrainClipOutLevel::renderBlocks(Shader *shader, const RenderShaderIndex*index, FootPrints*footPrint)
	{
		if (!blockModelState_.empty())
		{
			footPrint->renderBlocks(shader,index, &(blockModelState_[0]), blockModelState_.size());
		}

	}
	void TerrainClipOutLevel::renderFixUps(Shader *shader, const RenderShaderIndex*index, FootPrints*footPrint)
	{
		//fixups
		if (!fixUPHorizontalModelState_.empty() || !fixUPVerticalModelState_.empty())
		{
			footPrint->preRenderFixUP();
			if (!fixUPHorizontalModelState_.empty())
				footPrint->renderFixUP(shader,index, &(fixUPHorizontalModelState_[0]), fixUPHorizontalModelState_.size(), false);
			if (!fixUPVerticalModelState_.empty())
				footPrint->renderFixUP(shader,index, &(fixUPVerticalModelState_[0]), fixUPVerticalModelState_.size(), true);
			footPrint->postRenderFixUP();
		}
	}
	void TerrainClipOutLevel::renderTrims(Shader *shader, const RenderShaderIndex*index, FootPrints*footPrint)
	{
		//InteriorTrims
		if (!interiorVerticalTrimModelState_.empty() || !interiorHorizontalTrimModelState_.empty())
		{
			footPrint->preRenderTrim();
			if (!interiorVerticalTrimModelState_.empty())
				footPrint->renderTrim(shader,index, &(interiorVerticalTrimModelState_[0]), true);
			if (!interiorHorizontalTrimModelState_.empty())
				footPrint->renderTrim(shader,index, &(interiorHorizontalTrimModelState_[0]), false);
			footPrint->postRenderTrim();
		}

	}

	void TerrainClipOutLevel::renderDegenerateTriangels(Shader *shader, const RenderShaderIndex*index, FootPrints*footPrint)
	{

		if (!outTopDegenerateTrianglesmModelState_.empty() ||
			!outRightDegenerateTrianglesmModelState_.empty() ||
			!outButtomDegenerateTrianglesmModelState_.empty() ||
			!outLeftDegenerateTrianglesmModelState_.empty())
		{
			footPrint->preRenderDegenerateTriangle();
			if (!outTopDegenerateTrianglesmModelState_.empty())
			{
				footPrint->renderDegenerateTriangle(shader, index,&outTopDegenerateTrianglesmModelState_[0], 0);
			}
			if (!outRightDegenerateTrianglesmModelState_.empty())
			{
				footPrint->renderDegenerateTriangle(shader,index, &outRightDegenerateTrianglesmModelState_[0], 1);
			}
			if (!outButtomDegenerateTrianglesmModelState_.empty())
			{
				footPrint->renderDegenerateTriangle(shader,index, &outButtomDegenerateTrianglesmModelState_[0], 2);
			}
			if (!outLeftDegenerateTrianglesmModelState_.empty())
			{
				footPrint->renderDegenerateTriangle(shader, index,&outLeftDegenerateTrianglesmModelState_[0], 3);
			}
			footPrint->postRenderDegenerateTrianglem();
		}
	}

	void TerrainClipOutLevel::update(const CameraBase* camera, RasterUpdater*updater)
	{
		updateBlocks(camera);
		updateFixUps(camera);
		updateTrims(camera);
		if (coarserLevel() != nullptr)
		{
			updateDegenerateTriangels(camera);
		}
	}

	void TerrainClipOutLevel::updateBlocks(const CameraBase* camera)
	{
		if (level_ == 0) return;

		blockModelState_.clear();

		for (int z = 0; z < 4; z++)
		{
			for (int x = 0; x < 4; x++)
			{

				V2i offset_from_center_unit;
				// draw a 4x4 set of tiles. cut out the middle 2x2 unless we're at the finest level

				if ((level_ != 0) && (x == 1 || x == 2) && (z == 1 || z == 2))
					continue;
				offset_from_center_unit = TerrainBlock::block_offset_from_level_center_[z * 4 + x];


				/*	V2f offset_from_center = offset_from_center_unit * scale_;
				V2f block_top_left_WorldPos = level_center_ + offset_from_center;
				V2f minXZ = block_top_left_WorldPos;
				V2f maxXZ = V2f(blockSize_.x, blockSize_.y) + block_top_left_WorldPos;

				CameraBase::FRUSTUMINBOXSTATE state = camera->aabbFrustumTest(V3f(minXZ.x, -100.0, minXZ.y), V3f(maxXZ.x, 100.0, maxXZ.y));*/

				//if (state == CameraBase::INSIDE || state == CameraBase::INTERSECT)
				{
					BlockRenderState state{ offset_from_center_unit,scale_ };
					blockModelState_.push_back(state);
				}
				//else
				{
					int a = 3;
				}

			}
		}
	}

	void TerrainClipOutLevel::updateFixUps(const CameraBase* eye)
	{
		if (level_ == 0) return;

		V2f localOffset;
		//fixups
		fixUPHorizontalModelState_.clear();
		fixUPVerticalModelState_.clear();
		//vertical
		fixUPVerticalModelState_.push_back({ FixUp::local_offset_[0] ,(float)scale_ });
		fixUPVerticalModelState_.push_back({ FixUp::local_offset_[1] ,(float)scale_ });
		//Horizontal
		fixUPHorizontalModelState_.push_back({ FixUp::local_offset_[2] ,(float)scale_ });
		fixUPHorizontalModelState_.push_back({ FixUp::local_offset_[3] ,(float)scale_ });
	}

	void TerrainClipOutLevel::updateTrims(const CameraBase* eye)
	{
		if (level_ == 0) return;

		V2f localOffset;
		interiorVerticalTrimModelState_.clear();
		interiorHorizontalTrimModelState_.clear();

		int index = 0;

		if (level_offset_.x > 0 && level_offset_.y > 0) index = 1;
		else if (level_offset_.x > 0 && level_offset_.y < 0) index = 3;
		else if (level_offset_.x < 0 && level_offset_.y > 0) index = 0;
		else if (level_offset_.x < 0 && level_offset_.y < 0) index = 2;

		V4i offset = InteriorTrim::local_offset_[index];
		interiorVerticalTrimModelState_.push_back({ V2i(offset.x, offset.y) ,(float)scale_ });
		interiorHorizontalTrimModelState_.push_back({ V2i(offset.z, offset.w),(float)scale_ });
	}

	void TerrainClipOutLevel::updateDegenerateTriangels(const CameraBase* eye)
	{
		V2f localOffset;
		//DegenerateTriangles
		outTopDegenerateTrianglesmModelState_.clear();
		outRightDegenerateTrianglesmModelState_.clear();
		outButtomDegenerateTrianglesmModelState_.clear();
		outLeftDegenerateTrianglesmModelState_.clear();

		outTopDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0) ,(float)scale_ });
		outRightDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0)  ,(float)scale_ });
		outButtomDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0)  ,(float)scale_ });
		outLeftDegenerateTrianglesmModelState_.push_back({ V2f(0.0,0.0) ,(float)scale_ });
	}

	/*
	TerrainInnermostLayer
	*/
	TerrainClipInnermostLevel::TerrainClipInnermostLevel(int campSize):TerrainClipLevel(campSize)
	{

	}

	TerrainClipInnermostLevel::~TerrainClipInnermostLevel()
	{

	}

	void TerrainClipInnermostLevel::update(const CameraBase* camera, RasterUpdater*updater)
	{
		state_.offset_ = V2f(0.0, 0.0);
		state_.scale_ = scale_;
	}

	void TerrainClipOutLevel::render(const RenderShaderIndex*index,Shader* shader, FootPrints* footPrint)
	{
		V2f levelOffet = leftTop();
		shader->setFloat2(index->ulevelOffset_, levelOffet.x, levelOffet.y);
		renderBlocks(shader,index, footPrint);
		renderFixUps(shader, index, footPrint);
		renderTrims(shader, index, footPrint);
		renderDegenerateTriangels(shader, index, footPrint);
	}

	void TerrainClipInnermostLevel::render(const RenderShaderIndex*index,Shader* shader, FootPrints* footPrints)
	{
		V2f levelOffet = leftTop();
		shader->setFloat2(index->ulevelOffset_, levelOffet.x, levelOffet.y);
		footPrints->renderCenterLevel(shader, index,&state_);
	}

}