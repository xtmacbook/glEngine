
#include "jpeg_gpu.h"
#include "texture.h"
#include "framebuffers.h"
#include "log.h"
#include "gls.h"
#include "jpgd.h"
#include "debug.h"
#include "shader.h"
#include "boundingBox.h"


void Jpeg_Data::QuadGemetry::initGemetry()
{
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Jpeg_Data::QuadGemetry::draw()const
{
	glBindVertexArray(vao_);
	glDrawArrays(GL_POINTS, 0, 1);
}


UpdateInfo::UpdateInfo()
{
	blocksNum = 0;
	curBlock = 0;
	blocksAdded = 0;
	pSrcCorners = NULL;
	pDstCorners = NULL;
}

UpdateInfo::~UpdateInfo()
{
	release();
}

void UpdateInfo::initialize(int blocks)
{
	release();

	blocksNum = blocks;

	pSrcCorners = new int[blocksNum * 2];
	pDstCorners = new int[blocksNum * 2];

	reset();
}

void UpdateInfo::addBlock(int srcBlock[2], int dstBlock[2])
{
	pSrcCorners[2 * curBlock] = srcBlock[0];
	pSrcCorners[2 * curBlock + 1] = srcBlock[1];

	pDstCorners[2 * curBlock] = dstBlock[0];
	pDstCorners[2 * curBlock + 1] = dstBlock[1];

	blocksAdded++;
	curBlock++;

	curBlock %= blocksNum;
}

void UpdateInfo::reset()
{
	curBlock = 0;
	blocksAdded = 0;
}

void UpdateInfo::release()
{
	SAFTE_DELETE_ARRAY(pSrcCorners)
		SAFTE_DELETE_ARRAY(pDstCorners)

		blocksNum = 0;
	curBlock = 0;
	blocksAdded = 0;
}


int GPU_Data::intitialize(int width, int height, unsigned char *pQuantTable)
{
	pTextureDCT = new Texture;
	pTextureDCT->internalformat_ = GL_R8_SNORM;
	pTextureDCT->width_ = width;
	pTextureDCT->height_ = height;
	pTextureDCT->createObj();
	pTextureDCT->bind();
	pTextureDCT->mirrorRepeat();
	pTextureDCT->filterNearest();
	pTextureDCT->contextNULL();
	pTextureDCT->unBind();
	CHECK_GL_ERROR;

	pTextureQ = new Texture;
	pTextureQ->internalformat_ = GL_R8;
	pTextureQ->format_ = GL_RED;
	pTextureQ->width_ = 8;
	pTextureQ->height_ = 8;
	pTextureQ->createObj();
	pTextureQ->bind();
	pTextureQ->mirrorRepeat();
	pTextureQ->filterNearest();
	pTextureQ->context(pQuantTable);
	pTextureQ->unBind();
	CHECK_GL_ERROR;

	pTexture1Row = new Texture;
	pTexture1Row->internalformat_ = GL_RGBA16F;
	pTexture1Row->type_ = GL_HALF_FLOAT;
	pTexture1Row->width_ = width / 8;
	pTexture1Row->height_ = height;
	pTexture1Row->createObj();
	pTexture1Row->bind();
	pTexture1Row->mirrorRepeat();
	pTexture1Row->filterNearest();
	pTexture1Row->contextNULL();
	pTexture1Row->unBind();
	CHECK_GL_ERROR;


	pTexture2Row = new Texture;
	pTexture2Row->type_ = GL_HALF_FLOAT;
	pTexture2Row->internalformat_ = GL_RGBA16F;
	pTexture2Row->width_ = width / 8;
	pTexture2Row->height_ = height;
	pTexture2Row->createObj();
	pTexture2Row->bind();
	pTexture2Row->mirrorRepeat();
	pTexture2Row->filterNearest();
	pTexture2Row->contextNULL();
	pTexture2Row->unBind();
	CHECK_GL_ERROR;

	std::vector<Texture*> textures;
	textures.push_back(pTexture1Row);
	textures.push_back(pTexture2Row);
	rowFrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	rowFrameBuffer_->bindObj(true, false);
	rowFrameBuffer_->colorTextureAttachments(textures);
	rowFrameBuffer_->bindObj(false, false);


	pTexture1Col = new Texture;
	pTexture1Col->internalformat_ = GL_RGBA16F;
	pTexture1Col->type_ = GL_HALF_FLOAT;
	pTexture1Col->width_ = width;
	pTexture1Col->height_ = height / 8;
	pTexture1Col->createObj();
	pTexture1Col->bind();
	pTexture1Col->mirrorRepeat();
	pTexture1Col->filterNearest();
	pTexture1Col->contextNULL();
	pTexture1Col->unBind();
	CHECK_GL_ERROR;


	pTexture2Col = new Texture;
	pTexture2Col->internalformat_ = GL_RGBA16F;
	pTexture2Col->type_ = GL_HALF_FLOAT;
	pTexture2Col->width_ = width;
	pTexture2Col->height_ = height / 8;
	pTexture2Col->createObj();
	pTexture2Col->bind();
	pTexture2Col->mirrorRepeat();
	pTexture2Col->filterNearest();
	pTexture2Col->contextNULL();
	pTexture2Col->unBind();
	CHECK_GL_ERROR;
	textures.clear();
	textures.push_back(pTexture1Col);
	textures.push_back(pTexture2Col);
	colFrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	colFrameBuffer_->bindObj(true, false);
	colFrameBuffer_->colorTextureAttachments(textures);
	colFrameBuffer_->bindObj(false, false);


	pTextureTarget = new Texture;
	pTextureTarget->internalformat_ = GL_R16F;
	pTextureTarget->format_ = GL_RED;
	pTextureTarget->type_ = GL_HALF_FLOAT;
	pTextureTarget->width_ = width;
	pTextureTarget->height_ = height;
	pTextureTarget->createObj();
	pTextureTarget->bind();
	pTextureTarget->mirrorRepeat();
	pTextureTarget->filterNearest();
	pTextureTarget->contextNULL();
	pTextureTarget->unBind();
	CHECK_GL_ERROR;
	targetFrameBuffer_ = new FrameBufferObject(GL_FRAMEBUFFER);
	targetFrameBuffer_->bindObj(true, false);
	targetFrameBuffer_->colorTextureAttachments(pTextureTarget);
	targetFrameBuffer_->bindObj(false, false);
	

	return  1;
}

void GPU_Data::release()
{

}

Jpeg_Data::Jpeg_Data(const char*file)
{
	loadFile(file);
}
 
Jpeg_Data::Jpeg_Data()
{

}

Jpeg_Data::~Jpeg_Data()
{
	if (mappingHandle)
	{
		CloseHandle(mappingHandle);
		mappingHandle = NULL;
	}

	if (fileHandle)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}

	pFileData = NULL;
}

