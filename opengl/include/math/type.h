//
//  type.h
//  openGLTest
//
//  Created by xt on 18/6/10.
//  Copyright © 2018 xt. All rights reserved.
//

#ifndef type_h
#define type_h
#include "sys.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace math {

	template <class REAL>
	using  Vector3 = glm::tvec3<REAL, glm::highp>;

	template <class REAL>
	using  Vector4 = glm::tvec4<REAL, glm::highp>;

	template <class REAL>
	using Vector2 = glm::tvec2<REAL, glm::highp>;

	template <class REAL>
	using Matrix4 = glm::tmat4x4<REAL, glm::highp>;

	template <class REAL>
	using Quat = glm::tquat<REAL, glm::highp>;

	template <class REAL>
	using Matrix3 = glm::tmat3x3<REAL, glm::highp>;

	template <class REAL>
	using Matrix34 = glm::tmat3x4<REAL, glm::highp>;

		// Type definitions:
	typedef posh_u8_t   uint8;
	typedef posh_i8_t   int8;

	typedef posh_u16_t  uint16;
	typedef posh_i16_t  int16;

	typedef posh_u32_t  uint32;
	typedef posh_i32_t  int32;

	typedef posh_u64_t  uint64;
	typedef posh_i64_t  int64;

	// Aliases
	typedef uint32			uint;
	typedef Vector4<int16>	Vec4I16;   //!< Used for tangents only.

	typedef	float			f32;
	typedef uint16			Half;

	typedef Vector4<float>   V4f;
	typedef Vector4<double>  V4d;
	typedef Vector4<int>	 V4i;
	typedef Vector3<float>   V3f;
	typedef Vector3<double>  V3d;
	typedef Vector2<int>     V2i;
	typedef Vector2<unsigned int>     V2Ui;
	typedef Vector2<uint8>   V2ui8;
	typedef Vector2<float>   V2f;
	typedef Vector2<double>  V2d;
	typedef Matrix3<float>   Matrix3f;
	typedef Matrix3<double>  Matrix3d;
	typedef Matrix4<float>   Matrixf;
	typedef Matrix4<double>  Matrixd;
	typedef Matrix34<double> Matrix34d;
	typedef Matrix34<float>  Matrix34f;
	typedef Quat<float >     Quatf;
}
#endif /* type_h */
