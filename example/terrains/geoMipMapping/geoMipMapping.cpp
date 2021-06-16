#include "geoMipmapping.h"
#include <camera.h>
#include <png.h>
#include <shader.h>
#include <engineLoad.h>
#include <glu.h>
#include <resource.h>
#include <helpF.h>

//this demo is from Willem H.de Boer "Fast Terrain Rendering Using Geometrical MipMapping"
static const unsigned int BLOCKSIZE = 16;
static const unsigned int HALF_BLOCKSIZE = BLOCKSIZE / 2.0F;

static const unsigned int MAX_DRAW_VERT_COUNT = (1024 * 1024 * 2 * 3); // tune this
static const unsigned int MAX_DRAW_IDX_COUNT = (1024 * 1024 * 2 * 3); // tune this
#define	 RESTART_INDEX	0xffffffff
#define FOG_DENSITY			0.002f

#define HORIZONTAL_COLOR	{ 0.8f, 0.9f, 0.9f, 1.0f }


unsigned char limit(unsigned char ucValue)
{
	if (ucValue > 254)
		return 254;
	else if (ucValue < 0)
		return 0;

	return ucValue;
}

GeoMipMapping::GeoMipMapping()
{

}

GeoMipMapping::~GeoMipMapping()
{
	if (blocks_)
	{

	}
	if (vertexs_) delete vertexs_;
	if (indices_) delete indices_;
	if (vao_ != 0)
	{
		glDeleteVertexArrays(1, &vao_);
	}

}

bool GeoMipMapping::initGometry()
{
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	vertexs_ = new FlashBuffer<LVertex>(MAX_DRAW_VERT_COUNT);
	indices_ = new FlashBuffer<unsigned int >(MAX_DRAW_IDX_COUNT);

	vertexs_->setTarget(GL_ARRAY_BUFFER);
	vertexs_->create_buffers(FlashBuffer<LVertex>::ModeUnsynchronized,false);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LVertex), (void*)0);
	indices_->setTarget(GL_ELEMENT_ARRAY_BUFFER);
	indices_->create_buffers(FlashBuffer<unsigned int>::ModeUnsynchronized,false);

	glBindVertexArray(0);

	CHECK_GL_ERROR;

	return (vertexs_ && indices_);
}

bool GeoMipMapping::initHeightMap(const char*heightFile)
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

			heightData_.heightData_ = new float[mCX * mCZ];
			heightData_.xSize_ = mCX;
			heightData_.zSize_ = mCZ;

			if (image_png.format == PNG_FORMAT_LINEAR_Y)
			{
				for (unsigned int z = 0; z < mCZ; z++)
				{
					unsigned short * src = (unsigned short*)mData + z * mCX;
					for (unsigned int x = 0; x < mCX; x++)
					{
						heightData_.heightData_[(mCZ - z - 1) * mCX + x] = (float)(src[x]) * (0.1f / 40.0f);
					}
				}
			}

			free(mData);
			return true;
		}
	}
	return false;
}

void GeoMipMapping::initConstant(const CameraBase*camera, float vh)
{
	xsize_ =  heightData_.xSize_;
	zsize_ =   heightData_.zSize_;

	blocksXCount_ = xsize_ / BLOCKSIZE;
	blocksZCount_ = zsize_ / BLOCKSIZE;

	maxPixelError_ = 4;

	maxLevel_ = 1;
	int BLOCK_SIZE = BLOCKSIZE;
	while (BLOCK_SIZE >= 2) {
		BLOCK_SIZE >>= 1;
		maxLevel_++;
	}

	float l, r, t, b, n, f;
	camera->getViewFrustum(&l, &r, &t, &b, &n, &f);

	float A = n / t;
	float T = 2.0f * maxPixelError_ / vh;
	C_ = A / T;

	shader_->turnOn();
	GLint loc =  shader_->getVariable("tex");
	if (loc != -1)
		shader_->setFloat2(loc, xsize_ * 1.0f,zsize_ * 1.0f);
}

