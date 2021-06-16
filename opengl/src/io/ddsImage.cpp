
#include "imgLoad.h"
#include "image.h"
#include "gls.h"
#include "log.h"

namespace IO {
	//
	//  Structure defines and constants from nvdds
	//
	//////////////////////////////////////////////////////////////////////

	// surface description flags
	const uint DDSF_CAPS = 0x00000001l;
	const uint DDSF_HEIGHT = 0x00000002l;
	const uint DDSF_WIDTH = 0x00000004l;
	const uint DDSF_PITCH = 0x00000008l;
	const uint DDSF_PIXELFORMAT = 0x00001000l;
	const uint DDSF_MIPMAPCOUNT = 0x00020000l;
	const uint DDSF_LINEARSIZE = 0x00080000l;
	const uint DDSF_DEPTH = 0x00800000l;

	// pixel format flags
	const uint DDSF_ALPHAPIXELS = 0x00000001l;
	const uint DDSF_FOURCC = 0x00000004l;
	const uint DDSF_RGB = 0x00000040l;
	const uint DDSF_RGBA = 0x00000041l;

	// dwCaps1 flags
	const uint DDSF_COMPLEX = 0x00000008l;
	const uint DDSF_TEXTURE = 0x00001000l;
	const uint DDSF_MIPMAP = 0x00400000l;

	// dwCaps2 flags
	const uint DDSF_CUBEMAP = 0x00000200l;
	const uint DDSF_CUBEMAP_POSITIVEX = 0x00000400l;
	const uint DDSF_CUBEMAP_NEGATIVEX = 0x00000800l;
	const uint DDSF_CUBEMAP_POSITIVEY = 0x00001000l;
	const uint DDSF_CUBEMAP_NEGATIVEY = 0x00002000l;
	const uint DDSF_CUBEMAP_POSITIVEZ = 0x00004000l;
	const uint DDSF_CUBEMAP_NEGATIVEZ = 0x00008000l;
	const uint DDSF_CUBEMAP_ALL_FACES = 0x0000FC00l;
	const uint DDSF_VOLUME = 0x00200000l;

	// compressed texture types
	const uint FOURCC_UNKNOWN = 0;

#ifndef MAKEFOURCC
#define MAKEFOURCC(c0,c1,c2,c3) \
    ((uint)(uint8)(c0)| \
    ((uint)(uint8)(c1) << 8)| \
    ((uint)(uint8)(c2) << 16)| \
    ((uint)(uint8)(c3) << 24))
#endif

	const uint FOURCC_R8G8B8 = 20;
	const uint FOURCC_A8R8G8B8 = 21;
	const uint FOURCC_X8R8G8B8 = 22;
	const uint FOURCC_R5G6B5 = 23;
	const uint FOURCC_X1R5G5B5 = 24;
	const uint FOURCC_A1R5G5B5 = 25;
	const uint FOURCC_A4R4G4B4 = 26;
	const uint FOURCC_R3G3B2 = 27;
	const uint FOURCC_A8 = 28;
	const uint FOURCC_A8R3G3B2 = 29;
	const uint FOURCC_X4R4G4B4 = 30;
	const uint FOURCC_A2B10G10R10 = 31;
	const uint FOURCC_A8B8G8R8 = 32;
	const uint FOURCC_X8B8G8R8 = 33;
	const uint FOURCC_G16R16 = 34;
	const uint FOURCC_A2R10G10B10 = 35;
	const uint FOURCC_A16B16G16R16 = 36;

	const uint FOURCC_L8 = 50;
	const uint FOURCC_A8L8 = 51;
	const uint FOURCC_A4L4 = 52;
	const uint FOURCC_DXT1 = 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
	const uint FOURCC_DXT2 = 0x32545844l; //(MAKEFOURCC('D','X','T','1'))
	const uint FOURCC_DXT3 = 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
	const uint FOURCC_DXT4 = 0x34545844l; //(MAKEFOURCC('D','X','T','3'))
	const uint FOURCC_DXT5 = 0x35545844l; //(MAKEFOURCC('D','X','T','5'))
	const uint FOURCC_ATI1 = MAKEFOURCC('A', 'T', 'I', '1');
	const uint FOURCC_ATI2 = MAKEFOURCC('A', 'T', 'I', '2');
	const uint FOURCC_BC4U = MAKEFOURCC('B', 'C', '4', 'U');
	const uint FOURCC_BC4S = MAKEFOURCC('B', 'C', '4', 'S');
	const uint FOURCC_BC5S = MAKEFOURCC('B', 'C', '5', 'S');

	const uint FOURCC_D16_LOCKABLE = 70;
	const uint FOURCC_D32 = 71;
	const uint FOURCC_D24X8 = 77;
	const uint FOURCC_D16 = 80;

	const uint FOURCC_D32F_LOCKABLE = 82;

	const uint FOURCC_L16 = 81;

	const uint FOURCC_DX10 = MAKEFOURCC('D', 'X', '1', '0');

	// signed normalized formats
	const uint FOURCC_Q16W16V16U16 = 110;

	// Floating point surface formats

	// s10e5 formats (16-bits per channel)
	const uint FOURCC_R16F = 111;
	const uint FOURCC_G16R16F = 112;
	const uint FOURCC_A16B16G16R16F = 113;

	// IEEE s23e8 formats (32-bits per channel)
	const uint FOURCC_R32F = 114;
	const uint FOURCC_G32R32F = 115;
	const uint FOURCC_A32B32G32R32F = 116;

