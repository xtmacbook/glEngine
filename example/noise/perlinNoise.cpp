#include "perlineNoise.h"
#include "helpF.h"
#include <cmath>
#include <cstdio>
#include <random>
#include <functional>
#include <iostream>
#include <algorithm>


/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */


/************************************************************************/
/* https://catlikecoding.com/unity/tutorials/noise/ ¿ÉÒÔ²Î¿¼Õâ¸ö gradient ¸üºÃµÄÀí½â"gradient" µÄÒâË¼                                                                     */
/************************************************************************/


float dot(const V3f &a, const V3f &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(const V3f& g, float x, float y) 
{
	return g.x*x + g.y*y;
}

float dot(const V3f&g, float x, float y, float z) 
{
	return g.x*x + g.y*y + g.z*z;
}

template<typename T = float>
T lerp(const T &lo, const T &hi, const T &t)
{
	return lo * (1 - t) + hi * t;
}

float smoothstep(const float &t)
{
	return t * t * (3.0f - 2.0f * t);
}

float quintic(const float &t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}


float smoothstepDeriv(const float &t)
{
	return t * (6 - 6 * t);
}

float quinticDeriv(const float &t)
{
	return 30 * t * t * (t * (t - 2) + 1);
}

////////////////////////////////////////////////////////
/////
#define B SAMPLE_SIZE
#define BM (SAMPLE_SIZE-1)

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff


#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.0f;

float ClassPerlineNoise::noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	if (mStart)
	{
		srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0, bx1, rx0, rx1);

	sx = smoothstep(rx0);

	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return lerp(u, v, sx);
}

float ClassPerlineNoise::noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (mStart)
	{
		srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = smoothstep(rx0);
	sy = smoothstep(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[b00];
	u = at2(rx0, ry0);
	q = g2[b10];
	v = at2(rx1, ry0);
	a = lerp( u, v,sx);

	q = g2[b01];
	u = at2(rx0, ry1);
	q = g2[b11];
	v = at2(rx1, ry1);
	b = lerp(u, v,sx);

	return lerp(a, b,sy);
}

float ClassPerlineNoise::noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (mStart)
	{
		srand(mSeed);
		mStart = false;
		init();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t = smoothstep(rx0);
	sy = smoothstep(ry0);
	sz = smoothstep(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
	a = lerp(u, v,t);

	q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
	b = lerp(u, v,t);

	c = lerp(a, b,sy);

	q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
	a = lerp(u, v,t );

	q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
	b = lerp(u, v,t);

	d = lerp(a, b,sy);

	return lerp(c, d,sz);
}

void ClassPerlineNoise::normalize2(float v[2])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	s = 1.0f / s;
	v[0] = v[0] * s;
	v[1] = v[1] * s;
}

void ClassPerlineNoise::normalize3(float v[3])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	s = 1.0f / s;

	v[0] = v[0] * s;
	v[1] = v[1] * s;
	v[2] = v[2] * s;
}

