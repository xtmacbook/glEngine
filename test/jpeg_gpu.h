

#include "smartPointer.h"
#include <vector>

class Texture;
class Shader;
class FrameBufferObject;

#define MAX_CHANELS 3

class UpdateInfo
{
	int     blocksNum;      // A total number of blocks reserved for a single update step.
							// This should be at least StackSize / UpdateBorderSize
	int     curBlock;
	int     blocksAdded;
	int     *pSrcCorners;   // Values used to map data during UpdateSubresource calls
							// when we prepare data for IDCT pass.
							// Here we store only the positions of a left upper corner,
							// because update region size depends on the resource being used.
	int     *pDstCorners;   // Values used to map data during CopySubresourceRegion calls
							// when we update our clipmap stack texture with uncompressed data.

public:

	UpdateInfo();
	~UpdateInfo();
	void    initialize(int blocks);
	void    addBlock(int DstBlock[2], int SrcBlock[2]);
	int     getNewBlocksNumber() { return blocksAdded; }
	int*    getSrcBlocksPointer() { return pSrcCorners; }
	int*    getDstBlocksPointer() { return pDstCorners; }
	void    reset();
	void    release();
};

struct GPU_Data
{
	base::SmartPointer<Texture> pTextureDCT;
	base::SmartPointer<Texture> pTextureQ;

	// Row textures
	base::SmartPointer<Texture>                pTexture1Row;
	base::SmartPointer<Texture>                pTexture2Row;

	// Column textures
	base::SmartPointer<Texture>                pTexture1Col;
	base::SmartPointer<Texture>                pTexture2Col;

	base::SmartPointer<FrameBufferObject>		rowFrameBuffer_;
	base::SmartPointer<FrameBufferObject>		colFrameBuffer_;

	// Final texture
	base::SmartPointer<Texture>                pTextureTarget;

	base::SmartPointer<FrameBufferObject>		targetFrameBuffer_;


	int intitialize(int width, int height, unsigned char *pQuantTable);
	void release();
};

class Jpeg_Data
{

public:

	struct QuadGemetry
	{
		void initGemetry();
		void draw()const;
		unsigned int    vao_;
		unsigned int	vbo_;
	};

	Jpeg_Data(const char*file);

	Jpeg_Data();

	virtual ~Jpeg_Data();

	int			allocateTextures(int width, int height);

	void		updateTextureData(int blocksNum, int blockSize, int *pSrcCorners);

	void		uncompressTextureData();

	bool		loadFile(const char*file);

	int			height()const;
	int			width()const;

	static	bool	initTechnique();

	static  Shader*	 getTechnique(const char*);
	
	base::SmartPointer<Texture>  getFinalTarget(int idx)const { return textureData[idx].pTextureTarget; }

	static QuadGemetry* getQuad() { return &quadGemetry_; }
protected:
	int     imageWidth;
	int     imageHeight;
	int     componentsNum;
	int     imageFormat;
	int		imageType;

	unsigned char    quantTables[MAX_CHANELS][64];

	int     MCU_per_row;
	int     MCU_blocks_num;

	char*	pFileData = nullptr;
	void*	mappingHandle = nullptr;
	void*	fileHandle = nullptr;

	int		 type;

	GPU_Data	textureData[MAX_CHANELS];

	static std::vector< base::SmartPointer<Shader> > shaders_;
	static QuadGemetry quadGemetry_;
};

