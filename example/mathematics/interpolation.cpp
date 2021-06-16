

#include "scene.h"
#include "RenderNode.h"
#include "shader.h"
#include "engineLoad.h"
#include "resource.h"
#include "gLApplication.h"
#include "camera.h"
#include "windowManager.h"
#include "log.h"
#include "helpF.h"
#include <baseMesh.h>
#include <Inputmanager.h>
#include "glinter.h"
#include <cmath>

/**
 * /detail :Mathematics Graphics
	Linear Interpolation:
		 What is assured by this interpolant, is that equal steps in t result in EQUAL
		steps in x, y, and z. Illustrates this linear spacing with a 2D example where
		we interpolate between the points (1,1) and (4,5). Note the equal spacing between
		the intermediate interpolated points
	Non-Linear Interpolation:
		required that equal steps in t give rise to unequal steps in the interpolated values.
 */


class InterScene :public Scene
{
public:

	struct VecPoint
	{
		V3f Point_;
		V3f vec_;
	};

protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initKeyCallback();

public:
	void							gridBackGround();
	void							iterpolation(const V3f&point, const V3f&color);
	void							iterpolationVec(const V3f&s,const V3f&dir, const V3f&color);
	void							clear();
	V3f								line_iterpolation_f(const V3f&start, const V3f&end, float t);
	V3f								trigonometric_iterpolation_f(const V3f&start, const V3f&end, float t);
	V3f								cubic_iterpolation_f(const V3f&start, const V3f&end, float t);
	VecPoint						hermite_interpolation_f(const VecPoint&start, const VecPoint&end, float t);
									//iterploation vector
	V3f								iterpolation_vec_f(const V3f&startv, const V3f&endv, float t);
public:
	virtual void					render(PassInfo&);
};

bool InterScene::initShader(const SceneInitInfo&)
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
		"	gl_PointSize = 10.0;"
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

bool InterScene::initSceneModels(const SceneInitInfo&)
{
	gridBackGround();
	return true;
}

bool InterScene::initThisDemo(const SceneInitInfo&)
{
	glEnable(GL_PROGRAM_POINT_SIZE);
	return true;
}
InterScene * g_scene;

bool keyEvent(int code)
{
	V3f start(0.0, 0.0, 0.0);
	V3f end(5.0, 5.0, 0.0);


	InterScene::VecPoint sv, ev;
	sv.Point_ = start;
	sv.vec_ = V3f(-5.0, 0.0, 0.0);
	ev.Point_ = V3f(1.0, 1.0, 0.0);
	ev.vec_ = V3f(0.0, -5.0, 0.0);

	if (code == GLU_KEY_N)
	{
		static float t = 0.0;
		t += 0.1;
		if (t > 1.0) return false;
		V3f point = g_scene->line_iterpolation_f(start, end, t);
		g_scene->iterpolation(point,V3f(1.0,0.0,0.0));
	}
	if (code == GLU_KEY_O)
	{
		static float t = 0.0;
		t += 0.1;
		if (cos(t) * cos(t) > 1.0) return false;
		V3f point = g_scene->trigonometric_iterpolation_f(start, end, t);
		g_scene->iterpolation(point, V3f(1.0, 0.0, 1.0));
	}
	if (code == GLU_KEY_P)
	{
		static float t = 0.0;
		t += 0.1;
		if (t > 1.0) return false;
		V3f point = g_scene->cubic_iterpolation_f(start, end, t);
		g_scene->iterpolation(point, V3f(0.0, 0.0, 1.0));
	}
	if (code == GLU_KEY_Q)
	{
		static float t = 0.0;
		t += 0.05;
		if (t > 1.0) return false;
		InterScene::VecPoint vapoint = g_scene->hermite_interpolation_f(sv, ev, t);
		g_scene->iterpolation(vapoint.Point_,V3f(0.5,0.2,0.3));
	}
	if (code == GLU_KEY_R)
	{
		static float t = 0.0;

		V3f startv(1, 0, 0);
		V3f endv(1.0, 1.0, 1.0);
		math::normalizeVec3(startv);
		math::normalizeVec3(endv);
		if (t == 0.0)
		{
			g_scene->iterpolationVec(V3f(0.0, 0.0, 0.0), start, V3f(0.0, 0.6, 0.4));
			g_scene->iterpolationVec(V3f(0.0, 0.0, 0.0), endv, V3f(0.0, 0.6, 0.4));
		}

		t += 0.05;
		if (t > 1.0) return false;

		V3f vr = g_scene->iterpolation_vec_f (startv, endv, t);
		g_scene->iterpolationVec(start,vr,V3f(0.2,0.6,0.4));
	}
	if (code == GLU_KEY_C)
	{
		g_scene->clear();
	}

	return true;
}