bool Jpeg_Data::loadFile(const char*file)
{
	jpeg_decoder_file_stream* pinput_stream = new jpeg_decoder_file_stream();

	if (!pinput_stream->open(file))
	{
		delete pinput_stream;
		return false;
	}

	jpeg_decoder* Pd = new jpeg_decoder(pinput_stream);
	Pd->begin();

	std::string temporaryName = std::string(file) + "_UNCOMPRESSED";
	fileHandle = CreateFile((temporaryName).c_str(),
		GENERIC_ALL,
		NULL,
		NULL,
		CREATE_ALWAYS,
		FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
		NULL);

	if (!fileHandle)
	{
		return S_FALSE;
	}

	Pd->dump_DCT_data(fileHandle);

	mappingHandle = CreateFileMapping(
		fileHandle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL);

	pFileData = (char*)MapViewOfFile(
		mappingHandle,
		FILE_MAP_READ,
		0,
		0,
		0);


	imageWidth = Pd->get_width();
	imageHeight = Pd->get_height();
	componentsNum = Pd->get_num_components();
	imageFormat = GL_RED;
	type = GL_BYTE;

	static float scaleFactor[8] = { 1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
		1.0f, 0.785694958f, 0.541196100f, 0.275899379f };

	for (int i = 0; i < componentsNum; i++)
	{
		QUANT_TYPE * pTable = Pd->getQuantizationTable(i);

		for (int row = 0; row < 8; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				quantTables[i][row * 8 + col] = unsigned char(pTable[row * 8 + col] * scaleFactor[row] * scaleFactor[col]);
			}
		}
		MCU_per_row = Pd->get_mcus_per_row();

	}
	MCU_blocks_num = Pd->get_blocks_per_mcu();

	pinput_stream->close();

	delete Pd;
	delete pinput_stream;

	return true;
}

