// This code is in the public domain -- castanyo@yahoo.es

#pragma once
#ifndef __OpenGL__Color__
#define __OpenGL__Color__

#include "decl.h"
#include "sys.h"
#include "type.h"
#include <algorithm>

using namespace math;

namespace base
{

	/// 64 bit color stored as BGRA.
	class LIBENIGHT_EXPORT Color64
	{
	public:
		Color64() { }
		Color64(const Color64 & c) : u(c.u) { }
		Color64(uint16 R, uint16 G, uint16 B, uint16 A) { setRGBA(R, G, B, A); }
		explicit Color64(uint64 U) : u(U) { }

		void setRGBA(uint16 R, uint16 G, uint16 B, uint16 A)
		{
			r = R;
			g = G;
			b = B;
			a = A;
		}

		operator uint64 () const 
		{
			return u;
		}

		union {
			struct {
#if EN_LITTLE_ENDIAN
				uint16 r, a, b, g;
#else
				uint16 a : 16;
				uint16 r : 16;
				uint16 g : 16;
				uint16 b : 16;
#endif
			};
			uint64 u;
		};
	};

	/// 32 bit color stored as BGRA.
	class LIBENIGHT_EXPORT Color32
	{
	public:
		Color32() { }
		Color32(const Color32 & c) : u(c.u) { }
		Color32(uint8 R, uint8 G, uint8 B) { setRGBA(R, G, B, 0xFF); }
		Color32(uint8 R, uint8 G, uint8 B, uint8 A) { setRGBA(R, G, B, A); }
		//Color32(uint8 c[4]) { setRGBA(c[0], c[1], c[2], c[3]); }
		//Color32(float R, float G, float B) { setRGBA(uint(R*255), uint(G*255), uint(B*255), 0xFF); }
		//Color32(float R, float G, float B, float A) { setRGBA(uint(R*255), uint(G*255), uint(B*255), uint(A*255)); }
		explicit Color32(uint32 U) : u(U) { }

		void setRGBA(uint8 R, uint8 G, uint8 B, uint8 A)
		{
			r = R;
			g = G;
			b = B;
			a = A;
		}

		void setBGRA(uint8 B, uint8 G, uint8 R, uint8 A = 0xFF)
		{
			r = R;
			g = G;
			b = B;
			a = A;
		}

		operator uint32 () const 
		{
			return u;
		}

		union {
			struct {
#if EN_LITTLE_ENDIAN
				uint8 b, g, r, a;
#else
				uint8 a : 8;
				uint8 r : 8;
				uint8 g : 8;
				uint8 b : 8;
#endif
			};
			uint8 component[4];
			uint32 u;
		};
	};

	/// 16 bit 565 BGR color.
	class LIBENIGHT_EXPORT Color16
	{
	public:
		Color16() { }
		Color16(const Color16 & c) : u(c.u) { }
		explicit Color16(uint16 U) : u(U) { }

		union {
			struct {
#if EN_LITTLE_ENDIAN
				uint16 b : 5;
				uint16 g : 6;
				uint16 r : 5;
#else
				uint16 r : 5;
				uint16 g : 6;
				uint16 b : 5;
#endif
			};
			uint16 u;
		};
	};

	/// 16 bit 4444 BGRA color.
	class LIBENIGHT_EXPORT Color16_4444
	{
	public:
		Color16_4444() { }
		Color16_4444(const Color16_4444 & c) : u(c.u) { }
		explicit Color16_4444(uint16 U) : u(U) { }

		union {
			struct {
#if EN_LITTLE_ENDIAN
				uint16 b : 4;
				uint16 g : 4;
				uint16 r : 4;
				uint16 a : 4;
#else
				uint16 a : 4;
				uint16 r : 4;
				uint16 g : 4;
				uint16 b : 4;
#endif
			};
			uint16 u;
		};
	};



	template<class T> struct Color_tpl;

