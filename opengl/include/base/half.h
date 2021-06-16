//
//  half.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//

#ifndef half_h
#define half_h
#include "type.h"
#include "sys.h"

namespace math {

	typedef union floatint_union
	{
		float  f;
		uint32 i;
	} floatint_union;

	inline LIBENIGHT_EXPORT Half convertFloatToHalf(const float Value)
	{
		MEMORY_RW_REORDERING_BARRIER;
		unsigned int Result;

		unsigned int IValue = ((unsigned int*)(&Value))[0];
		unsigned int Sign = (IValue & 0x80000000U) >> 16U;
		IValue = IValue & 0x7FFFFFFFU;      // Hack off the sign

		if (IValue > 0x47FFEFFFU)
		{
			// The number is too large to be represented as a half.  Saturate to infinity.
			Result = 0x7FFFU;
		}
		else
		{
			if (IValue < 0x38800000U)
			{
				// The number is too small to be represented as a normalized half.
				// Convert it to a denormalized value.
				unsigned int Shift = 113U - (IValue >> 23U);
#if defined(CRY_UBSAN)
				// UBSAN (technically correctly) detects 113 bits of shift when converting 0.0f, which is undefined behavior.
				// The intended behavior (and probably actual behavior on all CPU) is for those shifts to result in 0.
				if (Shift >= 32)
				{
					IValue = 0;
				}
				else
#endif
					IValue = (0x800000U | (IValue & 0x7FFFFFU)) >> Shift;
			}
			else
			{
				// Rebias the exponent to represent the value as a normalized half.
				IValue += 0xC8000000U;
			}

			Result = ((IValue + 0x0FFFU + ((IValue >> 13U) & 1U)) >> 13U) & 0x7FFFU;
		}
		return (Half)(Result | Sign);
	}

	inline float convertHalfToFloat(const Half Value)
	{
		MEMORY_RW_REORDERING_BARRIER;
		unsigned int Mantissa;
		unsigned int Exponent;
		unsigned int Result;

		Mantissa = (unsigned int)(Value & 0x03FF);

		if ((Value & 0x7C00) != 0)  // The value is normalized
		{
			Exponent = (unsigned int)((Value >> 10) & 0x1F);
		}
		else if (Mantissa != 0)     // The value is denormalized
		{
			// Normalize the value in the resulting float
			Exponent = 1;

			do
			{
				Exponent--;
				Mantissa <<= 1;
			} while ((Mantissa & 0x0400) == 0);

			Mantissa &= 0x03FF;
		}
		else                        // The value is zero
		{
			Exponent = (unsigned int)-112;
		}

		Result = ((Value & 0x8000) << 16) | // Sign
			((Exponent + 112) << 23) | // Exponent
			(Mantissa << 13);          // Mantissa

		return *(float*)&Result;
	}

	struct LIBENIGHT_EXPORT Half2
	{
		Half x;
		Half y;

		Half2()
		{
		}
		Half2(Half _x, Half _y)
			: x(_x)
			, y(_y)
		{
		}
		Half2(const Half* const __restrict pArray)
		{
			x = pArray[0];
			y = pArray[1];
		}
		Half2(float _x, float _y)
		{
			x = convertFloatToHalf(_x);
			y = convertFloatToHalf(_y);
		}
		Half2(const float* const __restrict pArray)
		{
			x = convertFloatToHalf(pArray[0]);
			y = convertFloatToHalf(pArray[1]);
		}
		Half2& operator=(const Half2& Half2)
		{
			x = Half2.x;
			y = Half2.y;
			return *this;
		}

		bool operator!=(const Half2& rhs) const
		{
			return x != rhs.x || y != rhs.y;
		}

	};

	struct LIBENIGHT_EXPORT Half4
	{
		Half x;
		Half y;
		Half z;
		Half w;

		Half4()
		{
		}
		Half4(Half _x, Half _y, Half _z, Half _w)
			: x(_x)
			, y(_y)
			, z(_z)
			, w(_w)
		{
		}
		Half4(const Half* const __restrict pArray)
		{
			x = pArray[0];
			y = pArray[1];
			z = pArray[2];
			w = pArray[3];
		}
		Half4(float _x, float _y, float _z, float _w)
		{
			x = convertFloatToHalf(_x);
			y = convertFloatToHalf(_y);
			z = convertFloatToHalf(_z);
			w = convertFloatToHalf(_w);
		}
		Half4(const float* const __restrict pArray)
		{
			x = convertFloatToHalf(pArray[0]);
			y = convertFloatToHalf(pArray[1]);
			z = convertFloatToHalf(pArray[2]);
			w = convertFloatToHalf(pArray[3]);
		}
		Half4& operator=(const Half4& Half4)
		{
			x = Half4.x;
			y = Half4.y;
			z = Half4.z;
			w = Half4.w;
			return *this;
		}
		bool operator!=(const Half4& rhs) const
		{
			return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
		}

	};

}
#endif
