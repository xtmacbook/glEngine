
#include "rasterTile.h"
#include "rasterSource.h"
#include "rasterLevel.h"

#include "helpF.h"

namespace scene
{

	RasterTile::RasterTile(RasterResource*r, const RasterTileIdentifier*i):identifier_(*i)
	{
		TerrainResource * tr = dynamic_cast<TerrainResource*>(r);
		level_ = tr->level(identifier_.level_);

		extend_.left_ = identifier_.x_ * level_->titleWidth();
		extend_.right_ = math::Min(extend_.left_ + level_->titleWidth(), level_->rasterXNum()) - 1;
		extend_.buttom_ = identifier_.y_ * level_->titleHeigh();
		extend_.top_ = math::Min(extend_.buttom_ + level_->titleHeigh(), level_->rasterYNum()) - 1;
	}

	RasterTile::~RasterTile()
	{

	}

	TileData* RasterTile::loadData()
	{
		return level_->resource()->loadTileData(&identifier_);
	}

	RasterLevel * RasterTile::level()const
	{
		return level_;
	}

	const RasterTileIdentifier* RasterTile::identifier() const
	{
		return &identifier_;
	}

	const scene::RasterExtent* RasterTile::extent() const
	{
		return &extend_;
	}


	scene::RasterTile * RasterTile::southwestChild()
	{
		RasterTileIdentifier current = identifier_;
		RasterTileIdentifier* child = new RasterTileIdentifier(level_->level() - 1, current.x_ * 2, current.y_ * 2);
		return level_->resource()->getTile(child);
	}


	scene::RasterTile * RasterTile::southeastChild()
	{
		RasterTileIdentifier current = identifier_;
		RasterTileIdentifier* child = new RasterTileIdentifier(level_->level() - 1, current.x_ * 2 + 1, current.y_ * 2);
		return level_->resource()->getTile(child);
	}


	scene::RasterTile * RasterTile::northwestChild()
	{
		RasterTileIdentifier current = identifier_;
		RasterTileIdentifier* child = new RasterTileIdentifier(level_->level() - 1, current.x_ * 2, current.y_ * 2 + 1);
		return level_->resource()->getTile(child);
	}


	scene::RasterTile * RasterTile::northeastChild()
	{
		RasterTileIdentifier current = identifier_;
		RasterTileIdentifier* child = new RasterTileIdentifier(level_->level() - 1, current.x_ * 2 + 1, current.y_ * 2 + 1);
		return level_->resource()->getTile(child);
	}

	RasterTileRegion::RasterTileRegion()
	{

	}

	RasterTileRegion::RasterTileRegion(RasterTile* tile, int west, int south, int east, int north)
	{
			tile_ = tile;
			extent_.left_ = west;
			extent_.buttom_ = south;
			extent_.right_ = east;
			extent_.top_ = north;
	}

	void RasterTileRegion::setLeft(int l)
	{
		extent_.left_ = l;
	}

	void RasterTileRegion::setRight(int r)
	{
		extent_.right_ = r;
	}

	void RasterTileRegion::setTop(int t)
	{
		extent_.top_ = t;
	}

	void RasterTileRegion::setButtom(int b)
	{
		extent_.buttom_ = b;
	}

	void RasterTileRegion::setTitle(RasterTile* t)
	{
		tile_ = t;
	}

	int RasterTileRegion::left() const
	{
		return extent_.left_;
	}

	int RasterTileRegion::right() const
	{
		return extent_.right_;
	}

	int RasterTileRegion::top() const
	{
		return extent_.top_;
	}

	int RasterTileRegion::buttom() const
	{
		return extent_.buttom_;
	}

	scene::RasterTile* RasterTileRegion::title()const
	{
		return tile_;
	}

	RasterTileIdentifier::RasterTileIdentifier(int l , int x, int y)
	{
		level_ = l;
		x_ = x;
		y_ = y;
	}

}