	typedef Color_tpl<uint8> ColorB; //!< [0, 255]
	typedef Color_tpl<float> ColorF; //!< [0.0, 1.0]

									 //! RGBA Color structure.
	template<class T> struct Color_tpl
	{
		T r, g, b, a;

		inline Color_tpl() {};

		inline Color_tpl(T _x, T _y, T _z, T _w);
		inline Color_tpl(T _x, T _y, T _z);

		// works together with pack_abgr8888
		inline Color_tpl(const unsigned int abgr);
		inline Color_tpl(const math::f32 c);
		inline Color_tpl(const ColorF& c);
		inline Color_tpl(const ColorF& c, float fAlpha);
		inline Color_tpl(const V3f& c, float fAlpha);

		void Clamp(float Min = 0, float Max = 1.0f)
		{
			//r = ::FClamp(r, Min, Max);
			//g = ::FClamp(g, Min, Max);
			//b = ::FClamp(b, Min, Max);
			//a = ::FClamp(a, Min, Max);
		}
		void ScaleCol(float f)
		{
			r = static_cast<T>(r * f);
			g = static_cast<T>(g * f);
			b = static_cast<T>(b * f);
		}

		float Luminance() const
		{
			return r * 0.30f + g * 0.59f + b * 0.11f;
		}

		inline float Max() const
		{
			return max(r, max(b, g));
		}

		float NormalizeCol(ColorF& out) const
		{
			float max = Max();

			if (max == 0)
				return 0;

			out = *this / max;

			return max;
		}

		inline Color_tpl(const V3f& vVec)
		{
			r = (T)vVec.x;
			g = (T)vVec.y;
			b = (T)vVec.z;
			a = (T)1.f;
		}

		inline Color_tpl& operator=(const V3f& v) { r = (T)v.x; g = (T)v.y; b = (T)v.z; a = (T)1.0f; return *this; }
		inline Color_tpl& operator=(const Color_tpl& c) { r = (T)c.r; g = (T)c.g; b = (T)c.b; a = (T)c.a; return *this; }

		inline T&         operator[](int index) { (index >= 0 && index <= 3); return ((T*)this)[index]; }
		inline T          operator[](int index) const { (index >= 0 && index <= 3); return ((T*)this)[index]; }

		inline void       set(T _x, T _y, T _z, T _w)
		{
			r = _x;
			g = _y;
			b = _z;
			a = _w;
		}

		inline Color_tpl operator+() const
		{
			return *this;
		}
		inline Color_tpl operator-() const
		{
			return Color_tpl<T>(-r, -g, -b, -a);
		}

		inline Color_tpl& operator+=(const Color_tpl& v)
		{
			T _r = r, _g = g, _b = b, _a = a;
			_r += v.r;
			_g += v.g;
			_b += v.b;
			_a += v.a;
			r = _r;
			g = _g;
			b = _b;
			a = _a;
			return *this;
		}
		inline Color_tpl& operator-=(const Color_tpl& v)
		{
			r -= v.r;
			g -= v.g;
			b -= v.b;
			a -= v.a;
			return *this;
		}
		inline Color_tpl& operator*=(const Color_tpl& v)
		{
			r *= v.r;
			g *= v.g;
			b *= v.b;
			a *= v.a;
			return *this;
		}
		inline Color_tpl& operator/=(const Color_tpl& v)
		{
			r /= v.r;
			g /= v.g;
			b /= v.b;
			a /= v.a;
			return *this;
		}
		inline Color_tpl& operator*=(T s)
		{
			r *= s;
			g *= s;
			b *= s;
			a *= s;
			return *this;
		}
		inline Color_tpl& operator/=(T s)
		{
			s = 1.0f / s;
			r *= s;
			g *= s;
			b *= s;
			a *= s;
			return *this;
		}

		inline Color_tpl operator+(const Color_tpl& v) const
		{
			return Color_tpl(r + v.r, g + v.g, b + v.b, a + v.a);
		}
		inline Color_tpl operator-(const Color_tpl& v) const
		{
			return Color_tpl(r - v.r, g - v.g, b - v.b, a - v.a);
		}
		inline Color_tpl operator*(const Color_tpl& v) const
		{
			return Color_tpl(r * v.r, g * v.g, b * v.b, a * v.a);
		}
		inline Color_tpl operator/(const Color_tpl& v) const
		{
			return Color_tpl(r / v.r, g / v.g, b / v.b, a / v.a);
		}
		inline Color_tpl operator*(T s) const
		{
			return Color_tpl(r * s, g * s, b * s, a * s);
		}
		inline Color_tpl operator/(T s) const
		{
			s = 1.0f / s;
			return Color_tpl(r * s, g * s, b * s, a * s);
		}

		inline bool operator==(const Color_tpl& v) const
		{
			return (r == v.r) && (g == v.g) && (b == v.b) && (a == v.a);
		}
		inline bool operator!=(const Color_tpl& v) const
		{
			return (r != v.r) || (g != v.g) || (b != v.b) || (a != v.a);
		}

		inline unsigned char  pack_rgb332() const;
		inline unsigned short pack_argb4444() const;
		inline unsigned short pack_rgb555() const;
		inline unsigned short pack_rgb565() const;
		inline unsigned int   pack_bgr888() const;
		inline unsigned int   pack_rgb888() const;
		inline unsigned int   pack_abgr8888() const;
		inline unsigned int   pack_argb8888() const;
		inline V4f          toVec4() const { return V4f(r, g, b, a); }
		inline V3f          toVec3() const { return V3f(r, g, b); }

        inline void          toHSV(math::f32& h, math::f32& s, math::f32& v) const;
		inline void          fromHSV(f32 h, f32 s, f32 v);

		inline void          clamp(T bottom = 0.0f, T top = 1.0f);

		inline void          maximum(const Color_tpl<T>& ca, const Color_tpl<T>& cb);
		inline void          minimum(const Color_tpl<T>& ca, const Color_tpl<T>& cb);
		inline void          abs();

		inline void          adjust_contrast(T c);
		inline void          adjust_saturation(T s);
		inline void          adjust_luminance(float newLum);

		inline void          lerpFloat(const Color_tpl<T>& ca, const Color_tpl<T>& cb, float s);
		inline void          negative(const Color_tpl<T>& c);
		inline void          grey(const Color_tpl<T>& c);

		//! Helper function - maybe we can improve the integration.
		static inline uint32 ComputeAvgCol_Fast(const uint32 dwCol0, const uint32 dwCol1)
		{
			uint32 dwHalfCol0 = (dwCol0 / 2) & 0x7f7f7f7f;      // each component /2
			uint32 dwHalfCol1 = (dwCol1 / 2) & 0x7f7f7f7f;      // each component /2

			return dwHalfCol0 + dwHalfCol1;
		}

		//! mCIE: adjusted to compensate problems of DXT compression (extra bit in green channel causes green/purple artifacts).
		Color_tpl<T> RGB2mCIE() const
		{
			Color_tpl<T> in = *this;

			// to get grey chrominance for dark colors
			in.r += 0.000001f;
			in.g += 0.000001f;
			in.b += 0.000001f;

			float RGBSum = in.r + in.g + in.b;

			float fInv = 1.0f / RGBSum;

			float RNorm = 3 * 10.0f / 31.0f * in.r * fInv;
			float GNorm = 3 * 21.0f / 63.0f * in.g * fInv;
			float Scale = RGBSum / 3.0f;

			// test grey
			//	out.r = 10.0f/31.0f;		// 1/3 = 10/30      Red range     0..30, 31 unused
			//	out.g = 21.0f/63.0f;		// 1/3 = 21/63      Green range   0..63

            RNorm = std::max(0.0f, std::min(1.0f, RNorm));
			GNorm = std::max(0.0f, std::min(1.0f, GNorm));

			return Color_tpl<T>(RNorm, GNorm, Scale);
		}

		//! mCIE: adjusted to compensate problems of DXT compression (extra bit in green channel causes green/purple artefacts).
		Color_tpl<T> mCIE2RGB() const
		{
			Color_tpl<T> out = *this;

			float fScale = out.b;       //                  Blue range   0..31

										// test grey
										//	out.r = 10.0f/31.0f;		// 1/3 = 10/30      Red range     0..30, 31 unused
										//	out.g = 21.0f/63.0f;		// 1/3 = 21/63      Green range   0..63

			out.r *= 31.0f / 30.0f;
			out.g *= 63.0f / 63.0f;
			out.b = 0.999f - out.r - out.g;

			float s = 3.0f * fScale;

			out.r *= s;
			out.g *= s;
			out.b *= s;

			out.Clamp();

			return out;
		}

		void rgb2srgb()
		{
			for (int i = 0; i < 3; i++)
			{
				T& c = (*this)[i];

				if (c < 0.0031308f)
				{
					c = 12.92f * c;
				}
				else
				{
					c = 1.055f * pow(c, 1.0f / 2.4f) - 0.055f;
				}
			}
		}

		void srgb2rgb()
		{
			for (int i = 0; i < 3; i++)
			{
				T& c = (*this)[i];

				if (c <= 0.040448643f)
				{
					c = c / 12.92f;
				}
				else
				{
					c = pow((c + 0.055f) / 1.055f, 2.4f);
				}
			}
		}
	};