	//DXGI enums
	const uint DDS10_FORMAT_UNKNOWN = 0;
	const uint DDS10_FORMAT_R32G32B32A32_TYPELESS = 1;
	const uint DDS10_FORMAT_R32G32B32A32_FLOAT = 2;
	const uint DDS10_FORMAT_R32G32B32A32_UINT = 3;
	const uint DDS10_FORMAT_R32G32B32A32_SINT = 4;
	const uint DDS10_FORMAT_R32G32B32_TYPELESS = 5;
	const uint DDS10_FORMAT_R32G32B32_FLOAT = 6;
	const uint DDS10_FORMAT_R32G32B32_UINT = 7;
	const uint DDS10_FORMAT_R32G32B32_SINT = 8;
	const uint DDS10_FORMAT_R16G16B16A16_TYPELESS = 9;
	const uint DDS10_FORMAT_R16G16B16A16_FLOAT = 10;
	const uint DDS10_FORMAT_R16G16B16A16_UNORM = 11;
	const uint DDS10_FORMAT_R16G16B16A16_UINT = 12;
	const uint DDS10_FORMAT_R16G16B16A16_SNORM = 13;
	const uint DDS10_FORMAT_R16G16B16A16_SINT = 14;
	const uint DDS10_FORMAT_R32G32_TYPELESS = 15;
	const uint DDS10_FORMAT_R32G32_FLOAT = 16;
	const uint DDS10_FORMAT_R32G32_UINT = 17;
	const uint DDS10_FORMAT_R32G32_SINT = 18;
	const uint DDS10_FORMAT_R32G8X24_TYPELESS = 19;
	const uint DDS10_FORMAT_D32_FLOAT_S8X24_UINT = 20;
	const uint DDS10_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21;
	const uint DDS10_FORMAT_X32_TYPELESS_G8X24_UINT = 22;
	const uint DDS10_FORMAT_R10G10B10A2_TYPELESS = 23;
	const uint DDS10_FORMAT_R10G10B10A2_UNORM = 24;
	const uint DDS10_FORMAT_R10G10B10A2_UINT = 25;
	const uint DDS10_FORMAT_R11G11B10_FLOAT = 26;
	const uint DDS10_FORMAT_R8G8B8A8_TYPELESS = 27;
	const uint DDS10_FORMAT_R8G8B8A8_UNORM = 28;
	const uint DDS10_FORMAT_R8G8B8A8_UNORM_SRGB = 29;
	const uint DDS10_FORMAT_R8G8B8A8_UINT = 30;
	const uint DDS10_FORMAT_R8G8B8A8_SNORM = 31;
	const uint DDS10_FORMAT_R8G8B8A8_SINT = 32;
	const uint DDS10_FORMAT_R16G16_TYPELESS = 33;
	const uint DDS10_FORMAT_R16G16_FLOAT = 34;
	const uint DDS10_FORMAT_R16G16_UNORM = 35;
	const uint DDS10_FORMAT_R16G16_UINT = 36;
	const uint DDS10_FORMAT_R16G16_SNORM = 37;
	const uint DDS10_FORMAT_R16G16_SINT = 38;
	const uint DDS10_FORMAT_R32_TYPELESS = 39;
	const uint DDS10_FORMAT_D32_FLOAT = 40;
	const uint DDS10_FORMAT_R32_FLOAT = 41;
	const uint DDS10_FORMAT_R32_UINT = 42;
	const uint DDS10_FORMAT_R32_SINT = 43;
	const uint DDS10_FORMAT_R24G8_TYPELESS = 44;
	const uint DDS10_FORMAT_D24_UNORM_S8_UINT = 45;
	const uint DDS10_FORMAT_R24_UNORM_X8_TYPELESS = 46;
	const uint DDS10_FORMAT_X24_TYPELESS_G8_UINT = 47;
	const uint DDS10_FORMAT_R8G8_TYPELESS = 48;
	const uint DDS10_FORMAT_R8G8_UNORM = 49;
	const uint DDS10_FORMAT_R8G8_UINT = 50;
	const uint DDS10_FORMAT_R8G8_SNORM = 51;
	const uint DDS10_FORMAT_R8G8_SINT = 52;
	const uint DDS10_FORMAT_R16_TYPELESS = 53;
	const uint DDS10_FORMAT_R16_FLOAT = 54;
	const uint DDS10_FORMAT_D16_UNORM = 55;
	const uint DDS10_FORMAT_R16_UNORM = 56;
	const uint DDS10_FORMAT_R16_UINT = 57;
	const uint DDS10_FORMAT_R16_SNORM = 58;
	const uint DDS10_FORMAT_R16_SINT = 59;
	const uint DDS10_FORMAT_R8_TYPELESS = 60;
	const uint DDS10_FORMAT_R8_UNORM = 61;
	const uint DDS10_FORMAT_R8_UINT = 62;
	const uint DDS10_FORMAT_R8_SNORM = 63;
	const uint DDS10_FORMAT_R8_SINT = 64;
	const uint DDS10_FORMAT_A8_UNORM = 65;
	const uint DDS10_FORMAT_R1_UNORM = 66;
	const uint DDS10_FORMAT_R9G9B9E5_SHAREDEXP = 67;
	const uint DDS10_FORMAT_R8G8_B8G8_UNORM = 68;
	const uint DDS10_FORMAT_G8R8_G8B8_UNORM = 69;
	const uint DDS10_FORMAT_BC1_TYPELESS = 70;
	const uint DDS10_FORMAT_BC1_UNORM = 71;
	const uint DDS10_FORMAT_BC1_UNORM_SRGB = 72;
	const uint DDS10_FORMAT_BC2_TYPELESS = 73;
	const uint DDS10_FORMAT_BC2_UNORM = 74;
	const uint DDS10_FORMAT_BC2_UNORM_SRGB = 75;
	const uint DDS10_FORMAT_BC3_TYPELESS = 76;
	const uint DDS10_FORMAT_BC3_UNORM = 77;
	const uint DDS10_FORMAT_BC3_UNORM_SRGB = 78;
	const uint DDS10_FORMAT_BC4_TYPELESS = 79;
	const uint DDS10_FORMAT_BC4_UNORM = 80;
	const uint DDS10_FORMAT_BC4_SNORM = 81;
	const uint DDS10_FORMAT_BC5_TYPELESS = 82;
	const uint DDS10_FORMAT_BC5_UNORM = 83;
	const uint DDS10_FORMAT_BC5_SNORM = 84;
	const uint DDS10_FORMAT_B5G6R5_UNORM = 85;
	const uint DDS10_FORMAT_B5G5R5A1_UNORM = 86;
	const uint DDS10_FORMAT_B8G8R8A8_UNORM = 87;
	const uint DDS10_FORMAT_B8G8R8X8_UNORM = 88;
	const uint DDS10_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89;
	const uint DDS10_FORMAT_B8G8R8A8_TYPELESS = 90;
	const uint DDS10_FORMAT_B8G8R8A8_UNORM_SRGB = 91;
	const uint DDS10_FORMAT_B8G8R8X8_TYPELESS = 92;
	const uint DDS10_FORMAT_B8G8R8X8_UNORM_SRGB = 93;
	const uint DDS10_FORMAT_BC6H_TYPELESS = 94;
	const uint DDS10_FORMAT_BC6H_UF16 = 95;
	const uint DDS10_FORMAT_BC6H_SF16 = 96;
	const uint DDS10_FORMAT_BC7_TYPELESS = 97;
	const uint DDS10_FORMAT_BC7_UNORM = 98;
	const uint DDS10_FORMAT_BC7_UNORM_SRGB = 99;
	const uint DDS10_FORMAT_FORCE_UINT = 0xffffffffUL;
	// ASTC extension
	const uint DDS10_FORMAT_ASTC_4X4_UNORM = 134;
	const uint DDS10_FORMAT_ASTC_4X4_UNORM_SRGB = 135;
	const uint DDS10_FORMAT_ASTC_5X4_TYPELESS = 137;
	const uint DDS10_FORMAT_ASTC_5X4_UNORM = 138;
	const uint DDS10_FORMAT_ASTC_5X4_UNORM_SRGB = 139;
	const uint DDS10_FORMAT_ASTC_5X5_TYPELESS = 141;
	const uint DDS10_FORMAT_ASTC_5X5_UNORM = 142;
	const uint DDS10_FORMAT_ASTC_5X5_UNORM_SRGB = 143;
	const uint DDS10_FORMAT_ASTC_6X5_TYPELESS = 145;
	const uint DDS10_FORMAT_ASTC_6X5_UNORM = 146;
	const uint DDS10_FORMAT_ASTC_6X5_UNORM_SRGB = 147;
	const uint DDS10_FORMAT_ASTC_6X6_TYPELESS = 149;
	const uint DDS10_FORMAT_ASTC_6X6_UNORM = 150;
	const uint DDS10_FORMAT_ASTC_6X6_UNORM_SRGB = 151;
	const uint DDS10_FORMAT_ASTC_8X5_TYPELESS = 153;
	const uint DDS10_FORMAT_ASTC_8X5_UNORM = 154;
	const uint DDS10_FORMAT_ASTC_8X5_UNORM_SRGB = 155;
	const uint DDS10_FORMAT_ASTC_8X6_TYPELESS = 157;
	const uint DDS10_FORMAT_ASTC_8X6_UNORM = 158;
	const uint DDS10_FORMAT_ASTC_8X6_UNORM_SRGB = 159;
	const uint DDS10_FORMAT_ASTC_8X8_TYPELESS = 161;
	const uint DDS10_FORMAT_ASTC_8X8_UNORM = 162;
	const uint DDS10_FORMAT_ASTC_8X8_UNORM_SRGB = 163;
	const uint DDS10_FORMAT_ASTC_10X5_TYPELESS = 165;
	const uint DDS10_FORMAT_ASTC_10X5_UNORM = 166;
	const uint DDS10_FORMAT_ASTC_10X5_UNORM_SRGB = 167;
	const uint DDS10_FORMAT_ASTC_10X6_TYPELESS = 169;
	const uint DDS10_FORMAT_ASTC_10X6_UNORM = 170;
	const uint DDS10_FORMAT_ASTC_10X6_UNORM_SRGB = 171;
	const uint DDS10_FORMAT_ASTC_10X8_TYPELESS = 173;
	const uint DDS10_FORMAT_ASTC_10X8_UNORM = 174;
	const uint DDS10_FORMAT_ASTC_10X8_UNORM_SRGB = 175;
	const uint DDS10_FORMAT_ASTC_10X10_TYPELESS = 177;
	const uint DDS10_FORMAT_ASTC_10X10_UNORM = 178;
	const uint DDS10_FORMAT_ASTC_10X10_UNORM_SRGB = 179;
	const uint DDS10_FORMAT_ASTC_12X10_TYPELESS = 181;
	const uint DDS10_FORMAT_ASTC_12X10_UNORM = 182;
	const uint DDS10_FORMAT_ASTC_12X10_UNORM_SRGB = 183;
	const uint DDS10_FORMAT_ASTC_12X12_TYPELESS = 185;
	const uint DDS10_FORMAT_ASTC_12X12_UNORM = 186;
	const uint DDS10_FORMAT_ASTC_12X12_UNORM_SRGB = 187;