int Jpeg_Data::height() const
{
	return imageHeight;
}

int Jpeg_Data::width() const
{
	return imageWidth;
}

bool Jpeg_Data::initTechnique()
{
	Shader* shader;

	std::string code = Shader::loadMultShaderInOneFile("test/jpg_gpu.glsl");
	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "Quad", code);
	shader->linkProgram();
	shader->checkProgram();

	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Rows", code);
	shader->linkProgram();
	shader->checkProgram();
	shader->turnOn();
	shader->setInt(shader->getVariable("TextureDCT"), 0);
	shader->setInt(shader->getVariable("QuantTexture"), 1);
	shader->turnOff();

	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Unpack_Rows", code);
	shader->linkProgram();
	shader->checkProgram();
	shader->turnOn();
	shader->setInt(shader->getVariable("RowTexture1"), 0);
	shader->setInt(shader->getVariable("RowTexture2"), 1);
	shader->turnOff();


	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Columns", code);
	shader->linkProgram();
	shader->checkProgram();
	shader->turnOn();
	shader->setInt(shader->getVariable("TargetTexture"), 0);
	shader->turnOff();

	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_Unpack_Columns", code);
	shader->linkProgram();
	shader->checkProgram();
	shader->turnOn();
	shader->setInt(shader->getVariable("ColumnTexture1"), 0);
	shader->setInt(shader->getVariable("ColumnTexture2"), 1);
	shader->turnOff();


	shaders_.push_back(new Shader);
	shader = shaders_[shaders_.size() - 1];
	shader->getShaderFromMultCode(Shader::VERTEX, "Quad", code);
	shader->getShaderFromMultCode(Shader::GEOMETRY, "GL_Quad", code);
	shader->getShaderFromMultCode(Shader::FRAGMENT, "PS_IDCT_RenderToBuffer", code);
	shader->linkProgram();
	shader->checkProgram();
	shader->turnOn();
	shader->setInt(shader->getVariable("TextureY"), 0);
	shader->setInt(shader->getVariable("TextureCb"), 1);
	shader->setInt(shader->getVariable("TextureCr"), 2);
	shader->setInt(shader->getVariable("TextureHeight"), 3);
	shader->turnOff();

	CHECK_GL_ERROR;

	quadGemetry_.initGemetry();

	return true;
}

Shader* Jpeg_Data::getTechnique(const char*code)
{
	if (!strcmp(code, "base")) return shaders_[0];
	else if (!strcmp(code,"IDCT_Rows")) return shaders_[1];
	else if (!strcmp(code, "Unpack_Rows")) return shaders_[2];
	else if (!strcmp(code,"IDCT_Columns")) return shaders_[3];
	else if (!strcmp(code,"Unpack_Columns")) return shaders_[4];
	else if (!strcmp(code,"IDCT_RenderToBuffer")) return shaders_[5];
	else return nullptr;
}

std::vector< base::SmartPointer<Shader> > Jpeg_Data::shaders_;

Jpeg_Data::QuadGemetry Jpeg_Data::quadGemetry_;

int Jpeg_Data::allocateTextures(int width, int height)
{
	if (componentsNum == 1)
	{
		enAssert(textureData[0].intitialize(width, height, quantTables[0]));
		return 1;
	}
	else if (componentsNum == 3)
	{
		enAssert(textureData[0].intitialize(width, height, quantTables[0]));
		enAssert(textureData[1].intitialize(width / 2, height / 2, quantTables[1]));
		enAssert(textureData[2].intitialize(width / 2, height / 2, quantTables[2]));

		return 1;
	}

	return 0;
}

