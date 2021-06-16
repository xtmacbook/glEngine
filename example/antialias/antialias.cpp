
#include "scene.h"
#include "antialias.h"
#include "shader.h"
#include "resource.h"

#include "gLFWManager.h"
#include "gLApplication.h"
#include "camera.h"
#include "renderNode.h"

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

	std::string vs = get_shader_BasePath() + "basic.vs";
	std::string fs = get_shader_BasePath() + "basic.frag";
	currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr);

	shaders_.push_back(new Shader);
	vs = get_shader_BasePath() + "screenQuad.vs";
	fs = get_shader_BasePath() + "screenQuad.frag";
	currentShader = shaders_[1];
	currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr);
	int eunit = currentShader->getVariable("screenTexture");
	glUniform1i(eunit, 0);


	shaders_.push_back(new Shader);
	vs = get_shader_BasePath() + "screenQuad.vs";
	fs = get_shader_BasePath() + "multTexture.frag";
	currentShader = shaders_[2];
	currentShader->loadShaders(vs.c_str(), fs.c_str(), nullptr);
	glUniform1d(currentShader->getVariable("screenTextureMS"), 0);
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

	GLFWManager *pWindowManager = new GLFWManager();

	GLApplication application(scene);

	application.setWindowManager(pWindowManager);

	application.loop();

	return true;
}
