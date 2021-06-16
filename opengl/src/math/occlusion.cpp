
#include "occlusion.h"

namespace math
{

	void getPosAndNegPointProjPlane(const  V4f*plane, const V3f&boxMin, const V3f&boxMax,V3f * out)
	{
		V3f* p = &out[0];
		*p = boxMin;

		if (plane->x >= 0)
			p->x = boxMax.x;
		if (plane->y >= 0)
			p->y = boxMax.y;
		if (plane->z >= 0)
			p->z = boxMax.z;

		V3f* n = &out[1];
		*n = boxMax;
		if (plane->x >= 0)
			n->x = boxMin.x;
		if (plane->y >= 0)
			n->y = boxMin.y;
		if (plane->z >= 0)
			n->z = boxMin.z;
	}

 

	void LIBENIGHT_EXPORT getPosAndNegPointProjPlane(const float*plane, const V3f&boxMin, const V3f&boxMax, V3f * out)
	{
		V3f* p = &out[0];
		*p = boxMin;

		if (plane[0] >= 0)
			p->x = boxMax.x;
		if (plane[1] >= 0)
			p->y = boxMax.y;
		if (plane[2] >= 0)
			p->z = boxMax.z;

		V3f* n = &out[1];
		*n = boxMax;
		if (plane[0] >= 0)
			n->x = boxMin.x;
		if (plane[1] >= 0)
			n->y = boxMin.y;
		if (plane[2] >= 0)
			n->z = boxMin.z;
	}

}