#define  UPDATETEXTURE(tid, bid, dataoffset)  glTextureSubImage2D(textureData[tid].pTextureDCT->getTexture(),0, box[bid].min_.x, \
						box[bid].min_.y, \
						box[bid].max_.x - box[bid].min_.x, \
						box[bid].max_.y - box[bid].min_.y, \
						imageFormat, type,pFileData + offset + dataoffset ); \
						CHECK_GL_ERROR;


void Jpeg_Data::updateTextureData(int blocksNum, int blockSize, int *pSrcCorners)
{
	int MCU_SIZE = 8;
	int MCU_BLOCK_SIZE = 64;

	int MCU_ROW;
	int MCU_COLUMN;

	if (componentsNum == 3)
	{
		MCU_SIZE = 16;
		MCU_BLOCK_SIZE = 384;
	}

	int MCU_NUM_X = blockSize / MCU_SIZE;
	int MCU_NUM_Y = blockSize / MCU_SIZE;

	int updatesNum = 0;

	if (componentsNum == 1)
	{
		base::AABB box;
		box.min_.z = 0;
		box.max_.z = 1;

		for (int i = 0; i < blocksNum; i++)
		{
			MCU_ROW = pSrcCorners[i * 2 + 1] / MCU_SIZE;
			MCU_COLUMN = pSrcCorners[i * 2] / MCU_SIZE;

			box.min_.y = 0;
			box.max_.y = 8;

			for (int j = MCU_ROW; j < MCU_ROW + MCU_NUM_Y; j++)
			{
				box.min_.x = (i * MCU_NUM_X) * 8;
				box.max_.x = box.min_.x + 8;

				for (int k = MCU_COLUMN; k < MCU_COLUMN + MCU_NUM_X; k++)
				{
					int offset = MCU_BLOCK_SIZE * (j * MCU_per_row + k);
					//pd3dDevice->UpdateSubresource(textureData[0].pTextureDCT, 0, &box, pFileData + offset, 8, 0);
					glTextureSubImage2D(textureData[0].pTextureDCT->getTexture(), 0, box.min_.x,
						box.min_.y,
						box.max_.x - box.min_.x,
						box.max_.y - box.min_.y,
						imageFormat, type, pFileData + offset);
					box.min_.x += 8;
					box.max_.x += 8;
					CHECK_GL_ERROR;
				}

				box.min_.y += 8;
				box.max_.y += 8;
			}
		}
	}
	else if (componentsNum == 3)
	{
		base::AABB box[2];
		box[0].min_.z = 0;
		box[0].max_.z = 1;

		box[1].min_.z = 0;
		box[1].max_.z = 1;

		int corner[2];

		for (int i = 0; i < blocksNum; i++)
		{
			MCU_ROW = pSrcCorners[i * 2 + 1] / MCU_SIZE;
			MCU_COLUMN = pSrcCorners[i * 2] / MCU_SIZE;

			box[1].min_.y = 0;
			box[1].max_.y = 8;

			corner[1] = 0;

			for (int j = MCU_ROW; j < MCU_ROW + MCU_NUM_Y; j++)
			{
				corner[0] = i * MCU_NUM_X * 16;

				box[1].min_.x = i * MCU_NUM_X * 8;
				box[1].max_.x = box[1].min_.x + 8;

				for (int k = MCU_COLUMN; k < MCU_COLUMN + MCU_NUM_X; k++)
				{
					int offset = MCU_BLOCK_SIZE * (j * MCU_per_row + k);

					box[0].min_.y = corner[1];
					box[0].max_.y = corner[1] + 8;

					box[0].min_.x = corner[0];
					box[0].max_.x = corner[0] + 8;

					UPDATETEXTURE(0, 0, 0)
						//pd3dDevice->UpdateSubresource(textureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 0, 8, 0);

					box[0].min_.x += 8;
					box[0].max_.x += 8;
					UPDATETEXTURE(0, 0, 64)

						//pd3dDevice->UpdateSubresource(textureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 64, 8, 0);

					box[0].min_.y += 8;
					box[0].max_.y += 8;

					box[0].min_.x = corner[0];
					box[0].max_.x = corner[0] + 8;

					UPDATETEXTURE(0, 0, 128)
						//pd3dDevice->UpdateSubresource(textureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 128, 8, 0);

					box[0].min_.x += 8;
					box[0].max_.x += 8;
					UPDATETEXTURE(0, 0, 192)
						//pd3dDevice->UpdateSubresource(textureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 192, 8, 0);
					UPDATETEXTURE(1, 1, 256)
					UPDATETEXTURE(2, 1, 320)

						//pd3dDevice->UpdateSubresource(textureData[1].pTextureDCT, 0, &box[1], pFileData + offset + 256, 8, 0);
						//	pd3dDevice->UpdateSubresource(textureData[2].pTextureDCT, 0, &box[1], pFileData + offset + 320, 8, 0);
					updatesNum++;

					box[1].min_.x += 8;
					box[1].max_.x += 8;

					corner[0] += 16;
				}

				box[1].min_.y += 8;
				box[1].max_.y += 8;

				corner[1] += 16;
			}
		}
	}

}