bool InterScene::initKeyCallback( )
{
	g_scene = this;
	Scene::initKeyCallback(); //GLFW_KEY_N
	KEY_CALLBACK(78, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_N
	KEY_CALLBACK(79, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_O
	KEY_CALLBACK(80, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_P
	KEY_CALLBACK(81, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_Q
	KEY_CALLBACK(82, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_R
	KEY_CALLBACK(67, base::NewPermanentCallback(keyEvent)); //GLFW_KEY_C
	return true;
}


void InterScene::gridBackGround()
{
	float x = 5.0;
	float y = 5.0;

	unsigned int splidernum = 25;
	float ystep = y / splidernum;
	float xstep = x / splidernum;

	V3f color(1.0, 0.5, 0.5);
	for (int i = 0; i < splidernum; i++) //x
	{
		V3f s(i * xstep, y, 0.0);
		V3f e(i * xstep, 0.0, 0.0);
		addRenderNode(getLine(s, e, color,false));
	}

	for (int j = 0; j < splidernum; j++) //y
	{
		V3f s(0.0, j * ystep, 0.0);
		V3f e(x, j * ystep, 0.0);
		addRenderNode(getLine(s, e, color, false));
	}

}

void InterScene::iterpolation(const V3f&point,const V3f&color)
{
	addRenderNode(getPoints(point, color, false));
}

void InterScene::iterpolationVec(const V3f&s, const V3f&dir, const V3f&color)
{
	V3f normalDir = dir;
	math::normalizeVec3(normalDir);
	V3f ep = s + normalDir;
	addRenderNode(getRay(s, ep,color, false));
}

void InterScene::clear()
{
	clearRenderNode();
}

math::V3f InterScene::line_iterpolation_f(const V3f&start, const V3f&end, float t)
{
	//x = x1 * (1-t) + x2 * t;
	V3f n = start * t + end * (1 - t);
	return n;
}

math::V3f InterScene::trigonometric_iterpolation_f(const V3f&start, const V3f&end, float t)
{
	// 0 << t << pi / 2
	//n = n1 * cost * cost + n2 * sint * sint; (where cost * cost + sint * sint = 1)
	V3f n = start * cos(t) * cos(t) + end * sin(t) * sin(t);
	return n;
}

math::V3f InterScene::cubic_iterpolation_f(const V3f&start, const V3f&end, float t)
{
	/**

	for sinusoid that impossible to change the nature of the curve,so One way of gaining control over the interpolated curve is to use a polynomial

	 *  v1 = a * t * t * t + b * t * t + c * t + d
			
					[n1]
		n = [v1 v2]      
					[n2]

		v1 = 2 * t * t * t -3 * t * t + 1
		v2 = -2 * t * t * t + 3 * t * t 
		详见:mathematics Graphics Interpolation
	 */

	float sqrtt = t * t;
	float cubict = sqrtt * t;

	V3f n = start * (2 * cubict - 3 * sqrtt + 1) + end * ((-2) * cubict + 3 * sqrtt);

	return n;
}

InterScene::VecPoint InterScene::hermite_interpolation_f(const VecPoint&start, const VecPoint&end, float t)
{
	//详见:mathematics Graphics Interpolation

	float sqrtt = t * t;
	float cubitt = sqrtt * t;

	glm::mat4 der(2.0f,-2.0f,1.0f,1.0f,
				-3.0f,3.0f,-2.0f,-1.0f,
				0.0f,0.0f,1.0f,0.0f,
				1.0f,0.0f,0.0f,0.0f);

	glm::vec4  tder(cubitt,sqrtt,t,1);
	glm::vec4 xv(start.Point_.x, end.Point_.x, start.vec_.x, end.vec_.x);
	glm::vec4 yv(start.Point_.y, end.Point_.y, start.vec_.y, end.vec_.y);
	glm::vec4 zv(start.Point_.z, end.Point_.z, start.vec_.z, end.vec_.z);

	VecPoint reval;

	glm::vec4 tr = der * tder;
	reval.Point_.x = tr.x * xv.x + tr.y * xv.y + tr.z * xv.z + tr.w * xv.w;
	reval.Point_.y = tr.x * yv.x + tr.y * yv.y + tr.z * yv.z + tr.w * yv.w;
	reval.Point_.z = tr.x * zv.x + tr.y * zv.y + tr.z * zv.z + tr.w * zv.w;

	return reval;
}

math::V3f InterScene::iterpolation_vec_f(const V3f&startv, const V3f&endv, float t)
{
	float dotv = math::dotVec3(startv, endv);
	float radians = math::acosR(dotv);
	float sinr = math::sinR(radians);

	float tr =  math::sinR((1 - t) * radians);
	float tr1 = math::sinR(t * radians);

	tr /= sinr;
	tr1 /= sinr;

	V3f rv;
	rv = tr * startv + tr1 * endv;

	return rv;
}

void InterScene::render(PassInfo&ps)

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

int main()
{
	InterScene * scene = new InterScene;

	Camera* pCamera = new Camera(Camera::OBJECT | Camera::ALL | Camera::PAN_RELATIVE);
	Matrixf tr (1.0f);
	tr = translateR(tr, V3f(3, 0, 0));
	tr = rotateR(tr, (float)PI_2 / 2.0f, V3f(1.0, 0.0, 1.0));
	pCamera->setObject(tr);
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "Interpolation Test";
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