	//DDS 10 resource dimension enums
	const uint DDS10_RESOURCE_DIMENSION_UNKNOWN = 0;
	const uint DDS10_RESOURCE_DIMENSION_BUFFER = 1;
	const uint DDS10_RESOURCE_DIMENSION_TEXTURE1D = 2;
	const uint DDS10_RESOURCE_DIMENSION_TEXTURE2D = 3;
	const uint DDS10_RESOURCE_DIMENSION_TEXTURE3D = 4;

	struct DXTColBlock
	{
		uint16 col0;
		uint16 col1;

		uint8 row[4];
	};

	struct DXT3AlphaBlock
	{
		uint16 row[4];
	};

	struct DXT5AlphaBlock
	{
		uint8 alpha0;
		uint8 alpha1;

		uint8 row[6];
	};

	struct DDS_PIXELFORMAT
	{
		uint dwSize;
		uint dwFlags;
		uint dwFourCC;
		uint dwRGBBitCount;
		uint dwRBitMask;
		uint dwGBitMask;
		uint dwBBitMask;
		uint dwABitMask;
	};


	struct DDS_HEADER
	{
		uint dwSize;
		uint dwFlags;
		uint dwHeight;
		uint dwWidth;
		uint dwPitchOrLinearSize;
		uint dwDepth;
		uint dwMipMapCount;
		uint dwReserved1[11];
		DDS_PIXELFORMAT ddspf;
		uint dwCaps1;
		uint dwCaps2;
		uint dwReserved2[3];
	};

	struct DDS_HEADER_10
	{
		uint dxgiFormat;  // check type
		uint resourceDimension; //check type
		uint miscFlag;
		uint arraySize;
		uint reserved;
	};

	bool vertFlip = true;
	bool supportsBGR = true;
	bool auto_expandDXT = true;

