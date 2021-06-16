#include <type.h>

using namespace math;



/************************************************************************/
/* perlin noise is Gradients noise                                                                   */
/************************************************************************/
#define  SAMPLE_SIZE 1024
class ClassPerlineNoise
{
public:

	ClassPerlineNoise(int octaves, float freq, float amp, int seed);


	float eval(float x, float y)
	{
		float vec[2];
		vec[0] = x;
		vec[1] = y;
		return perlin_noise_2D(vec);
	};

	float eval(float x, float y,float z)
	{
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		return perlin_noise_3D(vec);
	};

private:
	float perlin_noise_2D(float vec[2]);
	float perlin_noise_3D(float vec[3]);

	float noise1(float arg);
	float noise2(float vec[2]);
	float noise3(float vec[3]);
	void normalize2(float v[2]);
	void normalize3(float v[3]);
	void init(void);

	int   mOctaves;
	float mFrequency;
	float mAmplitude;
	int   mSeed;

	int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
	float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
	float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
	bool  mStart;

};


class ImprovedPerlinNoise
{
public:
	ImprovedPerlinNoise(const unsigned &seed = 2016);
	virtual ~ImprovedPerlinNoise();

	//[comment]
	// Improved Noise implementation (2002)
	// This version compute the derivative of the noise function as well
	//[/comment]
	float eval(const V3f &p, V3f& derivs) const;

	//[comment]
	// classic/original Perlin noise implementation (1985)
	//[/comment]
	float eval(const V3f &p) const;

private:
	/* inline */
	uint8_t hash(const int &x, const int &y, const int &z) const;
 
	float gradientDotV(
		uint8_t perm, // a value between 0 and 255
		float x, float y, float z) const;

	static const unsigned tableSize = 256;
	static const unsigned tableSizeMask = tableSize - 1;
	V3f gradients[tableSize];
	unsigned permutationTable[tableSize * 2];
};

/************************************************************************/
/* http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf       
	
	https://catlikecoding.com/unity/tutorials/simplex-noise/

	http://knielsen-hq.org/simplex_noise_skew_factor.pdf
*/
/************************************************************************/
class SimplexPerlinNoise
{
public:

	static void init();

	static double noise2D(double  x, double y);
	static double noise3D(double  x, double y, double z);
	static double noise4D(double  x, double y, double z,double w);
private:
	static V3f grad3[12];
	//static V4f grad4[32];

	static short p[256];

	static short perm[512];
	static short permMod12[512] ;

	static double F2;
	static double G2;

	static double F3;
	static double G3;

	static double F4;
	static double G4;
};


__declspec(selectany) double SimplexPerlinNoise::F2 = 0.5*(std::sqrt(3.0) - 1.0);//trianglesToSquares 
__declspec(selectany) double SimplexPerlinNoise::G2 = (3.0 - std::sqrt(3.0)) / 6.0;

__declspec(selectany) double SimplexPerlinNoise::F3 = 1.0 / 3.0;
__declspec(selectany) double SimplexPerlinNoise::G3 = 1.0 / 6.0;

__declspec(selectany) double SimplexPerlinNoise::F4 = (std::sqrt(5.0) - 1.0) / 4.0;
__declspec(selectany) double SimplexPerlinNoise::G4 = (5.0 - std::sqrt(5.0)) / 20.0;