void ClassPerlineNoise::init(void)
{
	int i, j, k;

	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (float)((rand() % (B + B)) - B) / B;
		for (j = 0; j < 2; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
		for (j = 0; j < 3; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}


float ClassPerlineNoise::perlin_noise_2D(float vec[2])
{
	int terms = mOctaves;
	float freq = mFrequency;
	float result = 0.0f;
	float amp = mAmplitude;

	vec[0] *= mFrequency;
	vec[1] *= mFrequency;

	for (int i = 0; i < terms; i++)
	{
		result += noise2(vec)*amp;
		vec[0] *= 2.0f;
		vec[1] *= 2.0f;
		amp *= (0.5f);
	}
	return result;
}

float ClassPerlineNoise::perlin_noise_3D(float vec[3])
{
	int terms = mOctaves;
	float freq = mFrequency;
	float result = 0.0f;
	float amp = mAmplitude;

	vec[0] *= mFrequency;
	vec[1] *= mFrequency;
	vec[2] *= mFrequency;

	for (int i = 0; i < terms; i++)
	{
		result += noise3(vec)*amp;
		vec[0] *= 2.0f;
		vec[1] *= 2.0f;
		vec[2] *= 2.0f;
		amp *= (0.5f);
	}
	return result;
}
ClassPerlineNoise::ClassPerlineNoise(int octaves, float freq, float amp, int seed)
{
	mOctaves = octaves;
	mFrequency = freq;
	mAmplitude = amp;
	mSeed = seed;
	mStart = true;
}



ImprovedPerlinNoise::ImprovedPerlinNoise(const unsigned &seed)
{
	std::mt19937 generator(seed);
	std::uniform_real_distribution<float> distribution;
	auto dice = std::bind(distribution, generator);
	for (unsigned i = 0; i < tableSize; ++i) {
#if 0
		// bad
		float gradientLen2;
		do {
			gradients[i] = Vec3f(2 * dice() - 1, 2 * dice() - 1, 2 * dice() - 1);
			gradientLen2 = gradients[i].length2();
		} while (gradientLen2 > 1);
		gradients[i].normalize();
#else
		// better
		float theta = acos(2 * dice() - 1);
		float phi = 2 * dice() * Const<float>::pi();

		float x = cos(phi) * sin(theta);
		float y = sin(phi) * sin(theta);
		float z = cos(theta);
		gradients[i] = V3f(x, y, z);
#endif
		permutationTable[i] = i;
	}

	std::uniform_int_distribution<unsigned> distributionInt;
	auto diceInt = std::bind(distributionInt, generator);
	// create permutation table
	for (unsigned i = 0; i < tableSize; ++i)
		std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
	// extend the permutation table in the index range [256:512]
	for (unsigned i = 0; i < tableSize; ++i) {
		permutationTable[tableSize + i] = permutationTable[i];
	}
}
ImprovedPerlinNoise:: ~ImprovedPerlinNoise() {}

//[comment]
// Improved Noise implementation (2002)
// This version compute the derivative of the noise function as well
//[/comment]
float ImprovedPerlinNoise::eval(const V3f &p, V3f& derivs) const
{
	int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
	int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
	int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

	int xi1 = (xi0 + 1) & tableSizeMask;
	int yi1 = (yi0 + 1) & tableSizeMask;
	int zi1 = (zi0 + 1) & tableSizeMask;

	float tx = p.x - ((int)std::floor(p.x));
	float ty = p.y - ((int)std::floor(p.y));
	float tz = p.z - ((int)std::floor(p.z));

	float u = quintic(tx);
	float v = quintic(ty);
	float w = quintic(tz);

	// generate vectors going from the grid points to p
	float x0 = tx, x1 = tx - 1;
	float y0 = ty, y1 = ty - 1;
	float z0 = tz, z1 = tz - 1;

	float a = gradientDotV(hash(xi0, yi0, zi0), x0, y0, z0);
	float b = gradientDotV(hash(xi1, yi0, zi0), x1, y0, z0);
	float c = gradientDotV(hash(xi0, yi1, zi0), x0, y1, z0);
	float d = gradientDotV(hash(xi1, yi1, zi0), x1, y1, z0);
	float e = gradientDotV(hash(xi0, yi0, zi1), x0, y0, z1);
	float f = gradientDotV(hash(xi1, yi0, zi1), x1, y0, z1);
	float g = gradientDotV(hash(xi0, yi1, zi1), x0, y1, z1);
	float h = gradientDotV(hash(xi1, yi1, zi1), x1, y1, z1);

	float du = quinticDeriv(tx);
	float dv = quinticDeriv(ty);
	float dw = quinticDeriv(tz);

	float k0 = a;
	float k1 = (b - a);
	float k2 = (c - a);
	float k3 = (e - a);
	float k4 = (a + d - b - c);
	float k5 = (a + f - b - e);
	float k6 = (a + g - c - e);
	float k7 = (b + c + e + h - a - d - f - g);

	derivs.x = du *(k1 + k4 * v + k5 * w + k7 * v * w);
	derivs.y = dv *(k2 + k4 * u + k6 * w + k7 * v * w);
	derivs.z = dw *(k3 + k5 * u + k6 * v + k7 * v * w);

	return k0 + k1 * u + k2 * v + k3 * w + k4 * u * v + k5 * u * w + k6 * v * w + k7 * u * v * w;
}

//[comment]
// classic/original Perlin noise implementation (1985)
//[/comment]
float ImprovedPerlinNoise::eval(const V3f &p) const
{
	int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
	int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
	int zi0 = ((int)std::floor(p.z)) & tableSizeMask;

	int xi1 = (xi0 + 1) & tableSizeMask;
	int yi1 = (yi0 + 1) & tableSizeMask;
	int zi1 = (zi0 + 1) & tableSizeMask;

	float tx = p.x - ((int)std::floor(p.x));
	float ty = p.y - ((int)std::floor(p.y));
	float tz = p.z - ((int)std::floor(p.z));

	float u = smoothstep(tx);
	float v = smoothstep(ty);
	float w = smoothstep(tz);

	// gradients at the corner of the cell
	const V3f &c000 = gradients[hash(xi0, yi0, zi0)];
	const V3f &c100 = gradients[hash(xi1, yi0, zi0)];
	const V3f &c010 = gradients[hash(xi0, yi1, zi0)];
	const V3f &c110 = gradients[hash(xi1, yi1, zi0)];
	const V3f &c001 = gradients[hash(xi0, yi0, zi1)];
	const V3f &c101 = gradients[hash(xi1, yi0, zi1)];
	const V3f &c011 = gradients[hash(xi0, yi1, zi1)];
	const V3f &c111 = gradients[hash(xi1, yi1, zi1)];

	// generate vectors going from the grid points to p
	float x0 = tx, x1 = tx - 1;
	float y0 = ty, y1 = ty - 1;
	float z0 = tz, z1 = tz - 1;

	V3f p000 = V3f(x0, y0, z0);
	V3f p100 = V3f(x1, y0, z0);
	V3f p010 = V3f(x0, y1, z0);
	V3f p110 = V3f(x1, y1, z0);

	V3f p001 = V3f(x0, y0, z1);
	V3f p101 = V3f(x1, y0, z1);
	V3f p011 = V3f(x0, y1, z1);
	V3f p111 = V3f(x1, y1, z1);

	// linear interpolation
	float a = lerp(dot(c000, p000), dot(c100, p100), u);
	float b = lerp(dot(c010, p010), dot(c110, p110), u);
	float c = lerp(dot(c001, p001), dot(c101, p101), u);
	float d = lerp(dot(c011, p011), dot(c111, p111), u);

	float e = lerp(a, b, v);
	float f = lerp(c, d, v);

	return lerp(e, f, w); // g
}

uint8_t ImprovedPerlinNoise::hash(const int &x, const int &y, const int &z) const
{
	return permutationTable[permutationTable[permutationTable[x] + y] + z];
}

//[comment]
// Compute dot product between vector from cell corners to P with predefined gradient directions
//
//    perm: a value between 0 and 255
//
//    float x, float y, float z: coordinates of vector from cell corner to shaded point
//[/comment]
float ImprovedPerlinNoise::gradientDotV(
	uint8_t perm, // a value between 0 and 255
	float x, float y, float z) const
{
	switch (perm & 15) {
	case  0: return  x + y; // (1,1,0)
	case  1: return -x + y; // (-1,1,0)
	case  2: return  x - y; // (1,-1,0)
	case  3: return -x - y; // (-1,-1,0)
	case  4: return  x + z; // (1,0,1)
	case  5: return -x + z; // (-1,0,1)
	case  6: return  x - z; // (1,0,-1)
	case  7: return -x - z; // (-1,0,-1)
	case  8: return  y + z; // (0,1,1),
	case  9: return -y + z; // (0,-1,1),
	case 10: return  y - z; // (0,1,-1),
	case 11: return -y - z; // (0,-1,-1)
	case 12: return  y + x; // (1,1,0)
	case 13: return -x + y; // (-1,1,0)
	case 14: return -y + z; // (0,-1,1)
	case 15: return -y - z; // (0,-1,-1)
	}
}


short SimplexPerlinNoise::p[256] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

V3f SimplexPerlinNoise::grad3[12]= { V3f(1,1,0), V3f(-1,1,0),V3f(1,-1,0),V3f(-1,-1,0),
V3f(1,0,1),V3f(-1,0,1),V3f(1,0,-1),V3f(-1,0,-1),
V3f(0,1,1),V3f(0,-1,1),V3f(0,1,-1),V3f(0,-1,-1) };

short SimplexPerlinNoise::perm[512];
short SimplexPerlinNoise::permMod12[512];

void SimplexPerlinNoise::init()
{
	for (int i = 0; i < 512; i++)
	{
		perm[i] = p[i & 255];
		permMod12[i] = (short)(perm[i] % 12);
	}
}

double SimplexPerlinNoise::noise2D(double x, double y)
{
	// Skew the input space to determine which simplex cell we're in
	double s = (x + y)*F2;   //from rhombus to sequre
	int i = (int)std::floor(x + s);  
	int j = (int)std::floor(y + s);

	double t = (i + j)*G2;
	double X0 = i - t; // Unskew the cell origin back to (x,y) space
	double Y0 = j - t;

	double x0 = x - X0; // The x,y distances from the cell origin
	double y0 = y - Y0;


	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)
								  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
								  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
								  // c = (3-sqrt(3))/6


	double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	double y2 = y0 - 1.0 + 2.0 * G2;
	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = permMod12[ii + perm[jj]];
	int gi1 = permMod12[ii + i1 + perm[jj + j1]];
	int gi2 = permMod12[ii + 1 + perm[jj + 1]];
	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0*x0 - y0*y0; //https://blog.csdn.net/candycat1992/article/details/50346469

	double n0, n1, n2; // Noise contributions from the three corners
	if (t0 < 0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0);  // (x,y) of grad3 used for 2D gradient
	}
	double t1 = 0.5 - x1*x1 - y1*y1;
	if (t1 < 0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}
	double t2 = 0.5 - x2*x2 - y2*y2;
	if (t2 < 0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * (n0 + n1 + n2);

}

double SimplexPerlinNoise::noise3D(double x, double y, double z)
{
	double n0, n1, n2, n3; // Noise contributions from the four corners
						   // Skew the input space to determine which simplex cell we're in
	double s = (x + y + z)*F3; // Very nice and simple skew factor for 3D
	int i = (int)std::floor(x + s);
	int j = (int)std::floor(y + s);
	int k = (int)std::floor(z + s);
	double t = (i + j + k)*G3;
	double X0 = i - t; // Unskew the cell origin back to (x,y,z) space
	double Y0 = j - t;
	double Z0 = k - t;
	double x0 = x - X0; // The x,y,z distances from the cell origin
	double y0 = y - Y0;
	double z0 = z - Z0;
	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
	int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
	if (x0 >= y0) {
		if (y0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		} // X Y Z order
		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
	}
	else { // x0<y0
		if (y0<z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
		else if (x0<z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
	}
	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	double x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
	double y1 = y0 - j1 + G3;
	double z1 = z0 - k1 + G3;
	double x2 = x0 - i2 + 2.0*G3; // Offsets for third corner in (x,y,z) coords
	double y2 = y0 - j2 + 2.0*G3;
	double z2 = z0 - k2 + 2.0*G3;
	double x3 = x0 - 1.0 + 3.0*G3; // Offsets for last corner in (x,y,z) coords
	double y3 = y0 - 1.0 + 3.0*G3;
	double z3 = z0 - 1.0 + 3.0*G3;
	// Work out the hashed gradient indices of the four simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	int gi0 = permMod12[ii + perm[jj + perm[kk]]];
	int gi1 = permMod12[ii + i1 + perm[jj + j1 + perm[kk + k1]]];
	int gi2 = permMod12[ii + i2 + perm[jj + j2 + perm[kk + k2]]];
	int gi3 = permMod12[ii + 1 + perm[jj + 1 + perm[kk + 1]]];
	// Calculate the contribution from the four corners
	double t0 = 0.6 - x0*x0 - y0*y0 - z0*z0;
	if (t0<0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0, z0);
	}
	double t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;
	if (t1<0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1, z1);
	}
	double t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;
	if (t2<0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2, z2);
	}
	double t3 = 0.6 - x3*x3 - y3*y3 - z3*z3;
	if (t3<0) n3 = 0.0;
	else {
		t3 *= t3;
		n3 = t3 * t3 * dot(grad3[gi3], x3, y3, z3);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0*(n0 + n1 + n2 + n3);
}

double SimplexPerlinNoise::noise4D(double x, double y, double z, double w)
{
	return 0.0;
}
