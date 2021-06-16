
static int level0 = 0;
static int level1 = 0;
static int level2 = 0;
static int level3 = 0;
static int level4 = 0;
static int level5 = 0;

static int num_level0 = 0;
static int num_level1 = 0;
static int num_level2 = 0;
static int num_level3 = 0;
static int num_level4 = 0;
static int num_level5 = 0;

#include "type.h"

using namespace math;

extern void clearColor()
{
	level0 = 0;
	level1 = 0;
	level2 = 0;
	level3 = 0;
	level4 = 0;
	level5 = 0;

	num_level0 = 0;
	num_level1 = 0;
	num_level2 = 0;
	num_level3 = 0;
	num_level4 = 0;
	num_level5 = 0;
}

extern void calPatchColor(int iLOD)
{
	if (iLOD == 0)
	{
		num_level0++;
	}
	else if (iLOD == 1)
	{
		num_level1++;
	}
	else if (iLOD == 2)
	{
		num_level2++;
	}
	else if (iLOD == 3)
	{
		num_level3++;
	}
	else if (iLOD == 4)
	{
		num_level4++;
	}
	else if (iLOD == 5)
	{
		num_level5++;
	}

}

extern V3f getPatchColor(int iLOD)
{

	V3f color(1.0f,1.0,1.0);
	if (iLOD == 0)
	{
		level0++;
		color = V3f(level0 / (float)num_level0, 0.0, 0.0);
	}
	else if (iLOD == 1)
	{
		level1++;
		color = V3f(0.0, level1 / (float)num_level1, 0.0);
	}
	else if (iLOD == 2)
	{
		level2++;
		color = V3f(0.0, 0.0, level2 / (float)num_level2);
	}
	else if (iLOD == 3)
	{
		level3++;
		color = V3f(0.0, 1.0, 1.0);
	}
	
	return color;
}

