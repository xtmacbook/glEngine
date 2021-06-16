
#include "clipmap_prepprocessor.h"
#include "debug.h"
#include "util.h"
#include "jpgd.h"
#include "resource.h"
#include "texture.h"
#include "gls.h"
#include "log.h"
#include "framebuffers.h"
#include "common.h"
#include "shader.h"
#include <iostream>
#include <algorithm>

Clipmap_Manager::Clipmap_Manager()
{
	isInitialized = false;
	levelsNum = 0;
	pimgData = nullptr;
	pUpdateBlocks = nullptr;
	ppBakedTextures = nullptr;
	ppFrameBuffers = nullptr;
}

Clipmap_Manager::~Clipmap_Manager()
{
	SAFTE_DELETE(pimgData)
	SAFTE_DELETE_ARRAY(pimgDataHM)
	SAFTE_DELETE(pUpdateBlocks)

	if(ppBakedTextures)
	{
		for (int i = 0; i < levelsNum; i++)
		{
			SAFTE_DELETE(ppBakedTextures[i])
		}
		SAFTE_DELETE_ARRAY(ppBakedTextures);
	}
	if (ppFrameBuffers)
	{
		for (int i = 0; i < levelsNum; i++)
		{
			SAFTE_DELETE(ppFrameBuffers[i])
		}
		SAFTE_DELETE_ARRAY(ppFrameBuffers);
	}
}

bool Clipmap_Manager::intitialize(int levels, char fileNames[][260], char fileNamesHM[][260])
{
	if (!isInitialized)
	{
		if (!levels)
			return false;

		levelsNum = levels;

		pimgData = new Image_Data[levelsNum];
		pUpdateBlocks = new UpdateInfo[levelsNum];

		if (fileNamesHM)
		{
			pimgDataHM = new Image_Data[levelsNum];
		}

		for (int i = 0; i < levelsNum; i++)
		{
			enAssert(pimgData[i].loadFile((get_texture_BasePath() + fileNames[i]).c_str()));

			if (fileNamesHM)
				enAssert(pimgDataHM[i].loadFile( (get_texture_BasePath() + fileNamesHM[i]).c_str()));

		}

		isInitialized = true;

		Image_Data::initTechnique();
	}

	return true;
}

void Clipmap_Manager::addBlock(int level, int srcBlock[2], int dstBlock[2])
{
	if (level < levelsNum)
	{
		pUpdateBlocks[level].addBlock(srcBlock, dstBlock);
	}
}

void Clipmap_Manager::allocateBlocks(int blocksNum)
{
	for (int i = 0; i < levelsNum; i++)
	{
		pUpdateBlocks[i].initialize(int(blocksNum * pow(2.0, i)));
	}
}

void Clipmap_Manager::update(int level, base::SmartPointer<Texture> stackTexture)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	if (level < levelsNum)
	{
		int blocksNum = pUpdateBlocks[level].getNewBlocksNumber();

		// Process diffuse texture
		pimgData[level].updateTextureData( blocksNum, blockSize >> level, pUpdateBlocks[level].getSrcBlocksPointer());
		pimgData[level].uncompressTextureData();
		pUpdateBlocks[level].reset();

		ppFrameBuffers[level]->bindObj(true, true);
		ppFrameBuffers[level]->clearBuffer();
		glViewport(0, 0, ppBakedTextures[level]->width(), ppBakedTextures[level]->heigh());
		Shader * shader = Jpeg_Data::getTechnique("IDCT_RenderToBuffer");
		shader->turnOn();
		glActiveTexture(GL_TEXTURE0);
		pimgData[level].getFinalTarget(0)->bind();
		glActiveTexture(GL_TEXTURE1);
		pimgData[level].getFinalTarget(1)->bind();
		glActiveTexture(GL_TEXTURE2);
		pimgData[level].getFinalTarget(2)->bind();
		Jpeg_Data::getQuad()->draw();
		ppFrameBuffers[level]->bindObj(false, true);

		//copy texture to stacktexture

		int *pDstBlocks = pUpdateBlocks[level].getDstBlocksPointer();
		int levelBlockSize = blockSize >> level;

		int srcX = 0;
		int srcY = 0;

		// Copy fully unpacked data to the clipmap stack texture
		for (int i = 0; i < blocksNum; i++)
		{
			
			glCopyImageSubData(ppBakedTextures[level]->getTexture(),
				GL_TEXTURE_2D,
				0,
				srcX,
				srcY,
				0,
				stackTexture->getTexture(),
				GL_TEXTURE_2D_ARRAY,
				0,
				pDstBlocks[i * 2],
				pDstBlocks[i * 2 + 1],
				level,
				levelBlockSize, 
				levelBlockSize, 
				1); 

			//CHECK_GL_ERROR;
			srcX += levelBlockSize;

			//std::cout << "copy from: " << srcX << " " << srcY << "to: " << pDstBlocks[i * 2] << " " << pDstBlocks[i * 2 + 1] << std::endl;
		}

	}
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

int Clipmap_Manager::allocateTextures(int stackSize, int border)
{

	textureSize = stackSize;
	blockSize = border;

	ppBakedTextures = new Texture*[levelsNum];
	ppFrameBuffers = new FrameBufferObject*[levelsNum];

	for (int i = 0; i < levelsNum; i++)
	{
		enAssert((pimgData[i].allocateTextures(stackSize, border)));
		if(pimgDataHM)enAssert((pimgDataHM[i].allocateTextures(stackSize, border)));

		ppBakedTextures[i] = new Texture;
		ppBakedTextures[i]->width_ = stackSize;
		ppBakedTextures[i]->height_ = border;
		ppBakedTextures[i]->internalformat_ = GL_RGBA8;
		ppBakedTextures[i]->format_ = GL_RGBA;
		ppBakedTextures[i]->type_ = GL_HALF_FLOAT;
		ppBakedTextures[i]->createObj();
		ppBakedTextures[i]->bind();
		ppBakedTextures[i]->mirrorRepeat();
		ppBakedTextures[i]->filterLinear();
		ppBakedTextures[i]->contextNULL();
		ppBakedTextures[i]->unBind();

		ppFrameBuffers[i] = new FrameBufferObject(GL_FRAMEBUFFER);
		ppFrameBuffers[i]->bindObj(true, false);
		ppFrameBuffers[i]->colorTextureAttachments(ppBakedTextures[i]);
		ppFrameBuffers[i]->bindObj(false, false);
		CHECK_GL_ERROR;

		border >>= 1;
	}

	return 0;
}


 
