

#include <scene.h>
#include <RenderNode.h>
#include <shader.h>
#include <engineLoad.h>
#include <resource.h>
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <log.h>
#include <helpF.h>
#include <baseMesh.h>
#include <Inputmanager.h>
#include <glinter.h>


#include <cmath>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

template<typename T = float>
inline T lerp(const T &lo, const T &hi, const T &t)
{
	return lo * (1 - t) + hi * t;
}

class ValueNoise1D
{
public:
	ValueNoise1D(unsigned seed = 2011)
	{
		srand(seed);
		for (unsigned i = 0; i < kMaxVertices; ++i) {
			r[i] = (float)rand() / RAND_MAX;
		}
	}

	// Evaluate the noise function at position x
	float eval(const float &x)
	{
		int xi = (int)x;
		float t = x - xi;
		int xMin = xi % (int)kMaxVertices; // noise function was  PERIODIC  [value in the range [9:10] is same in [0,1] ,end and begin is same,so the perioic]
		int xMax = (xMin == kMaxVertices - 1) ? 0 : xMin + 1;

		return lerp(r[xMin], r[xMax], t);
	}

	float cosineRemap( const float &x)
	{
		int xi = (int)x;
		int xMin = xi % (int)kMaxVertices;  
		float t = x - xi;
		int xMax = (xMin == kMaxVertices - 1) ? 0 : xMin + 1;

		////////////////////////////////////////////
		assert(t >= 0 && t <= 1);
		float tRemapCosine = (1 - cos(t * math::Const<float>::pi())) * 0.5;
		return lerp(r[xMin], r[xMax], tRemapCosine);
	}

	float smoothstepRemap( const float &x)
	{
		int xi = (int)x;
		int xMin = xi % (int)kMaxVertices;
		float t = x - xi;
		int xMax = (xMin == kMaxVertices - 1) ? 0 : xMin + 1;

		float tRemapSmoothstep = t * t * (3 - 2 * t);
		return lerp(r[xMin], r[xMax], tRemapSmoothstep);
	}

	static const unsigned kMaxVertices = 10;
	float r[kMaxVertices];
};

class ValueNoise1DAlex
{
public:
	ValueNoise1DAlex(unsigned seed = 2011)
	{
		srand(seed);
		for (unsigned i = 0; i < kMaxVertices; ++i) {
			r[i] = (float)rand() / RAND_MAX;
		}
	}

	// Evaluate the noise function at position x
	float eval(const float &x)
	{
		int xi = (int)x - (x < 0 && x != (int)x);
		float t = x - xi;
		// Modulo using &
		int xMin = xi & kMaxVerticesMask;
		int xMax = (xMin + 1) & kMaxVerticesMask;

		return lerp(r[xMin], r[xMax], t);
	}

	float cosineRemap(const float &x)
	{
		int xi = (int)x - (x < 0 && x != (int)x);
		float t = x - xi;
		// Modulo using &
		int xMin = xi & kMaxVerticesMask;
		int xMax = (xMin + 1) & kMaxVerticesMask;

		////////////////////////////////////////////
		assert(t >= 0 && t <= 1);
		float tRemapCosine = (1 - cos(t * math::Const<float>::pi())) * 0.5;
		return lerp(r[xMin], r[xMax], tRemapCosine);
	}

	float smoothstepRemap(const float &x)
	{
		int xi = (int)x - (x < 0 && x != (int)x);
		float t = x - xi;
		// Modulo using &
		int xMin = xi & kMaxVerticesMask;
		int xMax = (xMin + 1) & kMaxVerticesMask;

		float tRemapSmoothstep = t * t * (3 - 2 * t);
		return lerp(r[xMin], r[xMax], tRemapSmoothstep);
	}

	static const unsigned kMaxVertices = 256;
	static const unsigned kMaxVerticesMask = kMaxVertices - 1;
	float r[kMaxVertices];
};

class OneNoiseScene :public Scene
{
public:
	
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);

public:
	void							gridBackGround();
public:
	virtual void					render(PassInfo&);

	std::vector<V3f>				value_NoisePos_;
	std::vector<V3f>				value_Noise_Cirse_Pos_;
	std::vector<V3f>				value_Noise_smooths_Pos_;

	std::vector<V3f>				value_NoisePos_Alex_;
	std::vector<V3f>				value_Noise_Cirse_Pos_Alex_;
	std::vector<V3f>				value_Noise_smooths_Pos_Alex_;
};

bool OneNoiseScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec3 vcolor;"

		"out vec3 ocolor;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view * model * vec4(position, 1.0f);"
		"	gl_PointSize = 3.0;"
		"	ocolor = vcolor;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec3 ocolor;"
		"out vec4 color;"
		"void main()"
		"{"
		"color = vec4(ocolor, 1.0f);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	shaders_.push_back(shader);


	return true;
}

