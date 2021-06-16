
#include "scene.h"
#include "antialias.h"
#include "shader.h"
#include "resource.h"

#include "windowManager.h"
#include "gLApplication.h"
#include "camera.h"
#include "renderNode.h"
#include "glinter.h"


/**
 * note:
  rasteriztion:
	The rasterization pipeline on a GPU takes as input the vertices of the primitive being rendered.The vertex position are used to determine the set of pixels in the current render target where the triangle will be visible. This visible set is determined from two things: coverage, and occlusion. Coverage is determined by performing some test to determine if the primitive overlaps a given pixel. In GPU¡¯s, coverage is calculated by testing if the primitive overlaps a single sample point located in the exact center of each pixel.Occlusion tells us whether a pixel covered by a primitive is also covered by any other triangles, and is handled by z-buffering in GPU¡¯s.
  MSAA:
	In terms of rasterization, MSAA works in a similar manner to supersampling. The coverage and occlusion tests are both performed at higher-than-normal resolution, which is typically 2x through 8x. For coverage, the hardware implements this by having N sample points within a pixel, where N is the multisample rate. These samples are known as subsamples, since they are sub-pixel samples. 

 Depth buffer os MSAA:
	The triangle is tested for coverage at each of the N sample points, essentially building a bitwise coverage mask representing the portion of the pixel covered by a triangle 2. For occlusion testing, the triangle depth is interpolated at each covered sample point and tested against the depth value in the z buffer. Since the depth test is performed for each subsample and not for each pixel, the size of the depth buffer must be augmented to store the additional depth values. In practice this means that the depth buffer will N times the size of the non-MSAA case. So for 2xMSAA the depth buffer will be twice the size, for 4x it will be four times the size, and so on.
	
 MSAA different with supersampling :
	Where MSAA begins to differ from supersampling is when the pixel shader is executed. In the standard MSAA case, the pixel shader is not executed for each subsample. Instead, the pixel shader is executed only once for each pixel where the triangle covers at least one subsample. Or in other words, it is executed once for each pixel where the coverage mask is non-zero. At this point pixel shading occurs in the same manner as non-MSAA rendering: the vertex attributes are interpolated to the center of the pixel and used by the pixel shader to fetch textures and perform lighting calculations. This means that the pixel shader cost does not increase substantially when MSAA is enabled, which is the primary benefit of MSAA over supersampling.

 web: https://docs.microsoft.com/zh-cn/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules?redirectedfrom=MSDN
	  https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/
*/

//0:normal
//1:framebuffer
//2:custom
GLuint g_state;

render::Antialias antialias(render::Antialias::FRAMEBUF);

render::Antialias antialias_texture(render::Antialias::TEXTURE);

class LocalScene :public Scene
{
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
public:
	virtual void					render(PassInfo&);
};

bool LocalScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);
	Shader * currentShader = shaders_[0];

	std::string vs = get_shader_BasePath() + "basic.vert";
	std::string fs = get_shader_BasePath() + "basic.frag";
	if (currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr) == -1) return false;

	shaders_.push_back(new Shader);
	vs = "screenQuad.vert";
	fs = "screenQuad.frag";
	currentShader = shaders_[1];
	if (currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr) == -1) return false;
	currentShader->turnOn();
	int eunit = currentShader->getVariable("screenTexture");
	glUniform1i(eunit, 0);

	shaders_.push_back(new Shader);
	vs = "screenQuad.vert";
	fs = "multTexture.frag";
	currentShader = shaders_[2];
	if (currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr) == -1) return false;
	currentShader->turnOn();
	glUniform1d(currentShader->getVariable("screenTextureMS"), 0);
	currentShader->turnOff();
	return true;
}

bool LocalScene::initSceneModels(const SceneInitInfo&)
{

	RenderNode_SP cubNode(new RenderNode);
	CommonGeometry_Sp cub = new Cub;
	cub->initGeometry();
	cubNode->setGeometry(cub);

	RenderNode_SP quadNode(new RenderNode);
	CommonGeometry_Sp qg = new Quad;
	qg->initGeometry();
	quadNode->setGeometry(qg);

	addRenderNode(cubNode);
	addRenderNode(quadNode);

	antialias.init();
	antialias_texture.init();

	return true;
}

bool LocalScene::initTexture(const SceneInitInfo&)
{
	return true;

}

void LocalScene::render(PassInfo& psi)
{
	if (g_state == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Shader * currentShader = shaders_[0];
		currentShader->turnOn();
		initUniformVal(currentShader);
		getRenderNode(0)->render(currentShader, psi);
		currentShader->turnOff();
	}

	else if (g_state == 2)
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		antialias.bindMultSampleFrameBuffer();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		Shader * currentShader = shaders_[0];
		currentShader->turnOn();
		initUniformVal(currentShader);

		getRenderNode(0)->render(currentShader, psi);

		antialias.blitFrameBuffer();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		currentShader = shaders_[1];
		currentShader->turnOn();
		getRenderNode(1)->bindVAO();
		glActiveTexture(GL_TEXTURE0);
		antialias.bindTexture();

		getRenderNode(1)->render(currentShader,psi);

	}

	else if (g_state == 1)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		antialias_texture.bindMultSampleFrameBuffer();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		Shader * currentShader = shaders_[0];
		currentShader->turnOn();
		initUniformVal(currentShader);

		getRenderNode(0)->render(currentShader,psi);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		currentShader = shaders_[2];
		currentShader->turnOn();
		getRenderNode(1)->bindVAO();
		glActiveTexture(GL_TEXTURE0);
		antialias_texture.bindMultSampleTexture();
		getRenderNode(1)->render(currentShader, psi);
		currentShader->turnOff();
	}
}

int main()
{
	LocalScene * scene = new LocalScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);


	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "msaa";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return true;
}