	bool TranslateDX10Format(const void *ptr, ::base::Image *img, int32 &bytesPerElement, bool &btcCompressed) {
		const DDS_HEADER_10 &header = *(const DDS_HEADER_10*)ptr;

#ifdef _DEBUG
		LOGI("translating DX10 Format\n");
		LOGI("  header.dxgiFormat = %x\n", header.dxgiFormat);
		LOGI("  header.resourceDimension = %x\n", header.resourceDimension);
		LOGI("  header.arraySize = %x\n", header.arraySize);
		LOGI("  header.miscFlag = %x\n", header.miscFlag);
#endif

		switch (header.resourceDimension) {
		case DDS10_RESOURCE_DIMENSION_TEXTURE1D:
		case DDS10_RESOURCE_DIMENSION_TEXTURE2D:
		case DDS10_RESOURCE_DIMENSION_TEXTURE3D:
			//do I really need to do anything here ?
			break;
		case DDS10_RESOURCE_DIMENSION_UNKNOWN:
		case DDS10_RESOURCE_DIMENSION_BUFFER:
		default:
			// these are presently unsupported formats
			LOGE("Bad resource dimension\n");
			return false;
		};


#define SET_TYPE_INFO( intf, f, t, size) \
    img->setinternalformat(intf); \
    img->setformat(f); \
    img->settype(t); \
    bytesPerElement = size; \
    btcCompressed = false;

#define SET_COMPRESSED_TYPE_INFO( intf, f, t, size) \
    img->setinternalformat(intf); \
    img->setformat(f);\
    img->settype(t); \
    bytesPerElement = size; \
    btcCompressed = true;

		img->blockSize_x_ = 4;
		img->blockSize_y_ = 4;

		switch (header.dxgiFormat) {
		case DDS10_FORMAT_R32G32B32A32_FLOAT:
			SET_TYPE_INFO(GL_RGBA32F, GL_RGBA, GL_FLOAT, 16);
			break;

		case DDS10_FORMAT_R32G32B32A32_UINT:
			SET_TYPE_INFO(GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 16);
			break;

		case DDS10_FORMAT_R32G32B32A32_SINT:
			SET_TYPE_INFO(GL_RGBA32I, GL_RGBA_INTEGER, GL_INT, 16);
			break;

		case DDS10_FORMAT_R32G32B32_FLOAT:
			SET_TYPE_INFO(GL_RGBA32F, GL_RGB, GL_FLOAT, 12);
			break;

		case DDS10_FORMAT_R32G32B32_UINT:
			SET_TYPE_INFO(GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, 12);
			break;

		case DDS10_FORMAT_R32G32B32_SINT:
			SET_TYPE_INFO(GL_RGB32I, GL_RGB_INTEGER, GL_INT, 12);
			break;

		case DDS10_FORMAT_R16G16B16A16_FLOAT:
			SET_TYPE_INFO(GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 8);
			break;

		case DDS10_FORMAT_R16G16B16A16_UNORM:
			SET_TYPE_INFO(GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, 8);
			break;

		case DDS10_FORMAT_R16G16B16A16_UINT:
			SET_TYPE_INFO(GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, 8);
			break;

		case DDS10_FORMAT_R16G16B16A16_SNORM:
			SET_TYPE_INFO(GL_RGBA16_SNORM, GL_RGBA, GL_SHORT, 8);
			break;

		case DDS10_FORMAT_R16G16B16A16_SINT:
			SET_TYPE_INFO(GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT, 8);
			break;

		case DDS10_FORMAT_R32G32_FLOAT:
			SET_TYPE_INFO(GL_RG32F, GL_RG, GL_FLOAT, 8);
			break;

		case DDS10_FORMAT_R32G32_UINT:
			SET_TYPE_INFO(GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, 8);
			break;

		case DDS10_FORMAT_R32G32_SINT:
			SET_TYPE_INFO(GL_RG32I, GL_RG_INTEGER, GL_INT, 8);
			break;

		case DDS10_FORMAT_R32G8X24_TYPELESS:
		case DDS10_FORMAT_D32_FLOAT_S8X24_UINT:
		case DDS10_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DDS10_FORMAT_X32_TYPELESS_G8X24_UINT:
			//these formats have no real direct mapping to OpenGL
			// fail creation
			return false;

		case DDS10_FORMAT_R10G10B10A2_UNORM:
			SET_TYPE_INFO(GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, 4); // is the rev version needed?
			break;

		case DDS10_FORMAT_R10G10B10A2_UINT:
			//doesn't exist in OpenGL
			return false;

		case DDS10_FORMAT_R11G11B10_FLOAT:
			SET_TYPE_INFO(GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, 4);
			break;

		case DDS10_FORMAT_R8G8B8A8_UNORM:
			SET_TYPE_INFO(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4);
			break;

		case DDS10_FORMAT_R8G8B8A8_UNORM_SRGB:
			SET_TYPE_INFO(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, 4);
			break;

		case DDS10_FORMAT_R8G8B8A8_UINT:
			SET_TYPE_INFO(GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, 4);
			break;

		case DDS10_FORMAT_R8G8B8A8_SNORM:
			SET_TYPE_INFO(GL_RGBA8_SNORM, GL_RGBA, GL_BYTE, 4);
			break;

		case DDS10_FORMAT_R8G8B8A8_SINT:
			SET_TYPE_INFO(GL_RGBA8UI, GL_RGBA_INTEGER, GL_BYTE, 4);
			break;

		case DDS10_FORMAT_R16G16_FLOAT:
			SET_TYPE_INFO(GL_RG16F, GL_RG, GL_HALF_FLOAT, 4);
			break;

		case DDS10_FORMAT_R16G16_UNORM:
			SET_TYPE_INFO(GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 4);
			break;

		case DDS10_FORMAT_R16G16_UINT:
			SET_TYPE_INFO(GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, 4);
			break;

		case DDS10_FORMAT_R16G16_SNORM:
			SET_TYPE_INFO(GL_RG16_SNORM, GL_RG, GL_SHORT, 4);
			break;

		case DDS10_FORMAT_R16G16_SINT:
			SET_TYPE_INFO(GL_RG16I, GL_RG_INTEGER, GL_SHORT, 4);
			break;

		case DDS10_FORMAT_D32_FLOAT:
			SET_TYPE_INFO(GL_DEPTH_COMPONENT32F, GL_DEPTH, GL_FLOAT, 4);
			break;

		case DDS10_FORMAT_R32_FLOAT:
			SET_TYPE_INFO(GL_R32F, GL_RED, GL_FLOAT, 4);
			break;

		case DDS10_FORMAT_R32_UINT:
			SET_TYPE_INFO(GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 4);
			break;

		case DDS10_FORMAT_R32_SINT:
			SET_TYPE_INFO(GL_R32I, GL_RED_INTEGER, GL_INT, 4);
			break;

			//these seem a little problematic to deal with
		case DDS10_FORMAT_R24G8_TYPELESS:
		case DDS10_FORMAT_D24_UNORM_S8_UINT:
		case DDS10_FORMAT_R24_UNORM_X8_TYPELESS:
		case DDS10_FORMAT_X24_TYPELESS_G8_UINT:
			//OpenGL doesn't really offer a packed depth stencil textures
			return false;

		case DDS10_FORMAT_R8G8_UNORM:
			SET_TYPE_INFO(GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 2);
			break;

		case DDS10_FORMAT_R8G8_UINT:
			SET_TYPE_INFO(GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, 2);
			break;

		case DDS10_FORMAT_R8G8_SNORM:
			SET_TYPE_INFO(GL_RG8_SNORM, GL_RG, GL_BYTE, 2);
			break;

		case DDS10_FORMAT_R8G8_SINT:
			SET_TYPE_INFO(GL_RG8I, GL_RG_INTEGER, GL_BYTE, 2);
			break;

		case DDS10_FORMAT_R16_FLOAT:
			SET_TYPE_INFO(GL_R16F, GL_RED, GL_HALF_FLOAT, 2);
			break;

		case DDS10_FORMAT_D16_UNORM:
			SET_TYPE_INFO(GL_DEPTH_COMPONENT16, GL_DEPTH, GL_UNSIGNED_SHORT, 2);
			break;

		case DDS10_FORMAT_R16_UNORM:
			SET_TYPE_INFO(GL_R16, GL_RED, GL_UNSIGNED_SHORT, 2);
			break;

		case DDS10_FORMAT_R16_UINT:
			SET_TYPE_INFO(GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, 2);
			break;

		case DDS10_FORMAT_R16_SNORM:
			SET_TYPE_INFO(GL_R16_SNORM, GL_RED, GL_SHORT, 2);
			return false;

		case DDS10_FORMAT_R16_SINT:
			SET_TYPE_INFO(GL_R16I, GL_RED_INTEGER, GL_SHORT, 2);
			break;

		case DDS10_FORMAT_R8_UNORM:
			SET_TYPE_INFO(GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1);
			break;

		case DDS10_FORMAT_R8_UINT:
			SET_TYPE_INFO(GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 1);
			break;

		case DDS10_FORMAT_R8_SNORM:
			SET_TYPE_INFO(GL_R8_SNORM, GL_RED, GL_BYTE, 1);
			break;

		case DDS10_FORMAT_R8_SINT:
			SET_TYPE_INFO(GL_R8I, GL_RED_INTEGER, GL_BYTE, 1);
			break;

		case DDS10_FORMAT_A8_UNORM:
			SET_TYPE_INFO(GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE, 1);
			break;

		case DDS10_FORMAT_R9G9B9E5_SHAREDEXP:
			SET_TYPE_INFO(GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, 4);
			break;

		case DDS10_FORMAT_R8G8_B8G8_UNORM:
		case DDS10_FORMAT_G8R8_G8B8_UNORM:
			// unsure how to interpret these formats
			return false;

		case DDS10_FORMAT_BC1_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 8);
			break;

		case DDS10_FORMAT_BC1_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 8);
			break;

