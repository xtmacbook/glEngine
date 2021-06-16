
#include <smartPointer.h>
#include "jpeg_gpu.h"

class Texture;
class FrameBufferObject;




class Image_Data :public Jpeg_Data
{
public:

	
};

class Clipmap_Manager
{
	bool        isInitialized;
	int         levelsNum;      // Preprocessed levels number usde for a clipmap stack update. This should be at least as much as a stack depth
	
	int         textureSize;    // The biggest size of 0 level.
	int         blockSize;      // Updatable region size for 0 level.

	Image_Data   *pimgData = nullptr;
	Image_Data   *pimgDataHM = nullptr;   // Used to store additional height maps values
	UpdateInfo  *pUpdateBlocks;
	Texture **ppBakedTextures;  // This textures are used at a final pass and they hold
										// an uncomprssed data ready for a clipmap stack texture update

	FrameBufferObject ** ppFrameBuffers;

public:

	Clipmap_Manager();
	virtual ~Clipmap_Manager();
	
	bool intitialize(int levels, char fileNames[][260], char fileNamesHM[][260]);

	void        addBlock(int level, int srcBlock[2], int dstBlock[2]);

	void	allocateBlocks(int blocksNum);

	void	update(int level,base::SmartPointer<Texture> stackTexture);

	int		allocateTextures(int stackSize, int borderSize);
};