void Jpeg_Data::uncompressTextureData()
{
	for (int i = 0; i < componentsNum; i++)
	{
		int width = textureData[i].pTextureTarget->width();
		int height = textureData[i].pTextureTarget->heigh();

		//pass0
		Shader *shader = getTechnique("IDCT_Rows");
		shader->turnOn();
		textureData[i].rowFrameBuffer_->bindObj(true, true);
		textureData[i].rowFrameBuffer_->clearBuffer();
		shader->setFloat(shader->getVariable("g_ColScale"), height / 8.0f);
		glActiveTexture(GL_TEXTURE0);
		textureData[i].pTextureDCT->bind();
		glActiveTexture(GL_TEXTURE1);
		textureData[i].pTextureQ->bind();
		glViewport(0, 0, textureData[i].pTexture1Row->width(), textureData[i].pTexture1Row->heigh());
		quadGemetry_.draw();
		CHECK_GL_ERROR;
		
		//pass1
		shader = getTechnique("Unpack_Rows");
		shader->turnOn();
		shader->setFloat(shader->getVariable("g_RowScale"), width / 8.0f);
		textureData[i].targetFrameBuffer_->bindObj(true, true);
		textureData[i].targetFrameBuffer_->clearBuffer();
		glActiveTexture(GL_TEXTURE0);
		textureData[i].pTexture1Row->bind();
		glActiveTexture(GL_TEXTURE1);
		textureData[i].pTexture2Row->bind();
		glViewport(0, 0, textureData[i].pTextureTarget->width(), textureData[i].pTextureTarget->heigh());
		quadGemetry_.draw();
		CHECK_GL_ERROR;

		//pass2
		shader = getTechnique("IDCT_Columns");
		shader->turnOn();
		textureData[i].colFrameBuffer_->bindObj(true, true);
		textureData[i].colFrameBuffer_->clearBuffer();
		glActiveTexture(GL_TEXTURE0);
		textureData[i].pTextureTarget->bind();
		glViewport(0, 0, textureData[i].pTexture1Col->width(), textureData[i].pTexture1Col->heigh());
		quadGemetry_.draw();
		CHECK_GL_ERROR;

		//pass3
		shader = getTechnique("Unpack_Columns");
		shader->turnOn();
		shader->setFloat(shader->getVariable("g_ColScale"), height / 8.0f);
		textureData[i].targetFrameBuffer_->bindObj(true, true);
		textureData[i].targetFrameBuffer_->clearBuffer();
		glActiveTexture(GL_TEXTURE0);
		textureData[i].pTexture1Col->bind();
		glActiveTexture(GL_TEXTURE1);
		textureData[i].pTexture2Col->bind();
		glViewport(0, 0, textureData[i].pTextureTarget->width(), textureData[i].pTextureTarget->heigh());
		quadGemetry_.draw();
		CHECK_GL_ERROR;
	}
}
 