
#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "decl.h"
#include "type.h"

using namespace math;
using namespace base;

struct LIBENIGHT_EXPORT Vertex
{
	V3f Position;
	// normal
	V3f Normal;
	// texCoords
	V2f TexCoords;
	// tangent
	V3f Tangent;
	// bitangent
	V3f Bitangent;
};
#endif