bool GeoMipMapping::initHeightTexture(unsigned int xsize, unsigned int ysize)
{
	std::string tx = get_media_BasePath() + "heightmap/ps_texture_4k.png";
	diffuseTexture_ = new Texture(tx.c_str());
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

	tx = get_texture_BasePath() + "detail.png";
	deailTexture_ = new Texture(tx.c_str());
	deailTexture_->target_ = GL_TEXTURE_2D;
	if (deailTexture_->loadData())
	{
		deailTexture_->createObj();
		deailTexture_->bind();
		deailTexture_->mirrorRepeat();
		deailTexture_->filterLinear();
		if (!deailTexture_->context(NULL))
		{
			return false;
		}
		CHECK_GL_ERROR;
	}

#if 0
	Image_SP imgs[4];

	imgs[0] = IO::EngineLoad::loadImage("terrain/lowestTile.tga");
	imgs[1] = IO::EngineLoad::loadImage("terrain/lowTile.tga");
	imgs[2] = IO::EngineLoad::loadImage("terrain/highTile.tga");
	imgs[3] = IO::EngineLoad::loadImage("terrain/highestTile.tga");

	numTexture_ = 4;
	int iLastHeight = -1;
	for (int i = 0; i < 4; i++)
	{
		//calculate the three height boundaries
		regions_[i].m_iLowHeight = iLastHeight + 1;
		iLastHeight += 255 / numTexture_;
		regions_[i].m_iOptimalHeight = iLastHeight;
		regions_[i].m_iHighHeight = (iLastHeight - regions_[i].m_iLowHeight) + iLastHeight;
	}

	Image_SP img = new Image;
	img->allocate(xsize, ysize, 24 / 8);
	img->levelDataPtr_ = new uint8*[1];
	img->levelDataPtr_[0] = (uint8*)img->pixels();
	img->settarget(GL_TEXTURE_2D);
	img->setinternalformat(GL_RGB8);
	img->settype(GL_UNSIGNED_BYTE);
	img->setformat(GL_RGB);
	img->settypesize(0);
	img->setfaces(1);

	float fTotalRed, fTotalGreen, fTotalBlue;

	unsigned int uiTexX;
	unsigned int uiTexY;

	uint8 ucRed, ucGreen, ucBlue;
	float fBlend[4];
	int i;

	float fMapRationX = (float)xsize_ / xsize;
	float fMapRationY = (float)ysize_ / ysize;

	for (int y = 0; y < ysize; y++)
	{
		for (int x = 0; x < xsize; x++)
		{
			fTotalRed = 0.0f;
			fTotalGreen = 0.0f;
			fTotalBlue = 0.0f;

			//loop through the tiles (for the third time in this function!)
			for (int i = 0; i < 4; i++)
			{
				uiTexX = x;
				uiTexY = y;

					getTexCoords(imgs[i]->width(),imgs[i]->height(), &uiTexX, &uiTexY);

					imgs[i]->getVal(uiTexX, uiTexY, &ucRed, &ucGreen, &ucBlue);

					fBlend[i] = regionPercent(i, interpolateHeight(x, y, fMapRationX, fMapRationY));

					//calculate the RGB values that will be used
					fTotalRed += ucRed*fBlend[i];
					fTotalGreen += ucGreen*fBlend[i];
					fTotalBlue += ucBlue*fBlend[i];
			}

			//set our terrain's texture color to the one that we previously calculated
			uint8 r = limit((unsigned char)fTotalRed);
			uint8 g = limit((unsigned char)fTotalGreen);
			uint8 b = limit((unsigned char)fTotalBlue);

			img->setVal(x, y, &r, &g, &b);
		}
	}

	texture_ = new Texture(img);
	texture_->createObj();
	texture_->bind();
	texture_->mirrorRepeat();
	texture_->filterLinear();
	if (texture_->context(NULL))
	{
		texture_->unBind();
	}
#endif

	return true;
}

void GeoMipMapping::updateGeometry(const CameraBase*camera)
{
	glBindVertexArray(vao_);
	num_vertexs_ = 0;
	num_indices_ = 0;
	vertexs_->mapBuffer();
	LVertex * vertices = vertexs_->getBufferPtr();
	indices_->mapBuffer();
	unsigned int * indices = indices_->getBufferPtr();

	for (int y = 0; y < blocksZCount_; y++)
		for (int x = 0; x < blocksXCount_; x++)
			updateBlockState(&blocks_[y][x], camera);

	for (int y = 0; y < blocksZCount_; y++)
		for (int x = 0; x < blocksXCount_; x++)
			if(blocks_[y][x].visiable_) 
				updateBlockGeometry(&blocks_[y][x], vertices, indices);

	vertexs_->setDataSize(num_vertexs_ * sizeof(Vertex));
	vertexs_->flush_data();
	indices_->setDataSize(num_indices_ * sizeof(unsigned int));
	indices_->flush_data();

}

