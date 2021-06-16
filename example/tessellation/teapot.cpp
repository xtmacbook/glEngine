
#include "type.h"
#include <memory>
#include <vector>
#include <algorithm>
#include "alg.h"
#include "helpF.h"

using namespace math;

#include "../teapotdata.h"

void tess_teapot(const V3f*p,int level)
{

}
 
V3f evalBezierPatch(const V3f *controlPoints, const float &u, const float &v)
{
	V3f uCurve[4];
	for (int i = 0; i < 4; ++i)
		uCurve[i] = evalBezierCurve(controlPoints + 4 * i, u);

	return evalBezierCurve(uCurve, v);
}


void createPolyTeapot(int level,std::vector<V3f>&Vs,
								std::vector<V3f>&Ns,
								std::vector<V2f>&st,
								std::vector<uint32>&indices)
{
	int addVertices = (level + 1) * (level + 1);
	int addVIndices =  2 * (level + 1) * level + level;

	 V3f* P(new V3f[addVertices]);
	 uint32*verticesIndex(new uint32[addVIndices]);
	 V3f* N(new V3f[addVertices]);
	 //std::unique_ptr<V2f[]> st(new V2f[(level + 1) * (level + 1)]);

	 int k = 0;
	 for (uint16 j = 0; j < level; ++j) 
	 {
		 for (uint16_t i = 0; i <= level; ++i) 
		 {
			 verticesIndex[k++] = (level + 1) * (j + 1) + i;
			 verticesIndex[k++] = (level + 1) * j + i;
		 }
		 verticesIndex[k++] = 0xffffffff;
	 }

	V3f  controlPoints[16];
	for (int i = 0; i < 32; i++)
	{
		unsigned short * controlPointsIndex = teapotPatches[i];

		for (unsigned short ci = 0; ci < 16; ci++)
		{
			controlPoints[ci].x = teapotVertices[controlPointsIndex[ci] - 1][0];
			controlPoints[ci].y= teapotVertices[controlPointsIndex[ci] - 1][1];
			controlPoints[ci].z = teapotVertices[controlPointsIndex[ci] - 1][2];
		}

		for (uint16_t j = 0, k = 0; j <= level; ++j) 
		{
			float v = j / (float)level;
			for (uint16_t i = 0; i <= level; ++i, ++k) 
			{
				float u = i / (float)level;
				P[k] = evalBezierPatch(controlPoints, u, v);

				V3f dU = dUBezier(controlPoints, u, v);
				V3f dV = dVBezier(controlPoints, u, v);
				N[k] = normalize(math::crossVec3(dU, dV));
			}
		}
		
		std::copy(verticesIndex, verticesIndex + addVIndices, indices.begin() + i * addVIndices);
		std::copy(P, P + (level + 1) * (level + 1), Vs.begin() + i * addVertices);
		std::copy(N, N + (level + 1) * (level + 1), Ns.begin() + i * addVertices);

		for (int i = 0; i < addVIndices; i++)
			if(verticesIndex[i] != 0xffffffff) verticesIndex[i] += addVertices;
	}

	delete[] P;
	delete[] verticesIndex;
}
