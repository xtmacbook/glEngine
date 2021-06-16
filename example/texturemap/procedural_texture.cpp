
#include <windowManager.h>
#include <gLApplication.h>
#include <camera.h>
#include <scene.h>
#include <shader.h>
#include <resource.h>
#include <log.h>
#include <engineLoad.h>
#include <image.h>
#include <helpF.h>
#include <callback.h>
#include <Inputmanager.h>
#include <renderNode.h>
#include <util.h>
#include <light.h>
#include <glinter.h>

class ProduceDuralScene :public Scene
{
public:
	
	virtual ~ProduceDuralScene() {}
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);
	virtual bool					initTexture(const SceneInitInfo&);

public:
	virtual void					render(PassInfo&);

	Shader*							render_shader_ = nullptr;
private:
};

bool ProduceDuralScene::initShader(const SceneInitInfo&)
{
	std::string code = Shader::loadMultShaderInOneFile("texture/producedural_texture.glsl");
	
	Shader * s1 = new Shader;
	s1->getShaderFromMultCode(Shader::VERTEX, "ver", code);
	s1->getShaderFromMultCode(Shader::FRAGMENT, "1", code);
	s1->linkProgram();
	s1->checkProgram();
	shaders_.push_back(s1);


	Shader * s2 = new Shader;
	s2->getShaderFromMultCode(Shader::VERTEX, "ver", code);
	s2->getShaderFromMultCode(Shader::FRAGMENT, "2", code);
	s2->linkProgram();
	s2->checkProgram();

	shaders_.push_back(s2);

	render_shader_ = shaders_[0];
	return true;
}

bool ProduceDuralScene::initSceneModels(const SceneInitInfo&)
{
	return true;
}

bool ProduceDuralScene::initThisDemo(const SceneInitInfo&)
{
	base::SmartPointer<Quad> quad = new Quad;
	quad->initGeometry();
	RenderNode_SP quadRenderNode = new RenderNode;
	quadRenderNode->setGeometry(quad);
	addRenderNode(quadRenderNode);

	return true;
}

void ProduceDuralScene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);

	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_1 )
		{
			render_shader_ = shaders_[0];
		}
		if (key == GLU_KEY_2)
		{
			render_shader_ = shaders_[1];
		}
	}
}

bool ProduceDuralScene::initTexture(const SceneInitInfo&)
{
	std::string tx = get_texture_BasePath() + "diffuse.jpg";
	Texture* curTexObj = new Texture(tx.c_str());
	curTexObj->target_ = GL_TEXTURE_CUBE_MAP;
	if (curTexObj->loadData())
	{
		curTexObj->createObj();
		curTexObj->bind();
		curTexObj->mirrorRepeat();
		curTexObj->filterLinear();
		if (curTexObj->context(NULL))
		{
			curTexObj->unBind();
			texturesObj_.push_back(curTexObj);
		}
		else return false;
		CHECK_GL_ERROR;
	}
}

void ProduceDuralScene::render(PassInfo&info)
{
	render_shader_->turnOn();
	initUniformVal(render_shader_);
	getRenderNode(0)->render(render_shader_, info);

}

int main()
{

	ProduceDuralScene* scene = new ProduceDuralScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;

	wc.title_ = "produceDural Texture";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	application.initialize(&wc, &dc);
	glDisable(GL_CULL_FACE);

	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
