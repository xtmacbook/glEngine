#include "worley.h"
#include "type.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace math;

V2f random2(V2f p) 
{
	return glm::fract(
		sin(V2f(dot(p, V2f(127.1, 311.7)), dot(p, V2f(269.5, 183.3))))*43758.5453f
	);
}
float smoothstep(float t)
{
	return t * t * (3.0 - 2.0 * t);
}

int main()
{
	float scale = 5.0;//2n + 1
	
	const uint32_t width = 512, height = 512;
	float *noiseMap = new float[width * height];
	for (uint32_t j = 0; j < height; ++j)
	{
		for (uint32_t i = 0; i < width; ++i)
		{
			V2f st =  V2f(i,j) / V2f(width,height);
			st *= scale;

			V2f f_st = fract(st);
			V2f i_st = floor(st);

			float min_dis = 1.0;
			V3f color = V3f(.0);

			V2f min_point;

			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					V2f neighbor = V2f(float(i), float(j));
					V2f n_point = random2(i_st + neighbor);
					V2f diff = neighbor + n_point - f_st;

					if (length(diff) < min_dis)
					{
						min_dis = length(diff);
						min_point = n_point;
					}
				}
			}

			noiseMap[j * width + i] = min_dis;
		}
	}
	std::ofstream ofs;
	ofs.open("./worley_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned k = 0; k < width * height; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	return 0;
}