	template<>
	inline Color_tpl<f32>::Color_tpl(math::f32 _x, math::f32 _y, math::f32 _z, math::f32 _w)
	{
		r = _x;
		g = _y;
		b = _z;
		a = _w;
	}

	template<>
	inline Color_tpl<f32>::Color_tpl(math::f32 _x, math::f32 _y, math::f32 _z)
	{
		r = _x;
		g = _y;
		b = _z;
		a = 1.f;
	}

	template<>
	inline Color_tpl<uint8>::Color_tpl(uint8 _x, uint8 _y, uint8 _z, uint8 _w)
	{
		r = _x;
		g = _y;
		b = _z;
		a = _w;
	}

	template<>
	inline Color_tpl<uint8>::Color_tpl(uint8 _x, uint8 _y, uint8 _z)
	{
		r = _x;
		g = _y;
		b = _z;
		a = 255;
	}

	//-----------------------------------------------------------------------------

	template<>
	inline Color_tpl<f32>::Color_tpl(const unsigned int abgr)
	{
		r = (abgr & 0xff) / 255.0f;
		g = ((abgr >> 8) & 0xff) / 255.0f;
		b = ((abgr >> 16) & 0xff) / 255.0f;
		a = ((abgr >> 24) & 0xff) / 255.0f;
	}

