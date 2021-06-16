
//https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/procedural-patterns-noise-part-1/simple-pattern-examples

#include <cmath> 
#include <cstdio> 
#include <random> 
#include <functional> 
#include <iostream> 
#include <fstream> 
#include <helpF.h>
template<typename T>
class Vec2
{
public:
	Vec2() : x(T(0)), y(T(0)) {}
	Vec2(T xx, T yy) : x(xx), y(yy) {}
	Vec2 operator * (const T &r) const { return Vec2(x * r, y * r); }
	Vec2& operator *= (const T &r) { x *= r, y *= r; return *this; }
	T x, y;
};

typedef Vec2<float> Vec2f;

template<typename T = float>
inline T lerp(const T &lo, const T &hi, const T &t)
{
	return lo * (1 - t) + hi * t;
}

inline float smoothstep(const float &t)
{
	return t * t * (3 - 2 * t);
}

class ValueNoise
{
public:
	ValueNoise(unsigned seed = 2016)
	{
		std::mt19937 gen(seed);
		std::uniform_real_distribution<float> distrFloat;
		auto randFloat = std::bind(distrFloat, gen);

		// create an array of random values and initialize permutation table
		for (unsigned k = 0; k < kMaxTableSize; ++k) {
			r[k] = randFloat();
			permutationTable[k] = k;
		}

		// shuffle values of the permutation table
		std::uniform_int_distribution<unsigned> distrUInt;
		auto randUInt = std::bind(distrUInt, gen);
		for (unsigned k = 0; k < kMaxTableSize; ++k) {
			unsigned i = randUInt() & kMaxTableSizeMask;
			std::swap(permutationTable[k], permutationTable[i]);
			permutationTable[k + kMaxTableSize] = permutationTable[k];
		}
	}

	float eval(Vec2f &p) const
	{
		int xi = std::floor(p.x);
		int yi = std::floor(p.y);

		float tx = p.x - xi;
		float ty = p.y - yi;

		int rx0 = xi & kMaxTableSizeMask;
		int rx1 = (rx0 + 1) & kMaxTableSizeMask;
		int ry0 = yi & kMaxTableSizeMask;
		int ry1 = (ry0 + 1) & kMaxTableSizeMask;

		// random values at the corners of the cell using permutation table
		const float & c00 = r[permutationTable[permutationTable[rx0] + ry0]];
		const float & c10 = r[permutationTable[permutationTable[rx1] + ry0]];
		const float & c01 = r[permutationTable[permutationTable[rx0] + ry1]];
		const float & c11 = r[permutationTable[permutationTable[rx1] + ry1]];

		// remapping of tx and ty using the Smoothstep function 
		float sx = smoothstep(tx);
		float sy = smoothstep(ty);

		// linearly interpolate values along the x axis
		float nx0 = lerp(c00, c10, sx);
		float nx1 = lerp(c01, c11, sx);

		// linearly interpolate the nx0/nx1 along they y axis
		return lerp(nx0, nx1, sy);
	}
	static const unsigned kMaxTableSize = 256;
	static const unsigned kMaxTableSizeMask = kMaxTableSize - 1;
	float r[kMaxTableSize];
	unsigned permutationTable[kMaxTableSize * 2];
};

int main()
{
	unsigned imageWidth = 512;
	unsigned imageHeight = 512;
	float *noiseMap = new float[imageWidth * imageHeight]{ 0 };

	unsigned seed = 2016;
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> distr;
	auto dice = std::bind(distr, gen); // std::function<float()> 

	ValueNoise noise;

	/************************************************************************/
	/* white noise                                                                     */
	/************************************************************************/
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			// generate a float in the range [0:1]
			noiseMap[j * imageWidth + i] = dice();
		}
	}
	std::ofstream ofs;
	ofs.open("./white_noise.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();
	/************************************************************************/
	/*          value_noise                                                             */
	/************************************************************************/
	float frequency = 0.5f;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			// generate a float in the range [0:1]
			noiseMap[j * imageWidth + i] = noise.eval(Vec2f(i, j) * frequency);
		}
	}
	ofs.open("./value_noise_0.5.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();
	/************************************************************************/
	/*           value_noise                                                           */
	/************************************************************************/
	frequency = 0.05f;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			// generate a float in the range [0:1]
			noiseMap[j * imageWidth + i] = noise.eval(Vec2f(i, j) * frequency);
		}
	}
	ofs.open("./value_noise_0.05.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();


	/************************************************************************/
	/*  fractal pattern                                                                     */
	/************************************************************************/
	frequency = 0.02f;
	float frequencyMult = 1.8;
	float amplitudeMult = 0.35;
	unsigned numLayers = 5;
	float maxNoiseVal = 0;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			Vec2f pNoise = Vec2f(i, j) * frequency;
			float amplitude = 1;
			for (unsigned l = 0; l < numLayers; ++l) {
				noiseMap[j * imageWidth + i] += noise.eval(pNoise) * amplitude;
				pNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			if (noiseMap[j * imageWidth + i] > maxNoiseVal) maxNoiseVal = noiseMap[j * imageWidth + i];
		}
	}
	for (unsigned i = 0; i < imageWidth * imageHeight; ++i) noiseMap[i] /= maxNoiseVal;
	ofs.open("./fractal.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	/************************************************************************/
	/*           turbulence  for simulate fire, smoke or clouds                                                           */
	/************************************************************************/
	frequency = 0.02f;
	frequencyMult = 1.8;
	amplitudeMult = 0.35;
	numLayers = 5; 
	maxNoiseVal = 0;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			Vec2f pNoise = Vec2f(i, j) * frequency;
			float amplitude = 1;
			for (unsigned l = 0; l < numLayers; ++l) {
				noiseMap[j * imageWidth + i] += std::fabs(2 * noise.eval(pNoise) - 1) * amplitude;
				pNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			if (noiseMap[j * imageWidth + i] > maxNoiseVal) maxNoiseVal = noiseMap[j * imageWidth + i];
		}
	}
	for (unsigned i = 0; i < imageWidth * imageHeight; ++i) noiseMap[i] /= maxNoiseVal;
	ofs.open("./turbulence.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();
	/************************************************************************/
	/* marble                                                                     */
	/************************************************************************/
	frequency = 0.02f;
	frequencyMult = 1.8;
	amplitudeMult = 0.35;
	numLayers = 5;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			Vec2f pNoise = Vec2f(i, j) * frequency;
			float amplitude = 1;
			float noiseValue = 0;
			// compute some fractal noise
			for (unsigned l = 0; l < numLayers; ++l) {
				noiseValue += noise.eval(pNoise) * amplitude;
				pNoise *= frequencyMult;
				amplitude *= amplitudeMult;
			}
			// we "displace" the value i used in the sin() expression by noiseValue * 100
			noiseMap[j * imageWidth + i] = (sin((i + noiseValue * 100) * 2 * math::Const<float>::pi() / 200.f) + 1) / 2.f;
		}
	}
	ofs.open("./marble.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();

	/*
		wood 
	*/
	frequency = 0.01f;
	for (unsigned j = 0; j < imageHeight; ++j) {
		for (unsigned i = 0; i < imageWidth; ++i) {
			float g = noise.eval(Vec2f(i, j) * frequency) * 10;
			noiseMap[j * imageWidth + i] = g - (int)g;
		}
	}
	ofs.open("./wood.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned k = 0; k < imageWidth * imageHeight; ++k) {
		unsigned char n = static_cast<unsigned char>(noiseMap[k] * 255);
		ofs << n << n << n;
	}
	ofs.close();
	return 0;
}