void GeoMipMapping::render(const CameraBase* camera)
{
	shader_->turnOn();

	GLint location = shader_->getVariable("projection");
	if (location != -1)
		shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera->getProjectionMatrix()));

	location = shader_->getVariable("view");
	if (location != -1)
		shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera->getViewMatrix()));

	if (diffuseTexture_)
	{
		glActiveTexture(GL_TEXTURE0);
		diffuseTexture_->bind();
	}
	CHECK_GL_ERROR;

	if (deailTexture_)
	{
		glActiveTexture(GL_TEXTURE1);
		deailTexture_->bind();
	}
	CHECK_GL_ERROR;

	glBindVertexArray(vao_);
	vertexs_->bind();
	indices_->bind();

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(RESTART_INDEX);
	glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, NULL);
	glDisable(GL_PRIMITIVE_RESTART);
	glBindVertexArray(0);

}

bool GeoMipMapping::initShader()
{
	shader_ = new Shader;
	shader_->addShader(Shader::VERTEX, "./terrain/terrain.vert"); CHECK_GL_ERROR;
	shader_->addShader(Shader::FRAGMENT, "./terrain/terrain.frag"); CHECK_GL_ERROR;
	shader_->linkProgram();
	shader_->checkProgram();
	CHECK_GL_ERROR;
	shader_->turnOn();

	GLint loc = shader_->getVariable("g_hightmap");
	if (loc != -1)
		shader_->setInt(loc, 0);
	loc = shader_->getVariable("g_detailmap");
	if (loc != -1)
		shader_->setInt(loc, 1);

	loc = shader_->getVariable("g_fog");
	if (loc != -1)
	{
		V4f frog = HORIZONTAL_COLOR;
		shader_->setFloat4(loc, frog.x,frog.y,frog.z, FOG_DENSITY);
	}
	shader_->turnOff();

	return true;
}

void GeoMipMapping::keyBoard(int key)
{
	if (key == GLU_KEY_PAGE_UP)
	{
		maxPixelError_++;
	}
	else
	{
		maxPixelError_--;
	}

	float l, r, t, b, n, f;
	//g_camera->getViewFrustum(&l, &r, &t, &b, &n, &f);

	float A = n / t;
	float T;// = 2.0f * maxPixelError_ / g_vh;
	C_ = A / T;

	for (int i = 0; i < blocksZCount_; i++)
	{
		for (int j = 0; j < blocksXCount_; j++)
		{

			TerrainBlock* block = &blocks_[i][j];
			if (block->dSeqList_ == nullptr) block->dSeqList_ = new float[maxLevel_ - 1];
			for (int k = 1; k < maxLevel_; k++)
			{
				float err = block->errList_[k - 1];
				float d = (err * err) * (C_ * C_); // squared
				block->dSeqList_[k - 1] = d;
			}
		}
	}
}

void GeoMipMapping::updateBlockState(TerrainBlock*block,const CameraBase*camera)
{
	block->visiable_ = (camera->cubeFrustumTest(block->center_.x, block->center_.y, block->center_.z, BLOCKSIZE * 0.7)) ? true : false;
	block->displayLevel_ = 0;
	if(block->visiable_)
	{
		V3f pos = camera->getPosition();

		V3f to_view = pos - block->center_;
		to_view[2] = 0.0;

		float dist = to_view.x * to_view.x + to_view.y * to_view.y;
		for (int i = 1; i < maxLevel_; i++)
		{
			if (dist > block->dSeqList_[i - 1])
				block->displayLevel_ = i;
			else
			{
				break;
			}
		}
	}
}

void GeoMipMapping::getBlockStart(TerrainBlock*block, float*x, float *z)
{
	float block_start_x = start_X_;
	float block_start_z = start_Z_;

	*x = block->xIndex_ * BLOCKSIZE + block_start_x;
	*z = block->zIndex_ * BLOCKSIZE + block_start_z;
}