	//! Use this with RGBA8 macro!
	template<>
	inline Color_tpl<uint8>::Color_tpl(const unsigned int c)
	{
		*(unsigned int*)(&r) = c;
	}

	//-----------------------------------------------------------------------------

	template<>
	inline Color_tpl<f32>::Color_tpl(const float c)
	{
		r = c;
		g = c;
		b = c;
		a = c;
	}
	template<>
	inline Color_tpl<uint8>::Color_tpl(const float c)
	{
		r = (uint8)(c * 255);
		g = (uint8)(c * 255);
		b = (uint8)(c * 255);
		a = (uint8)(c * 255);
	}
	//-----------------------------------------------------------------------------

	template<>
	inline Color_tpl<f32>::Color_tpl(const ColorF& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}
	template<>
	inline Color_tpl<uint8>::Color_tpl(const ColorF& c)
	{
		r = (uint8)(c.r * 255);
		g = (uint8)(c.g * 255);
		b = (uint8)(c.b * 255);
		a = (uint8)(c.a * 255);
	}

	template<>
	inline Color_tpl<f32>::Color_tpl(const ColorF& c, float fAlpha)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = fAlpha;
	}

	template<>
	inline Color_tpl<f32>::Color_tpl(const V3f& c, float fAlpha)
	{
		r = c.x;
		g = c.y;
		b = c.z;
		a = fAlpha;
	}

	template<>
	inline Color_tpl<uint8>::Color_tpl(const ColorF& c, float fAlpha)
	{
		r = (uint8)(c.r * 255);
		g = (uint8)(c.g * 255);
		b = (uint8)(c.b * 255);
		a = (uint8)(fAlpha * 255);
	}
	template<>
	inline Color_tpl<uint8>::Color_tpl(const V3f& c, float fAlpha)
	{
		r = (uint8)(c.x * 255);
		g = (uint8)(c.y * 255);
		b = (uint8)(c.z * 255);
		a = (uint8)(fAlpha * 255);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// functions implementation
	///////////////////////////////////////////////

	///////////////////////////////////////////////
	/*template <class T>
	inline Color_tpl<T>::Color_tpl(const T *p_elts)
	{
	r = p_elts[0]; g = p_elts[1]; b = p_elts[2]; a = p_elts[3];
	}*/

	///////////////////////////////////////////////
	///////////////////////////////////////////////
	template<class T>
	inline Color_tpl<T> operator*(T s, const Color_tpl<T>& v)
	{
		return Color_tpl<T>(v.r * s, v.g * s, v.b * s, v.a * s);
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned char Color_tpl<T >::pack_rgb332() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
		}
		return ((cr >> 5) << 5) | ((cg >> 5) << 2) | (cb >> 5);
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned short Color_tpl<T >::pack_argb4444() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		unsigned char ca;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
			ca = (unsigned char)a;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
			ca = (unsigned short)(a) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
			ca = (unsigned char)(a * 255.0f);
		}
		return ((ca >> 4) << 12) | ((cr >> 4) << 8) | ((cg >> 4) << 4) | (cb >> 4);
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned short Color_tpl<T >::pack_rgb555() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
		}
		return ((cr >> 3) << 10) | ((cg >> 3) << 5) | (cb >> 3);
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned short Color_tpl<T >::pack_rgb565() const
	{
		unsigned short cr;
		unsigned short cg;
		unsigned short cb;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned short)r;
			cg = (unsigned short)g;
			cb = (unsigned short)b;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
		}
		else // float or double
		{
			cr = (unsigned short)(r * 255.0f);
			cg = (unsigned short)(g * 255.0f);
			cb = (unsigned short)(b * 255.0f);
		}
		return ((cr >> 3) << 11) | ((cg >> 2) << 5) | (cb >> 3);
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned int Color_tpl<T >::pack_bgr888() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
		}
		return (cb << 16) | (cg << 8) | cr;
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned int Color_tpl<T >::pack_rgb888() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
		}
		return (cr << 16) | (cg << 8) | cb;
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned int Color_tpl<T >::pack_abgr8888() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		unsigned char ca;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
			ca = (unsigned char)a;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
			ca = (unsigned short)(a) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
			ca = (unsigned char)(a * 255.0f);
		}
		return (ca << 24) | (cb << 16) | (cg << 8) | cr;
	}

	///////////////////////////////////////////////
	template<class T>
	inline unsigned int Color_tpl<T >::pack_argb8888() const
	{
		unsigned char cr;
		unsigned char cg;
		unsigned char cb;
		unsigned char ca;
		if (sizeof(r) == 1) // char and unsigned char
		{
			cr = (unsigned char)r;
			cg = (unsigned char)g;
			cb = (unsigned char)b;
			ca = (unsigned char)a;
		}
		else if (sizeof(r) == 2) // short and unsigned short
		{
			cr = (unsigned short)(r) >> 8;
			cg = (unsigned short)(g) >> 8;
			cb = (unsigned short)(b) >> 8;
			ca = (unsigned short)(a) >> 8;
		}
		else // float or double
		{
			cr = (unsigned char)(r * 255.0f);
			cg = (unsigned char)(g * 255.0f);
			cb = (unsigned char)(b * 255.0f);
			ca = (unsigned char)(a * 255.0f);
		}
		return (ca << 24) | (cr << 16) | (cg << 8) | cb;
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::toHSV(f32& h, f32& s, f32& v) const
	{
		f32 r, g, b;
		if (sizeof(this->r) == 1) // 8bit integer
		{
			r = this->r * (1.0f / f32(0xff));
			g = this->g * (1.0f / f32(0xff));
			b = this->b * (1.0f / f32(0xff));
		}
		else if (sizeof(this->r) == 2) // 16bit integer
		{
			r = this->r * (1.0f / f32(0xffff));
			g = this->g * (1.0f / f32(0xffff));
			b = this->b * (1.0f / f32(0xffff));
		}
		else // floating point
		{
			r = this->r;
			g = this->g;
			b = this->b;
		}

		if ((b > g) && (b > r))
		{
			//if (!::iszero(v = b))
			{
				const f32 min = r < g ? r : g;
				const f32 delta = v - min;
				//if (!::iszero(delta))
				{
					s = delta / v;
					h = (240.0f / 360.0f) + (r - g) / delta * (60.0f / 360.0f);
				}
				//else
				{
					s = 0.0f;
					h = (240.0f / 360.0f) + (r - g) * (60.0f / 360.0f);
				}
				if (h < 0.0f) h += 1.0f;
			}
		//	else
			{
				s = 0.0f;
				h = 0.0f;
			}
		}
		else if (g > r)
		{
			//if (!::iszero(v = g))
			{
				const f32 min = r < b ? r : b;
				const f32 delta = v - min;
				//if (!::iszero(delta))
				{
					s = delta / v;
					h = (120.0f / 360.0f) + (b - r) / delta * (60.0f / 360.0f);
				}
				//else
				{
					s = 0.0f;
					h = (120.0f / 360.0f) + (b - r) * (60.0f / 360.0f);
				}
				if (h < 0.0f) h += 1.0f;
			}
			//else
			{
				s = 0.0f;
				h = 0.0f;
			}
		}
		else
		{
			//if (!::iszero(v = r))
			{
				const f32 min = g < b ? g : b;
				const f32 delta = v - min;
				//if (!::iszero(delta))
				{
					s = delta / v;
					h = (g - b) / delta * (60.0f / 360.0f);
				}
				//else
				{
					s = 0.0f;
					h = (g - b) * (60.0f / 360.0f);
				}
				if (h < 0.0f) h += 1.0f;
			}
			//else
			{
				s = 0.0f;
				h = 0.0f;
			}
		}
	}
#pragma warning(push)
#pragma warning(disable:4244)
	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::fromHSV(f32 h, f32 s, f32 v)
	{
		f32 r, g, b;
		//if (::iszero(v))
		{
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
		}
		//else if (::iszero(s))
		{
			r = v;
			g = v;
			b = v;
		}
		//else
		{
			const f32 hi = h * 6.0f;
			const int32 i = (int32)::floor(hi);
			const f32 f = hi - i;

			const f32 v0 = v * (1.0f - s);
			const f32 v1 = v * (1.0f - s * f);
			const f32 v2 = v * (1.0f - s * (1.0f - f));

			switch (i)
			{
			case 0:
				r = v;
				g = v2;
				b = v0;
				break;
			case 1:
				r = v1;
				g = v;
				b = v0;
				break;
			case 2:
				r = v0;
				g = v;
				b = v2;
				break;
			case 3:
				r = v0;
				g = v1;
				b = v;
				break;
			case 4:
				r = v2;
				g = v0;
				b = v;
				break;
			case 5:
				r = v;
				g = v0;
				b = v1;
				break;

			case 6:
				r = v;
				g = v2;
				b = v0;
				break;
			case -1:
				r = v;
				g = v0;
				b = v1;
				break;
			default:
				r = 0.0f;
				g = 0.0f;
				b = 0.0f;
				break;
			}
		}

		if (sizeof(this->r) == 1) // 8bit integer
		{
			this->r = r * f32(0xff);
			this->g = g * f32(0xff);
			this->b = b * f32(0xff);
		}
		else if (sizeof(this->r) == 2) // 16bit integer
		{
			this->r = r * f32(0xffff);
			this->g = g * f32(0xffff);
			this->b = b * f32(0xffff);
		}
		else // floating point
		{
			this->r = r;
			this->g = g;
			this->b = b;
		}
	}
#pragma warning(pop)
	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::clamp(T bottom, T top)
	{
		r = min(top, max(bottom, r));
		g = min(top, max(bottom, g));
		b = min(top, max(bottom, b));
		a = min(top, max(bottom, a));
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::maximum(const Color_tpl<T>& ca, const Color_tpl<T>& cb)
	{
		r = max(ca.r, cb.r);
		g = max(ca.g, cb.g);
		b = max(ca.b, cb.b);
		a = max(ca.a, cb.a);
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::minimum(const Color_tpl<T>& ca, const Color_tpl<T>& cb)
	{
		r = min(ca.r, cb.r);
		g = min(ca.g, cb.g);
		b = min(ca.b, cb.b);
		a = min(ca.a, cb.a);
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::abs()
	{
		r = fabs_tpl(r);
		g = fabs_tpl(g);
		b = fabs_tpl(b);
		a = fabs_tpl(a);
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::adjust_contrast(T c)
	{
		r = 0.5f + c * (r - 0.5f);
		g = 0.5f + c * (g - 0.5f);
		b = 0.5f + c * (b - 0.5f);
		a = 0.5f + c * (a - 0.5f);
	}

	//! Approximate values for each component's contribution to luminance.
	//! Based upon the NTSC standard described in ITU-R Recommendation BT.709.
	template<class T>
	inline void Color_tpl<T >::adjust_saturation(T s)
	{
		T grey = r * 0.2125f + g * 0.7154f + b * 0.0721f;
		r = grey + s * (r - grey);
		g = grey + s * (g - grey);
		b = grey + s * (b - grey);
		a = grey + s * (a - grey);
	}

	//! Optimized yet equivalent version of replacing luminance in XYZ space.
	//! Color and luminance are expected to be linear.
	template<class T>
	inline void Color_tpl<T >::adjust_luminance(float newLum)
	{
		Color_tpl<f32> colF(r, g, b, a);

		float lum = colF.r * 0.212671f + colF.g * 0.715160f + colF.b * 0.072169f;
		//if (iszero(lum))
			//return;

		*this = Color_tpl<T>(colF * newLum / lum);
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::lerpFloat(const Color_tpl<T>& ca, const Color_tpl<T>& cb, float s)
	{
		r = (T)(ca.r + s * (cb.r - ca.r));
		g = (T)(ca.g + s * (cb.g - ca.g));
		b = (T)(ca.b + s * (cb.b - ca.b));
		a = (T)(ca.a + s * (cb.a - ca.a));
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::negative(const Color_tpl<T>& c)
	{
		r = T(1.0f) - r;
		g = T(1.0f) - g;
		b = T(1.0f) - b;
		a = T(1.0f) - a;
	}

	///////////////////////////////////////////////
	template<class T>
	inline void Color_tpl<T >::grey(const Color_tpl<T>& c)
	{
		T m = (r + g + b) / T(3);

		r = m;
		g = m;
		b = m;
		a = a;
	}
} // nv namespace

#endif // 