		case DDS10_FORMAT_BC2_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_BC2_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_BC3_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_BC3_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_BC4_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1, GL_UNSIGNED_BYTE, 8);
			break;

		case DDS10_FORMAT_BC4_SNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_UNSIGNED_BYTE, 8);
			break;

		case DDS10_FORMAT_BC5_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_BC5_SNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2, GL_UNSIGNED_BYTE, 16);
			break;

		case DDS10_FORMAT_B5G6R5_UNORM:
			SET_TYPE_INFO(GL_RGB5, GL_BGR, GL_UNSIGNED_SHORT_5_6_5, 2);
			break;

		case DDS10_FORMAT_B5G5R5A1_UNORM:
			SET_TYPE_INFO(GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1, 2);
			break;

		case DDS10_FORMAT_B8G8R8A8_UNORM:
			SET_TYPE_INFO(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE, 2);
			break;

		case DDS10_FORMAT_B8G8R8X8_UNORM:
			SET_TYPE_INFO(GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE, 4);
			break;

		case DDS10_FORMAT_B8G8R8A8_UNORM_SRGB:
			SET_TYPE_INFO(GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE, 4);
			break;

		case DDS10_FORMAT_B8G8R8X8_UNORM_SRGB:
			SET_TYPE_INFO(GL_SRGB8, GL_BGRA, GL_UNSIGNED_BYTE, 4);
			break;
		case DDS10_FORMAT_ASTC_4X4_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_UNSIGNED_BYTE, 16);
			break;
		case DDS10_FORMAT_ASTC_4X4_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_UNSIGNED_BYTE, 16);
			break;
		case DDS10_FORMAT_ASTC_5X4_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 5;
			break;
		case DDS10_FORMAT_ASTC_5X4_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 5;
			break;
		case DDS10_FORMAT_ASTC_5X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 5;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_5X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 5;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_6X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 6;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_6X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 6;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_6X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 6;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_6X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 6;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_8X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_8X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_8X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_8X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_8X8_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 8;
			break;
		case DDS10_FORMAT_ASTC_8X8_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 8;
			img->blockSize_y_ = 8;
			break;
		case DDS10_FORMAT_ASTC_10X5_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_10X5_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 5;
			break;
		case DDS10_FORMAT_ASTC_10X6_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_10X6_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 6;
			break;
		case DDS10_FORMAT_ASTC_10X8_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 8;
			break;
		case DDS10_FORMAT_ASTC_10X8_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 8;
			break;
		case DDS10_FORMAT_ASTC_10X10_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 10;
			break;
		case DDS10_FORMAT_ASTC_10X10_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 10;
			img->blockSize_y_ = 10;
			break;
		case DDS10_FORMAT_ASTC_12X10_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 12;
			img->blockSize_y_ = 10;
			break;
		case DDS10_FORMAT_ASTC_12X10_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 12;
			img->blockSize_y_ = 10;
			break;
		case DDS10_FORMAT_ASTC_12X12_UNORM:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 12;
			img->blockSize_y_ = 12;
			break;
		case DDS10_FORMAT_ASTC_12X12_UNORM_SRGB:
			SET_COMPRESSED_TYPE_INFO(GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_UNSIGNED_BYTE, 16);
			img->blockSize_x_ = 12;
			img->blockSize_y_ = 12;
			break;

		case DDS10_FORMAT_R32G32B32A32_TYPELESS:
		case DDS10_FORMAT_R32G32B32_TYPELESS:
		case DDS10_FORMAT_R16G16B16A16_TYPELESS:
		case DDS10_FORMAT_R32G32_TYPELESS:
		case DDS10_FORMAT_R10G10B10A2_TYPELESS:
		case DDS10_FORMAT_R8G8B8A8_TYPELESS:
		case DDS10_FORMAT_R16G16_TYPELESS:
		case DDS10_FORMAT_R32_TYPELESS:
		case DDS10_FORMAT_R8G8_TYPELESS:
		case DDS10_FORMAT_R16_TYPELESS:
		case DDS10_FORMAT_R8_TYPELESS:
		case DDS10_FORMAT_BC1_TYPELESS:
		case DDS10_FORMAT_BC3_TYPELESS:
		case DDS10_FORMAT_BC4_TYPELESS:
		case DDS10_FORMAT_BC2_TYPELESS:
		case DDS10_FORMAT_BC5_TYPELESS:
		case DDS10_FORMAT_B8G8R8A8_TYPELESS:
		case DDS10_FORMAT_B8G8R8X8_TYPELESS:
		case DDS10_FORMAT_ASTC_5X4_TYPELESS:
		case DDS10_FORMAT_ASTC_5X5_TYPELESS:
		case DDS10_FORMAT_ASTC_6X5_TYPELESS:
		case DDS10_FORMAT_ASTC_6X6_TYPELESS:
		case DDS10_FORMAT_ASTC_8X5_TYPELESS:
		case DDS10_FORMAT_ASTC_8X6_TYPELESS:
		case DDS10_FORMAT_ASTC_8X8_TYPELESS:
		case DDS10_FORMAT_ASTC_10X5_TYPELESS:
		case DDS10_FORMAT_ASTC_10X6_TYPELESS:
		case DDS10_FORMAT_ASTC_10X8_TYPELESS:
		case DDS10_FORMAT_ASTC_10X10_TYPELESS:
		case DDS10_FORMAT_ASTC_12X10_TYPELESS:
		case DDS10_FORMAT_ASTC_12X12_TYPELESS:
			//unclear what to do with typeless formats, leave them as unsupported for now
			// in the future it might make sense to use a default type, if these are common
			return false;

		case DDS10_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DDS10_FORMAT_R1_UNORM:
		case DDS10_FORMAT_BC6H_TYPELESS:
		case DDS10_FORMAT_BC6H_UF16:
		case DDS10_FORMAT_BC6H_SF16:
		case DDS10_FORMAT_BC7_TYPELESS:
		case DDS10_FORMAT_BC7_UNORM:
		case DDS10_FORMAT_BC7_UNORM_SRGB:
			//these formats are unsupported presently
			return false;

		case DDS10_FORMAT_FORCE_UINT:
		case DDS10_FORMAT_UNKNOWN:
		default:
			//these are errors
			return false;
		};

		img->setfaces(header.arraySize);
		img->setCubeFlag((header.miscFlag & 0x4) != 0);


		return true;
	}


	base::Image* ImageFile::loadDDS(StdInputStream * stream) {

		char filecode[4];

		//fread(filecode, 1, 4, fp);
		stream->serializeData(filecode, 4);

		if (strncmp(filecode, "DDS ", 4) != 0)
		{
			return nullptr;
		}

		base::DDSImage* img = new base::DDSImage();

		// read in DDS header
		DDS_HEADER ddsh;
		DDS_HEADER_10 ddsh10;

		//fread(&ddsh, sizeof(DDS_HEADER), 1, fp);
		stream->serializeData(&ddsh, sizeof(DDS_HEADER));

		// check if image is a volume texture
		if ((ddsh.dwCaps2 & DDSF_VOLUME) && (ddsh.dwDepth > 0))
			img->setdepth(ddsh.dwDepth);
		else
			img->setdepth(0);

		if ((ddsh.ddspf.dwFlags & DDSF_FOURCC) && (ddsh.ddspf.dwFourCC == FOURCC_DX10)) {
			stream->serializeData(&ddsh10, sizeof(DDS_HEADER_10));
		}

		// There are flags that are supposed to mark these fields as valid, but some dds files don't set them properly
		img->setwidth(ddsh.dwWidth);
		img->setheight(ddsh.dwHeight);

		if (ddsh.dwFlags & DDSF_MIPMAPCOUNT) {
			img->setnumOfMiplevels(ddsh.dwMipMapCount);
		}
		else
			img->setnumOfMiplevels(1);

		//check cube-map faces, the DX10 parser will override this
		if (ddsh.dwCaps2 & DDSF_CUBEMAP && !(ddsh.ddspf.dwFlags & DDSF_FOURCC && ddsh.ddspf.dwFourCC == FOURCC_DX10)) {
			//this is a cubemap, count the faces
			img->rfaces() = 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) ? 1 : 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) ? 1 : 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) ? 1 : 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) ? 1 : 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) ? 1 : 0;
			img->rfaces() += (ddsh.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) ? 1 : 0;

			//check for a complete cubemap
			if ((img->faces() != 6) || (img->width() != img->height()))
			{
				delete img;

				return nullptr;
			}
			img->setCubeFlag(true);
		}
		else
		{
			img->rfaces() = 1;
		}

		bool btcCompressed = false;
		int32 bytesPerElement = 0;

		// figure out what the image format is
		if (ddsh.ddspf.dwFlags & DDSF_FOURCC)
		{
			switch (ddsh.ddspf.dwFourCC)
			{
			case FOURCC_DXT1:
				img->setformat(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
				img->setinternalformat(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
				img->settype(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
				bytesPerElement = 8;
				btcCompressed = true;
				break;

			case FOURCC_DXT2:
			case FOURCC_DXT3:
				img->setformat(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
				img->setinternalformat(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
				img->settype(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
				bytesPerElement = 16;
				btcCompressed = true;
				break;

			case FOURCC_DXT4:
			case FOURCC_DXT5:
				img->setformat(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
				img->setinternalformat(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
				img->settype(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
				bytesPerElement = 16;
				btcCompressed = true;
				break;

			case FOURCC_ATI1:
				img->setformat(GL_COMPRESSED_RED_RGTC1);
				img->setinternalformat(GL_COMPRESSED_RED_RGTC1);
				img->settype(GL_COMPRESSED_RED_RGTC1);
				bytesPerElement = 8;
				btcCompressed = true;
				break;

			case FOURCC_BC4U:
				img->setformat(GL_COMPRESSED_RED_RGTC1);
				img->setinternalformat(GL_COMPRESSED_RED_RGTC1);
				img->settype(GL_COMPRESSED_RED_RGTC1);
				bytesPerElement = 8;
				btcCompressed = true;
				break;

			case FOURCC_BC4S:
				img->setformat(GL_COMPRESSED_SIGNED_RED_RGTC1);
				img->setinternalformat(GL_COMPRESSED_SIGNED_RED_RGTC1);
				img->settype(GL_COMPRESSED_SIGNED_RED_RGTC1);
				bytesPerElement = 8;
				btcCompressed = true;
				break;

			case FOURCC_ATI2:
				img->setformat(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2;
				img->setinternalformat(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2;
				img->settype(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2;
				bytesPerElement = 16;
				btcCompressed = true;
				break;

			case FOURCC_BC5S:
				img->setformat(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2; 
				img->setinternalformat(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2;
				img->settype(GL_COMPRESSED_RG_RGTC2); //GL_COMPRESSED_LUMINANCE_ALPHA_LATC2;
				bytesPerElement = 16;
				btcCompressed = true;
				break;

			case FOURCC_R8G8B8:
				img->setformat(GL_BGR);
				img->setinternalformat(GL_RGB8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 3;
				break;

			case FOURCC_A8R8G8B8:
				img->setformat(GL_BGRA);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 4;
				break;

			case FOURCC_X8R8G8B8:
				img->setformat(GL_BGRA);
				img->setinternalformat(GL_RGB8);
				img->settype(GL_UNSIGNED_INT_8_8_8_8);
				bytesPerElement = 4;
				break;

			case FOURCC_R5G6B5:
				img->setformat(GL_BGR);
				img->setinternalformat(GL_RGB5);
				img->settype(GL_UNSIGNED_SHORT_5_6_5);
				bytesPerElement = 2;
				break;

			case FOURCC_A8:
				img->setformat(GL_ALPHA);
				img->setinternalformat(GL_ALPHA8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 1;
				break;

			case FOURCC_A2B10G10R10:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGB10_A2);
				img->settype(GL_UNSIGNED_INT_10_10_10_2);
				bytesPerElement = 4;
				break;

			case FOURCC_A8B8G8R8:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 4;
				break;

			case FOURCC_X8B8G8R8:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGB8);
				img->settype(GL_UNSIGNED_INT_8_8_8_8);
				bytesPerElement = 4;
				break;

			case FOURCC_A2R10G10B10:
				img->setformat(GL_BGRA);
				img->setinternalformat(GL_RGB10_A2);
				img->settype(GL_UNSIGNED_INT_10_10_10_2);
				bytesPerElement = 4;
				break;

			case FOURCC_G16R16:
				img->setformat(GL_RG);
				img->setinternalformat(GL_RG16);
				img->settype(GL_UNSIGNED_SHORT);
				bytesPerElement = 4;
				break;

			case FOURCC_A16B16G16R16:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA16);
				img->settype(GL_UNSIGNED_SHORT);
				bytesPerElement = 8;
				break;

			case FOURCC_L8:
				img->setformat(GL_LUMINANCE);
				img->setinternalformat(GL_LUMINANCE8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 1;
				break;

			case FOURCC_A8L8:
				img->setformat(GL_LUMINANCE_ALPHA);
				img->setinternalformat(GL_LUMINANCE8_ALPHA8);
				img->settype(GL_UNSIGNED_BYTE);
				bytesPerElement = 2;
				break;

			case FOURCC_L16:
				img->setformat(GL_LUMINANCE);
				img->setinternalformat(GL_LUMINANCE16);
				img->settype(GL_UNSIGNED_SHORT);
				bytesPerElement = 2;
				break;

			case FOURCC_Q16W16V16U16:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA16_SNORM);
				img->settype(GL_SHORT);
				bytesPerElement = 8;
				break;

			case FOURCC_R16F:
				img->setformat(GL_RED);
				img->setinternalformat(GL_R16F);
				img->settype(GL_HALF_FLOAT);
				bytesPerElement = 2;
				break;

			case FOURCC_G16R16F:
				img->setformat(GL_RG);
				img->setinternalformat(GL_RG16F);
				img->settype(GL_HALF_FLOAT);
				bytesPerElement = 4;
				break;

			case FOURCC_A16B16G16R16F:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA16F);
				img->settype(GL_HALF_FLOAT);
				bytesPerElement = 8;
				break;

			case FOURCC_R32F:
				img->setformat(GL_RED);
				img->setinternalformat(GL_R32F);
				img->settype(GL_FLOAT);
				bytesPerElement = 4;
				break;

			case FOURCC_G32R32F:
				img->setformat(GL_RG);
				img->setinternalformat(GL_RG32F);
				img->settype(GL_FLOAT);
				bytesPerElement = 8;
				break;

			case FOURCC_A32B32G32R32F:
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA32F);
				img->settype(GL_FLOAT);
				bytesPerElement = 16;
				break;

			case FOURCC_DX10:
				if (!TranslateDX10Format(&ddsh10, img, bytesPerElement, btcCompressed)) {
					return false; //translation from DX10 failed
				}
				break;

			case FOURCC_UNKNOWN:
			case FOURCC_X1R5G5B5:
			case FOURCC_A1R5G5B5:
			case FOURCC_A4R4G4B4:
			case FOURCC_R3G3B2:
			case FOURCC_A8R3G3B2:
			case FOURCC_X4R4G4B4:
			case FOURCC_A4L4:
			case FOURCC_D16_LOCKABLE:
			case FOURCC_D32:
			case FOURCC_D24X8:
			case FOURCC_D16:
			case FOURCC_D32F_LOCKABLE:
			default:
				return false;
			}
		}
		else if (ddsh.ddspf.dwFlags == DDSF_RGBA && ddsh.ddspf.dwRGBBitCount == 32)
		{
			if (ddsh.ddspf.dwRBitMask == 0xff && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff0000 && ddsh.ddspf.dwABitMask == 0xff000000) {
				//RGBA8 order
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_BYTE);
			}
			else if (ddsh.ddspf.dwRBitMask == 0xff0000 && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff && ddsh.ddspf.dwABitMask == 0xff000000) {
				//BGRA8 order
				img->setformat(GL_BGRA);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_BYTE);
			}
			else if (ddsh.ddspf.dwRBitMask == 0x3ff00000 && ddsh.ddspf.dwGBitMask == 0xffc00 && ddsh.ddspf.dwBBitMask == 0x3ff && ddsh.ddspf.dwABitMask == 0xc0000000) {
				//BGR10_A2 order
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGB10_A2);
				img->settype(GL_UNSIGNED_INT_2_10_10_10_REV); //GL_UNSIGNED_INT_10_10_10_2;
			}
			else if (ddsh.ddspf.dwRBitMask == 0x3ff && ddsh.ddspf.dwGBitMask == 0xffc00 && ddsh.ddspf.dwBBitMask == 0x3ff00000 && ddsh.ddspf.dwABitMask == 0xc0000000) {
				//RGB10_A2 order
				img->setformat(GL_RGBA);
				img->setinternalformat(GL_RGB10_A2);
				img->settype(GL_UNSIGNED_INT_10_10_10_2);
			}
			else {
				//we'll just guess BGRA8, because that is the common legacy format for improperly labeled files
				img->setformat(GL_BGRA);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_BYTE);
			}
			bytesPerElement = 4;
		}
		else if (ddsh.ddspf.dwFlags == DDSF_RGB  && ddsh.ddspf.dwRGBBitCount == 32)
		{
			if (ddsh.ddspf.dwRBitMask == 0xffff && ddsh.ddspf.dwGBitMask == 0xffff0000 && ddsh.ddspf.dwBBitMask == 0x00 && ddsh.ddspf.dwABitMask == 0x00) {
				img->setformat(GL_RG);
				img->setinternalformat(GL_RG16);
				img->settype(GL_UNSIGNED_SHORT);
			}
			else if (ddsh.ddspf.dwRBitMask == 0xff && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff0000 && ddsh.ddspf.dwABitMask == 0x00) {
				img->setformat(GL_RGB);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_INT_8_8_8_8);
			}
			else if (ddsh.ddspf.dwRBitMask == 0xff0000 && ddsh.ddspf.dwGBitMask == 0xff00 && ddsh.ddspf.dwBBitMask == 0xff && ddsh.ddspf.dwABitMask == 0x00) {
				img->setformat(GL_BGR);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_INT_8_8_8_8);
			}
			else {
				// probably a poorly labeled file with BGRX semantics
				img->setformat(GL_BGR);
				img->setinternalformat(GL_RGBA8);
				img->settype(GL_UNSIGNED_INT_8_8_8_8);
			}
			bytesPerElement = 4;
		}
		else if (ddsh.ddspf.dwFlags == DDSF_RGB  && ddsh.ddspf.dwRGBBitCount == 24)
		{
			img->setformat(GL_BGR);
			img->setinternalformat(GL_RGB8);
			img->settype(GL_UNSIGNED_BYTE);
			bytesPerElement = 3;
		}
		// these cases revived from NVHHDDS...
		else if ((ddsh.ddspf.dwRGBBitCount == 16) &&
			(ddsh.ddspf.dwRBitMask == 0x0000F800) &&
			(ddsh.ddspf.dwGBitMask == 0x000007E0) &&
			(ddsh.ddspf.dwBBitMask == 0x0000001F) &&
			(ddsh.ddspf.dwABitMask == 0x00000000))
		{
			// We support D3D's R5G6B5, which is actually RGB in linear
			// memory.  It is equivalent to GL's GL_UNSIGNED_SHORT_5_6_5
			img->setformat(GL_BGR);
			img->setinternalformat(GL_RGB5);
			img->settype(GL_UNSIGNED_SHORT_5_6_5);
			bytesPerElement = 2;
		}
		else if ((ddsh.ddspf.dwRGBBitCount == 8) &&
			(ddsh.ddspf.dwRBitMask == 0x00000000) &&
			(ddsh.ddspf.dwGBitMask == 0x00000000) &&
			(ddsh.ddspf.dwBBitMask == 0x00000000) &&
			(ddsh.ddspf.dwABitMask == 0x000000FF))
		{
			// We support D3D's A8
			img->setformat(GL_ALPHA);
			img->setinternalformat(GL_ALPHA8);
			img->settype(GL_UNSIGNED_BYTE);
			bytesPerElement = 1;
		}
		else if ((ddsh.ddspf.dwRGBBitCount == 8) &&
			(ddsh.ddspf.dwRBitMask == 0x000000FF) &&
			(ddsh.ddspf.dwGBitMask == 0x00000000) &&
			(ddsh.ddspf.dwBBitMask == 0x00000000) &&
			(ddsh.ddspf.dwABitMask == 0x00000000))
		{
			// We support D3D's L8 (flagged as 8 bits of red only)
			img->setformat(GL_LUMINANCE);
			img->setinternalformat(GL_LUMINANCE8);
			img->settype(GL_UNSIGNED_BYTE);
			bytesPerElement = 1;
		}
		else if ((ddsh.ddspf.dwRGBBitCount == 16) &&
			(((ddsh.ddspf.dwRBitMask == 0x000000FF) &&
			(ddsh.ddspf.dwGBitMask == 0x00000000) &&
				(ddsh.ddspf.dwBBitMask == 0x00000000) &&
				(ddsh.ddspf.dwABitMask == 0x0000FF00)) ||
				((ddsh.ddspf.dwRBitMask == 0x000000FF) && // GIMP header for L8A8
				(ddsh.ddspf.dwGBitMask == 0x000000FF) &&  // Ugh
					(ddsh.ddspf.dwBBitMask == 0x000000FF) &&
					(ddsh.ddspf.dwABitMask == 0x0000FF00)))
			)
		{
			// We support D3D's A8L8 (flagged as 8 bits of red and 8 bits of alpha)
			img->setformat(GL_LUMINANCE_ALPHA);
			img->setinternalformat(GL_LUMINANCE8_ALPHA8);
			img->settype(GL_UNSIGNED_BYTE);
			bytesPerElement = 2;
		}
		// else fall back to L8 generic handling if capable.
		else if (ddsh.ddspf.dwRGBBitCount == 8)
		{
			img->setformat(GL_LUMINANCE);
			img->setinternalformat(GL_LUMINANCE8);
			img->settype(GL_UNSIGNED_BYTE);
			bytesPerElement = 1;
		}
		// else, we can't decode this file... :-(
		else
		{
			return false;
		}

		/// Flip not allowed for ASTC
		if (vertFlip &&
			((img->internalformat() >= 0x93B0 && img->internalformat() <= 0x93BD)
				|| (img->internalformat() >= 0x93D0 && img->internalformat() <= 0x93DD)))
		{
			delete img;

			return nullptr;
		}

		img->setElementSize(bytesPerElement);

		bool mustExpandDXT = auto_expandDXT &&
			((img->format() == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
			(img->format() == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
				(img->format() == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT));

		img->levelDataPtr_ = new uint8*[img->faces() * img->numOfMiplevels()];

		int32 totalSize = 0;
		{
			for (int32 face = 0; face < img->faces(); face++) {
				int32 w = img->width(), h = img->height(), d = (img->depth()) ? img->depth() : 1;
				for (int32 level = 0; level < img->numOfMiplevels(); level++) {
					int32 bw = (btcCompressed) ? (w - 1) / img->blockSize_x_ + 1 : w;
					int32 bh = (btcCompressed) ? (h - 1) / img->blockSize_y_ + 1 : h;
					int32 readSize = bw*bh*d*bytesPerElement;
					int32 allocSize = mustExpandDXT ? (w * h * 4) : readSize;

					totalSize += allocSize;

					//reduce mip sizes
					if (!btcCompressed) {
						w = (w > 1) ? w >> 1 : 1;
						h = (h > 1) ? h >> 1 : 1;
						d = (d > 1) ? d >> 1 : 1;
					}
				}
			}
		}

		img->allocate(totalSize);
		img->blockSize_ = totalSize;

		uint8* ptr = (uint8*)img->pixels();
		int currentLevel = 0;

		// Use the original format when performing component swaps, but store any new
		// format in the image once the loop over all faces/levels is complete
		uint originalFormat = img->format();
		uint finalFormat = img->format();

		for (uint face = 0; face < img->faces(); face++) {
			uint w = img->width(), h = img->height(), d = (img->depth()) ? img->depth() : 1;
			for (uint level = 0; level < img->numOfMiplevels(); level++) {
				uint bw = (btcCompressed) ? (w - 1) / img->blockSize_x_ + 1 : w;
				uint bh = (btcCompressed) ? (h - 1) / img->blockSize_y_ + 1 : h;
				uint readSize = bw*bh*d*bytesPerElement;
				uint allocSize = mustExpandDXT ? (w * h * 4) : readSize;

				uint8 *pixels = ptr;
				img->levelDataPtr_[currentLevel++] = ptr;
				ptr += allocSize;

				stream->serializeData(pixels, readSize);

				if (vertFlip && !img->cubMap())
					img->flipSurface(pixels, w, h, d);

				if (!supportsBGR)
					finalFormat = img->componentSwapSurface(pixels, w, h, d);

				// do we need to expand DXT?
				if (mustExpandDXT)
					img->expandDXT(pixels, w, h, d, readSize);

				//reduce mip sizes
				w = (w > 1) ? w >> 1 : 1;
				h = (h > 1) ? h >> 1 : 1;
				d = (d > 1) ? d >> 1 : 1;
			}
		}

		if (mustExpandDXT) {
			img->setformat(GL_RGBA);
			img->settype(GL_UNSIGNED_BYTE);
			img->setinternalformat(GL_RGBA8);
		}
		else
		{
			img->setformat(finalFormat);
		}

		return img;
	}
}

namespace base
{
	void Image::flip_blocks_dxtc1(uint8 *ptr, uint numBlocks)
	{
		IO::DXTColBlock *curblock = (IO::DXTColBlock*)ptr;
		uint8 temp;

		for (uint i = 0; i < numBlocks; i++) {
			temp = curblock->row[0];
			curblock->row[0] = curblock->row[3];
			curblock->row[3] = temp;
			temp = curblock->row[1];
			curblock->row[1] = curblock->row[2];
			curblock->row[2] = temp;

			curblock++;
		}
	}

	//
	// flip a DXT3 color block
	////////////////////////////////////////////////////////////
	void Image::flip_blocks_dxtc3(uint8 *ptr, uint numBlocks)
	{
		IO::DXTColBlock *curblock = (IO::DXTColBlock*)ptr;
		IO::DXT3AlphaBlock *alphablock;
		uint16 tempS;
		uint8 tempB;

		for (uint i = 0; i < numBlocks; i++)
		{
			alphablock = (IO::DXT3AlphaBlock*)curblock;

			tempS = alphablock->row[0];
			alphablock->row[0] = alphablock->row[3];
			alphablock->row[3] = tempS;
			tempS = alphablock->row[1];
			alphablock->row[1] = alphablock->row[2];
			alphablock->row[2] = tempS;

			curblock++;

			tempB = curblock->row[0];
			curblock->row[0] = curblock->row[3];
			curblock->row[3] = tempB;
			tempB = curblock->row[1];
			curblock->row[1] = curblock->row[2];
			curblock->row[2] = tempB;

			curblock++;
		}
	}

	//
	// flip a DXT5 alpha block
	////////////////////////////////////////////////////////////
	void flip_dxt5_alpha(IO::DXT5AlphaBlock *block)
	{
		uint8 gBits[4][4];

		const uint mask = 0x00000007;          // bits = 00 00 01 11
		uint bits = 0;
		memcpy(&bits, &block->row[0], sizeof(uint8) * 3);

		gBits[0][0] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[0][1] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[0][2] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[0][3] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[1][0] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[1][1] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[1][2] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[1][3] = (uint8)(bits & mask);

		bits = 0;
		memcpy(&bits, &block->row[3], sizeof(uint8) * 3);

		gBits[2][0] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[2][1] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[2][2] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[2][3] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[3][0] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[3][1] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[3][2] = (uint8)(bits & mask);
		bits >>= 3;
		gBits[3][3] = (uint8)(bits & mask);

		// clear existing alpha bits
		memset(block->row, 0, sizeof(uint8) * 6);

		uint *pBits = ((uint*) &(block->row[0]));

		*pBits = *pBits | (gBits[3][0] << 0);
		*pBits = *pBits | (gBits[3][1] << 3);
		*pBits = *pBits | (gBits[3][2] << 6);
		*pBits = *pBits | (gBits[3][3] << 9);

		*pBits = *pBits | (gBits[2][0] << 12);
		*pBits = *pBits | (gBits[2][1] << 15);
		*pBits = *pBits | (gBits[2][2] << 18);
		*pBits = *pBits | (gBits[2][3] << 21);

		pBits = ((uint*) &(block->row[3]));

		*pBits = *pBits | (gBits[1][0] << 0);
		*pBits = *pBits | (gBits[1][1] << 3);
		*pBits = *pBits | (gBits[1][2] << 6);
		*pBits = *pBits | (gBits[1][3] << 9);

		*pBits = *pBits | (gBits[0][0] << 12);
		*pBits = *pBits | (gBits[0][1] << 15);
		*pBits = *pBits | (gBits[0][2] << 18);
		*pBits = *pBits | (gBits[0][3] << 21);
	}

	//
	// flip a DXT5 color block
	////////////////////////////////////////////////////////////
	void Image::flip_blocks_dxtc5(uint8 *ptr, uint numBlocks)
	{
		IO::DXTColBlock *curblock = (IO::DXTColBlock*)ptr;
		IO::DXT5AlphaBlock *alphablock;
		uint8 temp;

		for (uint i = 0; i < numBlocks; i++)
		{
			alphablock = (IO::DXT5AlphaBlock*)curblock;

			flip_dxt5_alpha(alphablock);

			curblock++;

			temp = curblock->row[0];
			curblock->row[0] = curblock->row[3];
			curblock->row[3] = temp;
			temp = curblock->row[1];
			curblock->row[1] = curblock->row[2];
			curblock->row[2] = temp;

			curblock++;
		}
	}

	//
	// flip a BC4 block
	////////////////////////////////////////////////////////////
	void Image::flip_blocks_bc4(uint8 *ptr, uint numBlocks)
	{
		IO::DXT5AlphaBlock *alphablock = (IO::DXT5AlphaBlock*)ptr;

		for (uint i = 0; i < numBlocks; i++)
		{
			flip_dxt5_alpha(alphablock);

			alphablock++;
		}
	}

	//
	// flip a BC5 block
	////////////////////////////////////////////////////////////
	void Image::flip_blocks_bc5(uint8 *ptr, uint numBlocks)
	{
		IO::DXT5AlphaBlock *alphablock = (IO::DXT5AlphaBlock*)ptr;

		for (uint i = 0; i < numBlocks; i++)
		{
			flip_dxt5_alpha(alphablock);

			alphablock++;
			flip_dxt5_alpha(alphablock);

			alphablock++;
		}
	}
}
