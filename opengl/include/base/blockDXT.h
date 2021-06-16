//----------------------------------------------------------------------------------
// File:        NvImage/BlockDXT.h
// SDK Version: v3.00 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014-2015, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#ifndef NV_IMAGE_BLOCKDXT_H
#define NV_IMAGE_BLOCKDXT_H

#include "colorBlock.h"

namespace base
{
	/// DXT1 block.
	struct BlockDXT1
	{
		Color16 col0;
		Color16 col1;
		union {
			uint8 row[4];
			uint indices;
		};

		bool isFourColorMode() const;

		uint evaluatePalette(Color32 color_array[4]) const;
		uint evaluatePaletteFast(Color32 color_array[4]) const;
		void evaluatePalette3(Color32 color_array[4]) const;
		void evaluatePalette4(Color32 color_array[4]) const;

		void decodeBlock(ColorBlock * block) const;

		void setIndices(int32_t * idx);
	};

	/// Return true if the block uses four color mode, false otherwise.
	inline bool BlockDXT1::isFourColorMode() const
	{
		return col0.u > col1.u;
	}


	/// DXT3 alpha block with explicit alpha.
	struct AlphaBlockDXT3
	{
		union {
			struct {
				uint alpha0 : 4;
				uint alpha1 : 4;
				uint alpha2 : 4;
				uint alpha3 : 4;
				uint alpha4 : 4;
				uint alpha5 : 4;
				uint alpha6 : 4;
				uint alpha7 : 4;
				uint alpha8 : 4;
				uint alpha9 : 4;
				uint alphaA : 4;
				uint alphaB : 4;
				uint alphaC : 4;
				uint alphaD : 4;
				uint alphaE : 4;
				uint alphaF : 4;
			};
			uint16_t row[4];
		};

		void decodeBlock(ColorBlock * block) const;

		void flip4();
		void flip2();
	};


	/// DXT3 block.
	struct BlockDXT3
	{
		AlphaBlockDXT3 alpha;
		BlockDXT1 color;

		void decodeBlock(ColorBlock * block) const;
	};


	/// DXT5 alpha block.
	struct AlphaBlockDXT5
	{
		union {
			struct {
				uint64 alpha0 : 8;    // 8
				uint64 alpha1 : 8;    // 16
				uint64 bits0 : 3;    // 3 - 19
				uint64 bits1 : 3;     // 6 - 22
				uint64 bits2 : 3;     // 9 - 25
				uint64 bits3 : 3;    // 12 - 28
				uint64 bits4 : 3;    // 15 - 31
				uint64 bits5 : 3;    // 18 - 34
				uint64 bits6 : 3;    // 21 - 37
				uint64 bits7 : 3;    // 24 - 40
				uint64 bits8 : 3;    // 27 - 43
				uint64 bits9 : 3;     // 30 - 46
				uint64 bitsA : 3;     // 33 - 49
				uint64 bitsB : 3;    // 36 - 52
				uint64 bitsC : 3;    // 39 - 55
				uint64 bitsD : 3;    // 42 - 58
				uint64 bitsE : 3;    // 45 - 61
				uint64 bitsF : 3;    // 48 - 64
			};
			uint64 u;
		};

		void evaluatePalette(uint8 alpha[8]) const;
		void evaluatePalette8(uint8 alpha[8]) const;
		void evaluatePalette6(uint8 alpha[8]) const;
		void indices(uint8 index_array[16]) const;

		uint index(uint index) const;
		void setIndex(uint index, uint value);

		void decodeBlock(ColorBlock * block) const;
	};


	/// DXT5 block.
	struct BlockDXT5
	{
		AlphaBlockDXT5 alpha;
		BlockDXT1 color;

		void decodeBlock(ColorBlock * block) const;
	};

} // nv namespace

#endif // NV_IMAGE_BLOCKDXT_H