void GeoMipMapping::updateBlockGeometry(TerrainBlock*block, LVertex*vertexData, unsigned int * indicesData)
{
	unsigned int t = BLOCKSIZE >> block->displayLevel_;
	unsigned int block_need_vertices = (t + 1) * (t + 1);
	if (block_need_vertices + num_vertexs_ > MAX_DRAW_VERT_COUNT) return;

	int need_max_indices = (t * (t + 1) * 2) + t; /* plus restart index */
	need_max_indices += (t / 2) * 4; // possible edge restart index

	if ((need_max_indices + num_indices_) > MAX_DRAW_IDX_COUNT) return;


	float vert_start_x;
	float vert_start_z;
	getBlockStart(block, &vert_start_x, &vert_start_z);
	int vert_step = 1 << block->displayLevel_;

	unsigned global_index_offset = (unsigned)num_vertexs_;

	// setup vertices
	for (int z = vert_start_z; z <= vert_start_z + BLOCKSIZE; z += vert_step) 
	{
		for (int x = vert_start_x; x <= vert_start_x + BLOCKSIZE; x += vert_step) 
		{
			vertexData[num_vertexs_++].xyz = V3f((float)x, getTrueHeightAtPoint(x, z), (float)z); //
		}
	}
	
	if (block->displayLevel_ == maxLevel_ - 1) 
	{
		indicesData[num_indices_++] = RESTART_INDEX;

		indicesData[num_indices_++] = 2 + global_index_offset;
		indicesData[num_indices_++] = 0 + global_index_offset;
		indicesData[num_indices_++] = 3 + global_index_offset;
		indicesData[num_indices_++] = 1 + global_index_offset;

		return;
	}

	for (int row = 1; row < t - 1; ++row) 
	{
		indicesData[num_indices_++] = RESTART_INDEX;

		for (int col = 1; col < t; ++col) 
		{
			int local_idx0 = (row + 1) * (t + 1) + col;
			int local_idx1 = local_idx0 - t - 1;

			indicesData[num_indices_++] = local_idx0 + global_index_offset;
			indicesData[num_indices_++] = local_idx1 + global_index_offset;
		}
	}

	// check neighbors
	// bottom
	if (block->zIndex_ > 0) { // has bottom neighbor
		TerrainBlock * bottom_neighbor = &blocks_[block->zIndex_ - 1][block->xIndex_];
		if (bottom_neighbor->displayLevel_ > block->displayLevel_) {
			SetupBottomEdgeFixGap(block, bottom_neighbor, global_index_offset,indicesData);
		}
		else {
			SetupBottomEdgeNormally(block, global_index_offset, indicesData);
		}
	}
	else {
		SetupBottomEdgeNormally(block, global_index_offset, indicesData);
	}

	// right
	if (block->xIndex_ < blocksXCount_ - 1) {
		TerrainBlock * right_neighbor = &blocks_[block->zIndex_][block->xIndex_ + 1];
		if (right_neighbor->displayLevel_ > block->displayLevel_) {
			SetupRightEdgeFixGap(block, right_neighbor, global_index_offset, indicesData);
		}
		else {
			SetupRightEdgeNormally(block, global_index_offset, indicesData);
		}
	}
	else {
		SetupRightEdgeNormally(block, global_index_offset, indicesData);
	}

	// top
	if (block->zIndex_ < blocksZCount_ - 1) {
		TerrainBlock * top_neighbor = &blocks_[block->zIndex_ + 1][block->xIndex_];
		if (top_neighbor->displayLevel_ > block->displayLevel_) {
			SetupTopEdgeFixGap(block, top_neighbor, global_index_offset, indicesData);
		}
		else {
			SetupTopEdgeNormally(block, global_index_offset, indicesData);
		}
	}
	else {
		SetupTopEdgeNormally(block, global_index_offset, indicesData);
	}

	// left
	if (block->xIndex_ > 0) {
		TerrainBlock * left_neighbor = &blocks_[block->zIndex_][block->xIndex_ - 1];
		if (left_neighbor->displayLevel_ > block->displayLevel_) {
			SetupLeftEdgeFixGap(block, left_neighbor, global_index_offset, indicesData);
		}
		else {
			SetupLeftEdgeNormally(block, global_index_offset, indicesData);
		}
	}
	else {
		SetupLeftEdgeNormally(block, global_index_offset, indicesData);
	}
}

/*

bottom edge topology

<------
______
 /|\ |\ |\
/_|_\|_\|_\

*/