bool OneNoiseScene::initSceneModels(const SceneInitInfo&)
{
	gridBackGround();
	return true;
}

bool OneNoiseScene::initThisDemo(const SceneInitInfo&)
{
	glEnable(GL_PROGRAM_POINT_SIZE);

	ValueNoise1D valueNoise1D;
	int numSteps = 200;

	for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (i / float(numSteps - 1)) * 10.0;
		value_NoisePos_.push_back(V3f(x, valueNoise1D.eval(x), 0.0));
	}

	for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (i / float(numSteps - 1)) * 10.0;
		value_Noise_Cirse_Pos_.push_back(V3f(x, valueNoise1D.cosineRemap(x), 0.0));
	}
	
	for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (i / float(numSteps - 1)) * 10.0;
		value_Noise_smooths_Pos_.push_back(V3f(x, valueNoise1D.smoothstepRemap(x), 0.0));
	}

	/////////////////////
	numSteps = 512;
	ValueNoise1DAlex alex;
	/*for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (i / float(numSteps - 1)) * 256.0;
		value_NoisePos_Alex_.push_back(V3f(x, alex.eval(x), 0.0));
	}

	for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (i / float(numSteps - 1)) * 256.0;
		value_Noise_Cirse_Pos_Alex_.push_back(V3f(x, alex.cosineRemap(x), 0.0));
	}*/

	for (int i = 0; i < numSteps - 1; ++i)
	{
		float x = (2 * (i / float(numSteps - 1)) - 1) * 10;
		value_Noise_smooths_Pos_Alex_.push_back(V3f(x, alex.smoothstepRemap(x), 0.0));
	}
	return true;

}

void OneNoiseScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);

	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_M) //value noise
		{
			for(int i = 0;i < value_NoisePos_.size();i++)
				addRenderNode(getPoints(value_NoisePos_[i], V3f(0.0,1.0,0.0), false));
		}
		if (key == GLU_KEY_N) //value noise   PERIODIC PERIODICPERIODICPERIODICPERIODICPERIODIC
		{
			for (int i = 0; i < value_NoisePos_.size(); i++)
				addRenderNode(getPoints(V3f(value_NoisePos_[i].x -10,value_NoisePos_[i].y,value_NoisePos_[i].z ), V3f(1.0, 0.3, 0.5), false));
			for (int i = 0; i < value_NoisePos_.size(); i++)
				addRenderNode(getPoints(V3f(value_NoisePos_[i].x + 10, value_NoisePos_[i].y, value_NoisePos_[i].z), V3f(1.0, 0.3, 0.5), false));
		}

		if (key == GLU_KEY_O)
		{
			for (int i = 0; i < value_Noise_smooths_Pos_.size(); i++)
				addRenderNode(getPoints(value_Noise_Cirse_Pos_[i], V3f(1.0, 1.0, 0.0), false));
		}
		if (key == GLU_KEY_P)
		{
			for (int i = 0; i < value_Noise_smooths_Pos_.size(); i++)
				addRenderNode(getPoints(value_Noise_smooths_Pos_[i], V3f(1.0, 1.0, 1.0), false));
		}

		////////////////alex
		if (key == GLU_KEY_Q)
		{
			for (int i = 0; i < value_Noise_smooths_Pos_Alex_.size(); i++)
				addRenderNode(getPoints(value_Noise_smooths_Pos_Alex_[i], V3f(0.0, 0.0, 0.0), false));
		}
	}
}

void OneNoiseScene::gridBackGround()
{
	float x = 15.0;
	float y = 15.0;

	int splidernum = 15;
	float ystep = y / splidernum;
	float xstep = x / splidernum;

	V3f color(1.0, 0.5, 0.5);
	for (int i = -splidernum; i < splidernum; i++) //x
	{
		V3f s(i * xstep, y, 0.0);
		V3f e(i * xstep, -y, 0.0);
		addRenderNode(getLine(s, e, color, false));
	}

	for (int j = -splidernum; j < splidernum; j++) //y
	{
		V3f s(-x, j * ystep, 0.0);
		V3f e(x, j * ystep, 0.0);
		addRenderNode(getLine(s, e, color, false));
	}

}

void OneNoiseScene::render(PassInfo&ps)
{
	Shader * shader = shaders_[0];
	ps.tranform_ = math::translateR(ps.tranform_, V3f(1.0, 0.0, 0.0));
	shader->turnOn();
	initUniformVal(shader);
	for (int i = 0; i < renderNodeNum(); i++)
	{
		getRenderNode(i)->render(shader, ps);
	}
	shader->turnOff();
	CHECK_GL_ERROR;
}

int main(int argc, char **argv)
{
	
	OneNoiseScene * scene = new OneNoiseScene;

	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "One dis Noise test";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();


	return 0;
}

