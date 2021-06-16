
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"
#include <vector>


namespace IO
{
	//tga
	unsigned char g_ucUTGAcompare[12] = { 0, 0, 2,  0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char g_ucCTGAcompare[12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	struct TGAInformationHeader
	{
		uint8		  m_ucHeader[6];
		unsigned int  m_uiBytesPerPixel;
		unsigned int  m_uiImageSize;
		unsigned int  m_uiTemp;
		unsigned int  m_uiType;
		unsigned int  m_uiHeight;
		unsigned int  m_uiWidth;
		unsigned int  m_uiBPP;
	};
	base::Image* ImageFile::loadTga(StdInputStream * stream)
	{
		std::vector<uint8> byte_array;
		byte_array.resize(stream->size());
		stream->serializeData(byte_array.data(), stream->size());

		if (stream->isError())
		{
			Log::instance()->printMessage("read tga file is error !\n");
			return nullptr;
		}

		if (memcmp(&(byte_array[0]), g_ucUTGAcompare, 12) == 0)
		{
			//load the file using the uncompressed TGA Loader
			TGAInformationHeader pTGAinfo;
			unsigned int uiCSwap;

			memcpy(pTGAinfo.m_ucHeader, &(byte_array[12]), sizeof(uint8[6]));

			int w = pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0];
			int h = pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2];
			int band = pTGAinfo.m_ucHeader[4];

		}
		else if (memcmp(&(byte_array[0]), g_ucCTGAcompare, 12) == 0)
		{
			TGAInformationHeader pTGAinfo;
			int				ucpFile;
			unsigned char* ucpColorBuffer;
			unsigned char  ucChunkHeader;
			unsigned int   uiPixelCount;
			unsigned int   uiCurrentPixel;
			unsigned int   uiCurrentByte;
			short		   i;

			memcpy(pTGAinfo.m_ucHeader, &(byte_array[12]), sizeof(uint8[6]));
			int w = pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0];
			int h = pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2];
			int band = pTGAinfo.m_ucHeader[4];


			if ((w <= 0) || (h <= 0) || ((band != 24) && (band != 32)))
				return nullptr;

			pTGAinfo.m_uiBPP = band;
			pTGAinfo.m_uiHeight = h;
			pTGAinfo.m_uiWidth = w;

			pTGAinfo.m_uiBytesPerPixel = pTGAinfo.m_uiBPP / 8;
			pTGAinfo.m_uiImageSize = (pTGAinfo.m_uiBytesPerPixel*pTGAinfo.m_uiWidth*pTGAinfo.m_uiHeight);

			uiPixelCount = pTGAinfo.m_uiHeight * pTGAinfo.m_uiWidth;

			base::Image* img = new base::Image();

			img->allocate(pTGAinfo.m_uiWidth, pTGAinfo.m_uiHeight, pTGAinfo.m_uiBytesPerPixel);

			img->levelDataPtr_ = new uint8*[1];
			img->levelDataPtr_[0] = (uint8*)img->pixels();

			img->settarget(GL_TEXTURE_2D);
			img->setinternalformat(GL_RGB8);
			img->settype(GL_UNSIGNED_BYTE);
			img->setformat(GL_RGB);

			uint8 * data = (uint8*)img->pixels();

			uiCurrentPixel = 0;
			uiCurrentByte = 0;
			ucpColorBuffer = new uint8[pTGAinfo.m_uiBytesPerPixel];
			ucpFile = 18; // 12 + 6
			do
			{
				ucChunkHeader = byte_array[ucpFile];
				ucpFile++;

				if (ucChunkHeader < 128)
				{
					ucChunkHeader++;

					//read RAW color values
					for (i = 0; i < ucChunkHeader; i++)
					{
						memcpy(ucpColorBuffer, &(byte_array[ucpFile]), pTGAinfo.m_uiBytesPerPixel);
						ucpFile += pTGAinfo.m_uiBytesPerPixel;

						//flip R and B color values around
						data[uiCurrentByte] = ucpColorBuffer[2];
						data[uiCurrentByte + 1] = ucpColorBuffer[1];
						data[uiCurrentByte + 2] = ucpColorBuffer[0];

						if (pTGAinfo.m_uiBytesPerPixel == 4)
							data[uiCurrentByte + 3] = ucpColorBuffer[3];

						uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
						uiCurrentPixel++;

						//make sure too many pixels have not been read in
						if (uiCurrentPixel > uiPixelCount)
						{
							if (ucpColorBuffer != NULL)
								delete[] ucpColorBuffer;
							if (data != NULL)
								delete[] data;

							delete img;
							return nullptr;
						}
					}
				}

				//the chunk header is greater than 128 RLE data
				else
				{
					ucChunkHeader -= 127;

					memcpy(ucpColorBuffer, &(byte_array[ucpFile]), pTGAinfo.m_uiBytesPerPixel);
					ucpFile += pTGAinfo.m_uiBytesPerPixel;

					//copy the color into the image data as many times as needed
					for (i = 0; i < ucChunkHeader; i++)
					{
						//switch R and B bytes around while copying
						data[uiCurrentByte] = ucpColorBuffer[2];
						data[uiCurrentByte + 1] = ucpColorBuffer[1];
						data[uiCurrentByte + 2] = ucpColorBuffer[0];

						if (pTGAinfo.m_uiBytesPerPixel == 4)
							data[uiCurrentByte + 3] = ucpColorBuffer[3];

						uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
						uiCurrentPixel++;

						//make sure that we have not written too many pixels
						if (uiCurrentPixel > uiPixelCount)
						{
							if (ucpColorBuffer != NULL)
								delete[] ucpColorBuffer;
							if (data != NULL)
								delete[] data;

							delete img;
							return nullptr;
						}
					}
				}
			}

			//loop while there are still pixels left
			while (uiCurrentPixel < uiPixelCount);

			return img;
		}
	}

}