void GeoMipMapping::SetupBottomEdgeNormally(TerrainBlock *block, unsigned int global_index_offset,unsigned int*indicesData)
{
	int display_quad_dim = BLOCKSIZE >> block->displayLevel_;

	indicesData[num_indices_++] = RESTART_INDEX;

	indicesData[num_indices_++] = display_quad_dim + global_index_offset;
	for (int col = display_quad_dim - 1; col >= 1; --col) {
		int local_idx0 = display_quad_dim + 1 + col;
		int local_idx1 = local_idx0 - display_quad_dim - 1;

		indicesData[num_indices_++] = local_idx0 + global_index_offset;
		indicesData[num_indices_++] = local_idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = 0 + global_index_offset;
}

/*

right edge topology

|
|
\|/

  /|
 /_|
| /|
|/_|
| /|
|/_|
 \ |
  \|

*/

void GeoMipMapping::SetupRightEdgeNormally(TerrainBlock *block, unsigned int global_index_offset,unsigned int *indicesData)
{
	int display_quad_dim = BLOCKSIZE >> block->displayLevel_;

	indicesData[num_indices_++] = RESTART_INDEX;

	int last_local_idx = (display_quad_dim + 1) * (display_quad_dim + 1) - 1;

	indicesData[num_indices_++] = last_local_idx + global_index_offset;
	for (int row = display_quad_dim - 1; row >= 1; --row) {
		int local_idx0 = (display_quad_dim + 1) * (row + 1) - 2;
		int local_idx1 = local_idx0 + 1;

		indicesData[num_indices_++] = local_idx0 + global_index_offset;
		indicesData[num_indices_++] = local_idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = display_quad_dim + global_index_offset;
}

/*

top edge topology

------>
___________
\ |\ |\ | /
 \|_\|_\|/

*/
void GeoMipMapping::SetupTopEdgeNormally(TerrainBlock *block, unsigned int global_index_offset, unsigned int *indicesData)
{
	int display_quad_dim = BLOCKSIZE >> block->displayLevel_;

	indicesData[num_indices_++] = RESTART_INDEX;

	indicesData[num_indices_++] = display_quad_dim * (display_quad_dim + 1) + global_index_offset;
	for (int col = 1; col < display_quad_dim; ++col) {
		int local_idx0 = (display_quad_dim - 1) * (display_quad_dim + 1) + col;
		int local_idx1 = local_idx0 + display_quad_dim + 1;

		indicesData[num_indices_++] = local_idx0 + global_index_offset;
		indicesData[num_indices_++] = local_idx1 + global_index_offset;
	}
	int last_local_idx = (display_quad_dim + 1) * (display_quad_dim + 1) - 1;
	indicesData[num_indices_++] = last_local_idx + global_index_offset;
}

/*

left edge topology

/|\
|
|

|\
|_\
| /|
|/_|
| /|
|/_|
| /
|/

*/
void GeoMipMapping::SetupLeftEdgeNormally(TerrainBlock *block, unsigned int global_index_offset, unsigned int *indicesData)
{
	int display_quad_dim = BLOCKSIZE >> block->displayLevel_;

	indicesData[num_indices_++] = RESTART_INDEX;
	indicesData[num_indices_++] = 0 + global_index_offset;
	for (int row = 1; row < display_quad_dim; ++row) {
		int local_idx0 = (display_quad_dim + 1) * row + 1;
		int local_idx1 = local_idx0 - 1;

		indicesData[num_indices_++] = local_idx0 + global_index_offset;
		indicesData[num_indices_++] = local_idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = display_quad_dim * (display_quad_dim + 1) + global_index_offset;
}

void GeoMipMapping::SetupBottomEdgeFixGap(TerrainBlock *block, TerrainBlock *bottom_neighbor, unsigned int global_index_offset, unsigned int *indicesData)
{
	int block_quad_dim = BLOCKSIZE >> block->displayLevel_;
	int neighbor_quad_dim = BLOCKSIZE >> bottom_neighbor->displayLevel_;

	int t = block_quad_dim / neighbor_quad_dim;

	int x_max, x_min;

	bool ret = false;

	// right corner
	x_max = neighbor_quad_dim * t;
	x_min = x_max - t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int x = x_max; x > x_min; --x) {
		if (x - 1 <= 0) {
			ret = true;
			break;
		}

		int idx0 = x_max;
		int idx1 = (x - 1) + block_quad_dim + 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = x_min + global_index_offset;

	if (ret) {
		return;
	}

	// left corner
	x_max = t;
	x_min = 0;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int x = x_max; x > x_min; --x) {
		int idx0 = x_max;
		int idx1 = x + block_quad_dim + 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = x_min + global_index_offset;

	// bottom edge
	for (int i = neighbor_quad_dim - 2; i >= 1; --i) {
		x_max = (i + 1) * t;
		x_min = x_max - t;

		indicesData[num_indices_++] = RESTART_INDEX;
		for (int x = x_max; x >= x_min; --x) {
			int idx0 = x_max;
			int idx1 = x + block_quad_dim + 1;
			indicesData[num_indices_++] = idx0 + global_index_offset;
			indicesData[num_indices_++] = idx1 + global_index_offset;
		}
		indicesData[num_indices_++] = x_min + global_index_offset;
	}
}

void GeoMipMapping::SetupRightEdgeFixGap(TerrainBlock *block, TerrainBlock *right_neighbor, unsigned int global_index_offset, unsigned int *indicesData)
{
	int block_quad_dim = BLOCKSIZE >> block->displayLevel_;
	int neighbor_quad_dim = BLOCKSIZE >> right_neighbor->displayLevel_;

	int t = block_quad_dim / neighbor_quad_dim;

	int y_max, y_min;
	bool ret = false;

	// top corner
	y_max = neighbor_quad_dim * t;
	y_min = y_max - t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int y = y_max; y > y_min; --y) {
		if (y - 1 == 0) {
			ret = true;
			break;
		}

		int idx0 = y_max * (block_quad_dim + 1) + (block_quad_dim + 1) - 1;
		int idx1 = (y - 1) * (block_quad_dim + 1) + (block_quad_dim + 1) - 1 - 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = y_min * (block_quad_dim + 1) + (block_quad_dim + 1) - 1 + global_index_offset;

	if (ret) {
		return;
	}

	// bottom corner
	y_max = t;
	y_min = y_max - t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int y = y_max; y > y_min; --y) {
		int idx0 = y_max * (block_quad_dim + 1) + (block_quad_dim + 1) - 1;
		int idx1 = y * (block_quad_dim + 1) + (block_quad_dim + 1) - 1 - 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = y_min * (block_quad_dim + 1) + (block_quad_dim + 1) - 1 + global_index_offset;

	// right edge
	for (int i = neighbor_quad_dim - 2; i >= 1; --i) {
		y_max = (i + 1) * t;
		y_min = y_max - t;

		indicesData[num_indices_++] = RESTART_INDEX;
		for (int y = y_max; y >= y_min; --y) {
			int idx0 = (y_max + 1) * (block_quad_dim + 1) - 1;
			int idx1 = (y + 1) * (block_quad_dim + 1) - 1 - 1;
			indicesData[num_indices_++] = idx0 + global_index_offset;
			indicesData[num_indices_++] = idx1 + global_index_offset;
		}
		indicesData[num_indices_++] = (y_min + 1) * (block_quad_dim + 1) - 1 + global_index_offset;
	}
}

void GeoMipMapping::SetupTopEdgeFixGap(TerrainBlock *block, TerrainBlock *top_neighbor, unsigned int global_index_offset, unsigned int *indicesData)
{
	int block_quad_dim = BLOCKSIZE >> block->displayLevel_;
	int neighbor_quad_dim = BLOCKSIZE >> top_neighbor->displayLevel_;

	int t = block_quad_dim / neighbor_quad_dim;

	int x_max, x_min;
	bool ret = false;

	// left corner
	x_min = 0;
	x_max = x_min + t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int x = x_min; x < x_max; ++x) {
		if (x + 1 >= neighbor_quad_dim * t) {
			ret = true;
			break;
		}

		int idx0 = x_min + block_quad_dim * (block_quad_dim + 1);
		int idx1 = (x + 1) + block_quad_dim * (block_quad_dim + 1) - block_quad_dim - 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = x_max + block_quad_dim * (block_quad_dim + 1) + global_index_offset;

	if (ret) {
		return;
	}

	// right corner
	x_min = (neighbor_quad_dim - 1) * t;
	x_max = x_min + t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int x = x_min; x < x_max; ++x) {
		int idx0 = x_min + block_quad_dim * (block_quad_dim + 1);
		int idx1 = x + block_quad_dim * (block_quad_dim + 1) - block_quad_dim - 1;
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = x_max + block_quad_dim * (block_quad_dim + 1) + global_index_offset;

	// top edge
	for (int i = 1; i <= neighbor_quad_dim - 2; ++i) {
		x_min = i * t;
		x_max = x_min + t;

		indicesData[num_indices_++] = RESTART_INDEX;
		for (int x = x_min; x <= x_max; ++x) {
			int idx0 = x_min + block_quad_dim * (block_quad_dim + 1);
			int idx1 = x + block_quad_dim * (block_quad_dim + 1) - block_quad_dim - 1;
			indicesData[num_indices_++] = idx0 + global_index_offset;
			indicesData[num_indices_++] = idx1 + global_index_offset;
		}
		indicesData[num_indices_++] = x_max + block_quad_dim * (block_quad_dim + 1) + global_index_offset;
	}
}

void GeoMipMapping::SetupLeftEdgeFixGap(TerrainBlock *block, TerrainBlock *left_neighbor, unsigned int global_index_offset, unsigned int *indicesData)
{
	int block_quad_dim = BLOCKSIZE >> block->displayLevel_;
	int neighbor_quad_dim = BLOCKSIZE >> left_neighbor->displayLevel_;

	int t = block_quad_dim / neighbor_quad_dim;

	int y_max, y_min;

	bool ret = false;

	// bottom corner
	y_min = 0;
	y_max = t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int y = y_min; y < y_max; ++y) {
		if (y + 1 >= neighbor_quad_dim * t) {
			ret = true;
			break;
		}

		int idx1 = (y + 1) * (block_quad_dim + 1) + 1;
		int idx0 = y_min * (block_quad_dim + 1);
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = y_max * (block_quad_dim + 1) + global_index_offset;

	if (ret) {
		return;
	}

	// top corner
	y_min = (neighbor_quad_dim - 1) * t;
	y_max = y_min + t;

	indicesData[num_indices_++] = RESTART_INDEX;
	for (int y = y_min; y < y_max; ++y) {
		int idx1 = y * (block_quad_dim + 1) + 1;
		int idx0 = y_min * (block_quad_dim + 1);
		indicesData[num_indices_++] = idx0 + global_index_offset;
		indicesData[num_indices_++] = idx1 + global_index_offset;
	}
	indicesData[num_indices_++] = y_max * (block_quad_dim + 1) + global_index_offset;

	// left edge
	for (int i = 1; i <= neighbor_quad_dim - 2; ++i) {
		y_min = i * t;
		y_max = y_min + t;

		indicesData[num_indices_++] = RESTART_INDEX;
		for (int y = y_min; y <= y_max; ++y) {
			int idx1 = y * (block_quad_dim + 1) + 1;
			int idx0 = y_min * (block_quad_dim + 1);
			indicesData[num_indices_++] = idx0 + global_index_offset;
			indicesData[num_indices_++] = idx1 + global_index_offset;
		}
		indicesData[num_indices_++] = y_max * (block_quad_dim + 1) + global_index_offset;
	}
}

void GeoMipMapping::getTexCoords(unsigned int uiWidth, unsigned int uiHeight, unsigned int* x, unsigned int* y)
{
	int iRepeatX = -1;
	int iRepeatY = -1;
	int i = 0;

	while (iRepeatX == -1)
	{
		i++;
		if (*x < (uiWidth*i))
			iRepeatX = i - 1;
	}

	i = 0;
	while (iRepeatY == -1)
	{
		i++;
		if (*y < (uiHeight*i))
			iRepeatY = i - 1;
	}

	*x = *x - (uiWidth*iRepeatX);
	*y = *y - (uiHeight*iRepeatY);
}

float GeoMipMapping::regionPercent(int tileType, unsigned char ucHeight)
{
	float fTemp1, fTemp2;
	int t = ucHeight;
	if (tileType == LOWEST_TILE && ucHeight < regions_[LOWEST_TILE].m_iOptimalHeight)
			return 1.0f;

	if (ucHeight < regions_[tileType].m_iLowHeight)
		return 0.0f;

	else if (ucHeight > regions_[tileType].m_iHighHeight)
		return 0.0f;

	if (ucHeight < regions_[tileType].m_iOptimalHeight)
	{
		//calculate the texture percentage for the given tile's region
		fTemp1 = (float)ucHeight - regions_[tileType].m_iLowHeight;
		fTemp2 = (float)regions_[tileType].m_iOptimalHeight - regions_[tileType].m_iLowHeight;

		return (fTemp1 / fTemp2);
	}

	else if (ucHeight == regions_[tileType].m_iOptimalHeight)
		return 1.0f;

	else if (ucHeight > regions_[tileType].m_iOptimalHeight)
	{
		fTemp1 = (float)regions_[tileType].m_iHighHeight - regions_[tileType].m_iOptimalHeight;
		return ((fTemp1 - (ucHeight - regions_[tileType].m_iOptimalHeight)) / fTemp1);
	}

	return 0.0f;
}

unsigned char GeoMipMapping::interpolateHeight(int x, int y, float fHeightToTexRatioX, float fHeightToTexRatioY)
{
	unsigned char ucLow, ucHighX, ucHighY;
	float ucX, ucZ;
	float fScaledX = x*fHeightToTexRatioX;
	float fScaledY = y*fHeightToTexRatioY;
	float fInterpolation;

	//set the middle boundary
	ucLow = getTrueHeightAtPoint((int)fScaledX, (int)fScaledY);

	if ((fScaledX + 1) > xsize_)
		return ucLow;
	else
		ucHighX = getTrueHeightAtPoint((int)fScaledX + 1, (int)fScaledY);

	fInterpolation = (fScaledX - (int)fScaledX);
	ucX = ((ucHighX - ucLow)*fInterpolation) + ucLow;

	if ((fScaledY + 1) > zsize_)
		return ucLow;
	else
		ucHighY = getTrueHeightAtPoint((int)fScaledX, (int)fScaledY + 1);

	fInterpolation = (fScaledY - (int)fScaledY);
	ucZ = ((ucHighY - ucLow)*fInterpolation) + ucLow;

	return ((unsigned char)((ucX + ucZ) / 2));
}

bool GeoMipMapping::initBlock()
{
	blocks_ = new TerrainBlock*[blocksZCount_];
	for (int i = 0; i < blocksZCount_; i++)
		blocks_[i] = new TerrainBlock[blocksXCount_];


	for (int i = 0; i < blocksZCount_; i++)
	{
		for (int j = 0; j < blocksXCount_; j++)
		{

			TerrainBlock * block = &blocks_[i][j];
			block->xIndex_ = j;
			block->zIndex_ = i;

			block->displayLevel_ = 0;
			initBlockCenter(block);
			initBlockErrList(block);

			////preCaclulate the Dn for each level
			if (block->dSeqList_ == nullptr) block->dSeqList_ = new float[maxLevel_ - 1];
			for (int k = 1; k < maxLevel_; k++)
			{
				float err = block->errList_[k - 1];
				float d = (err * err) * (C_ * C_); // squared
				block->dSeqList_[k - 1] = d;
			}
		}
	}

	return true;
}

#define MIN_LEVEL_ERROR_TIMES	1.5f

bool GeoMipMapping::initBlockErrList(TerrainBlock*block)
{
	if (block->errList_ == nullptr)
		block->errList_ = new float[maxLevel_ - 1];

	float prior_level_max_error = 0.0f;
	for (int i = 1; i < maxLevel_; i++)
	{
		float current_level_max_error = 0.0f;

		int step = 1 << i;
		int half_step = step / 2;

		float x_s, z_s;
		getBlockStart(block, &x_s, &z_s);

		int ixs = (int)x_s;
		int izs = (int)z_s;

		for (int z = (int)izs; z <= izs + BLOCKSIZE; z += step)
		{
			for (int x = ixs + half_step; x <= ixs + BLOCKSIZE - half_step; x += step)
			{
				float height = getTrueHeightAtPoint(x, z);
				float height_lf = getTrueHeightAtPoint(x - half_step, z);
				float height_rt = getTrueHeightAtPoint(x + half_step, z);
				float simplified_height = (height_lf + height_rt) * 0.5f;
				float error = fabsf(height - simplified_height);

				if (error > current_level_max_error) 
				{
					current_level_max_error = error;
				}
			}
		}

		// odd rows
		for (int z = izs + half_step; z <= izs + BLOCKSIZE - half_step; z += step) {

			for (int x = ixs; x <= ixs + BLOCKSIZE; x += step) 
			{
				float height = getTrueHeightAtPoint(x,z);
				float height_tp = getTrueHeightAtPoint(x, z + half_step);
				float height_bt = getTrueHeightAtPoint(x, z- half_step);
				float simplified_height = (height_tp + height_bt) * 0.5f;
				float error = fabsf(height - simplified_height);

				if (error > current_level_max_error) 
				{
					current_level_max_error = error;
				}
			}
		}

		// center
		for (int z = izs + half_step; z <= izs + BLOCKSIZE - half_step; z += step) 
		{

			for (int x = ixs + half_step; x <= ixs + BLOCKSIZE - half_step; x += step) 
			{

				float height = getTrueHeightAtPoint(x,z);
				float height_tp_lf = getTrueHeightAtPoint(x - half_step,z + half_step);
				float height_bt_rt = getTrueHeightAtPoint(x + half_step,z - half_step);
				float simplified_height = (height_tp_lf + height_bt_rt) * 0.5f;
				float error = fabsf(height - simplified_height);

				if (error > current_level_max_error) 
				{
					current_level_max_error = error;
				}
			}

		}

		if (current_level_max_error < prior_level_max_error * MIN_LEVEL_ERROR_TIMES) {
			current_level_max_error = prior_level_max_error * MIN_LEVEL_ERROR_TIMES;
		}

		block->errList_[i - 1] = current_level_max_error;
		prior_level_max_error = current_level_max_error;
	}
	return true;
}

void GeoMipMapping::initBlockCenter(TerrainBlock*block)
{
	float x_s, z_s;
	getBlockStart(block, &x_s, &z_s);

	block->center_.x = x_s + HALF_BLOCKSIZE;
	block->center_.z = z_s + HALF_BLOCKSIZE;

	float max_height = FLT_MIN;
	float min_height = FLT_MAX;

	for (int z = z_s; z <= z_s + BLOCKSIZE; z++)
	{
		for (int x = x_s; x <= x_s + BLOCKSIZE; x++)
		{
			float height = getTrueHeightAtPoint(x, z);
			max_height = (max_height < height) ? height : max_height;
			min_height = (min_height > height) ? height : min_height;
		}
	}

	block->center_.y =  (max_height + min_height) / 2.0f;
}

