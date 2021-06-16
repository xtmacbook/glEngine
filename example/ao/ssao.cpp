
/**
 * @ 17/01/2020
   @ xian
   @ xt
 */

 //screen-space ambient occlusion

 /**
  * for each fragment on a screenfilled quad we calculate an occlusion factor based on the fragment¡¯s
	 surrounding depth values.
  */

#include <scene.h>
#include <gbuffer.h>
#include <shader.h>
#include <log.h>
#include <camera.h>
#include <gLApplication.h>
#include <renderNode.h>
#include <engineLoad.h>
#include <light.h>
#include <helpF.h>
#include <resource.h>
#include <windowManager.h>
#include <random>
#include <algorithm>
#include <glinter.h>

std::vector<glm::vec3> ssaoKernel;

PointLightLocation plocation;

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}


class LocalRenderMeshObj :public RenderMeshObj
{
public:

	LocalRenderMeshObj(RenderNode_SP& rs) :rs_(rs) {}

	virtual void preRender(Shader*);

	std::vector<unsigned int > t_indices_;

	RenderNode_SP rs_;
};

void LocalRenderMeshObj::preRender(Shader* shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (unsigned int i = 0; i < t_indices_.size(); i++)
	{
		std::string number;
		std::string name;
		Texture_Sp texture = rs_->textures_[t_indices_[0]];
		TEXTURE_TYPE tt = texture->t_type_;
		switch (tt)
		{
		case T_DIFFUSE:
			name = "texture_diffuse";
			number = std::to_string(diffuseNr++);
			break;
		case T_SPECULAR:
			name = "texture_specular";
			number = std::to_string(specularNr++); // transfer unsigned int to stream
			break;
		case T_NORMAL:
			name = "texture_normal";
			number = std::to_string(normalNr++); // transfer unsigned int to stream
			break;
		case T_HEIGHT:
			name = "texture_height";
			number = std::to_string(heightNr++); // transfer unsigned int to stream
			break;
		case T_NONE:
			break;
		default:
			break;
		}
		GLint local = shader->getVariable((name + number).c_str(), false);
		if (local != -1)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			shader->setInt(local, i);
			texture->bind();
		}
	}
}

class LocalMeshGeometry :public MeshGeometry
{
public:

	LocalMeshGeometry(RenderNode_SP&r) :rn_(r)
	{

	}

	virtual IRenderMeshObj_SP createRenderMeshObj();
	virtual void setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj);

	RenderNode_SP rn_;
};

IRenderMeshObj_SP LocalMeshGeometry::createRenderMeshObj()
{
	return new LocalRenderMeshObj(rn_);
}

void LocalMeshGeometry::setupMesh(const Mesh_SP mesh, IRenderMeshObj_SP& obj)
{
	MeshGeometry::setupMesh(mesh, obj);

	TMesh * tmesh = dynamic_cast<TMesh*>((Mesh*)mesh);
	if (tmesh)
	{
		LocalRenderMeshObj * lrmso = dynamic_cast<LocalRenderMeshObj*>((IRenderMeshObj*)obj);

		if (lrmso)
		{
			lrmso->t_indices_ = tmesh->t_indices_;
		}
	}
}


class SSAOScene :public Scene
{
	virtual ~SSAOScene();
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initKeyCallback();
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initLight(const SceneInitInfo&);
public:
	virtual void					render(PassInfo&);

	base::SmartPointer<GBuffer>		gbuffer_;
	base::SmartPointer<GBuffer>		ssaoFBO_;
	base::SmartPointer<GBuffer>		ssaoBlurFBO_;

	Texture_Sp						noiseText_;
};

SSAOScene::~SSAOScene()
{

}

bool SSAOScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader("ao/ssao_geometry.vs", "ao/ssao_geometry.fs")); CHECK_GL_ERROR;
	shaders_.push_back(new Shader("ao/ssao.vs", "ao/ssao_lighting.fs")); CHECK_GL_ERROR;
	shaders_.push_back(new Shader("ao/ssao.vs", "ao/ssao.fs")); CHECK_GL_ERROR;
	shaders_.push_back(new Shader("ao/ssao.vs", "ao/ssao_blur.fs")); CHECK_GL_ERROR;
	shaders_.push_back(new Shader("ao/basic.vert", "ao/basic.frag")); CHECK_GL_ERROR;

	Shader&shaderLightingPass = *shaders_[1];
	shaderLightingPass.turnOn();
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gPosition"), 0);
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gNormal"), 1);
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gAlbedo"), 2);
	shaderLightingPass.setInt(shaderLightingPass.getVariable("ssao"), 3);
	getPointLightShaderLocation(0, &shaderLightingPass, plocation);

	Shader&shaderSSAO = *shaders_[2];
	shaderSSAO.turnOn();
	shaderSSAO.setInt(shaderSSAO.getVariable("gPosition"), 0);
	shaderSSAO.setInt(shaderSSAO.getVariable("gNormal"), 1);
	shaderSSAO.setInt(shaderSSAO.getVariable("texNoise"), 2);

	Shader&shaderSSAOBlur = *shaders_[3];
	shaderSSAOBlur.turnOn();
	shaderSSAOBlur.setInt(shaderSSAOBlur.getVariable("ssaoInput"), 0);

	return true;
}

bool SSAOScene::initSceneModels(const SceneInitInfo&)
{
	IO::LModelInfo data;
	if (IO::EngineLoad::loadNode((get_model_BasePath() + "/nanosuit/nanosuit.obj").c_str(), data))
	{
		if (!data.meshs_.empty())
		{
			RenderNode_SP rn = new RenderNode;
			base::SmartPointer<LocalMeshGeometry> mg = new LocalMeshGeometry(rn);
			mg->meshs_ = data.meshs_;
			mg->initGeometry();
			if (rn)
			{
				rn->setGeometry(mg);
				rn->textures_ = data.textures_;
				rn->loadTextures();
			}
			addRenderNode(rn);
		}
	}
	else { return false; }
	//cube
	base::SmartPointer<Cub> cub = new Cub;
	cub->initGeometry();
	RenderNode_SP cubRN(new RenderNode);
	cubRN->setGeometry(cub);
	addRenderNode(cubRN);

	//quad
	base::SmartPointer<Quad> quad = new Quad;
	quad->initGeometry();
	RenderNode_SP quadRenderNode = new RenderNode;
	quadRenderNode->setGeometry(quad);
	addRenderNode(quadRenderNode);

	return true;
}

bool SSAOScene::initTexture(const SceneInitInfo&)
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	noiseText_ = new Texture;
	noiseText_->createObj();
	noiseText_->bind();
	noiseText_->width_ = noiseText_->height_ = 4;
	noiseText_->internalformat_ = GL_RGB32F;
	noiseText_->type_ = GL_FLOAT;
	noiseText_->format_ = GL_RGB;
	noiseText_->eMinFilter = noiseText_->eMagFilter = GL_NEAREST;
	noiseText_->iWrapR = noiseText_->iWrapS = noiseText_->iWrapT = GL_REPEAT;
	noiseText_->initGLObj();
	noiseText_->context(&ssaoNoise[0]);
	return true;
}

bool SSAOScene::initKeyCallback()
{
	return true;
}

bool SSAOScene::initThisDemo(const SceneInitInfo&flag)
{
	gbuffer_ = new GBuffer(flag.w_, flag.h_);
	if (gbuffer_)
	{
		gbuffer_->initBuffer();
		gbuffer_->createGLObj();

		gbuffer_->position_->texture_->eMinFilter = gbuffer_->position_->texture_->eMagFilter = GL_NEAREST;
		gbuffer_->normal_->texture_->eMinFilter = gbuffer_->normal_->texture_->eMagFilter = GL_NEAREST;
		gbuffer_->specular_->texture_->eMinFilter = gbuffer_->specular_->texture_->eMagFilter = GL_NEAREST;

		gbuffer_->position_->texture_->iWrapR =
			gbuffer_->position_->texture_->iWrapS =
			gbuffer_->position_->texture_->iWrapT = GL_CLAMP_TO_EDGE;
		gbuffer_->normal_->texture_->iWrapR = gbuffer_->normal_->texture_->iWrapS = gbuffer_->normal_->texture_->iWrapT = GL_REPEAT;
		gbuffer_->specular_->texture_->iWrapR = gbuffer_->specular_->texture_->iWrapS = gbuffer_->specular_->texture_->iWrapT = GL_REPEAT;

		gbuffer_->specular_->texture_->format_ = gbuffer_->specular_->texture_->internalformat_ = GL_RGB;
		if (!gbuffer_->initGLObj()) return false;
		gbuffer_->bindGLObj();
		gbuffer_->drawToBuffer();
		gbuffer_->unBindGLObj();
	}
	CHECK_GL_ERROR;

	ssaoFBO_ = new GBuffer(flag.w_, flag.h_, GB_POSITION);
	if (ssaoFBO_)
	{
		ssaoFBO_->initBuffer();
		ssaoFBO_->createGLObj();
		ssaoFBO_->position_->texture_->eMinFilter = ssaoFBO_->position_->texture_->eMagFilter = GL_NEAREST;
		ssaoFBO_->position_->texture_->iWrapR = ssaoFBO_->position_->texture_->iWrapS = ssaoFBO_->position_->texture_->iWrapT = GL_REPEAT;
		ssaoFBO_->position_->texture_->internalformat_ = GL_RED;
		ssaoFBO_->position_->texture_->format_ = GL_RGB;
		if (!ssaoFBO_->initGLObj()) return false;

	}
	CHECK_GL_ERROR;

	ssaoBlurFBO_ = new GBuffer(flag.w_, flag.h_, GB_POSITION);
	if (ssaoBlurFBO_)
	{
		ssaoBlurFBO_->initBuffer();
		ssaoBlurFBO_->createGLObj();
		ssaoBlurFBO_->position_->texture_->eMinFilter = ssaoBlurFBO_->position_->texture_->eMagFilter = GL_NEAREST;
		ssaoBlurFBO_->position_->texture_->iWrapR = ssaoBlurFBO_->position_->texture_->iWrapS = ssaoBlurFBO_->position_->texture_->iWrapT = GL_REPEAT;
		ssaoBlurFBO_->position_->texture_->internalformat_ = GL_RED;
		ssaoBlurFBO_->position_->texture_->format_ = GL_RGB;
		if (!ssaoBlurFBO_->initGLObj()) return false;
	}

	CHECK_GL_ERROR;

	return true;
}

bool SSAOScene::initLight(const SceneInitInfo&)
{
	PointLight* light = new PointLight;
	light->position_ = V3f(2.0, 4.0, -2.0);
	light->color_ = V3f(0.2, 0.2, 0.7);
	light->atten_.linear_ = 0.09;
	light->atten_.quad_ = 0.032;
	addLight(light);
	return true;
}

void SSAOScene::render(PassInfo&ps)
{		
	//for test
	if(0)
	{
		Shader * curs = shaders_[4];
		curs->turnOn();
		initUniformVal(curs);
		ps.tranform_ = glm::mat4(1.0);
		ps.tranform_ = glm::translate(ps.tranform_, glm::vec3(0.0, 7.0f, 0.0f));
		ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(7.5f, 7.5f, 7.5f));
		getRenderNode(1)->render(curs, ps);
		ps.tranform_ = glm::mat4(1.0);
		ps.tranform_ = glm::translate(ps.tranform_, glm::vec3(0.0f, 0.0f, 5.0));
		ps.tranform_ = glm::rotate(ps.tranform_, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(0.5f));
		getRenderNode(0)->render(curs, ps);
		return;
	}

	Shader * curs = shaders_[0];
	curs->turnOn();
	gbuffer_->bindGLObj();
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		initUniformVal(curs);
		GLint loc = curs->getVariable("invertedNormals");
		if (loc != -1)
		{
			curs->setInt(loc, 1);
			ps.tranform_ = glm::mat4(1.0);
			ps.tranform_ = glm::translate(ps.tranform_, glm::vec3(0.0, 7.0f, 0.0f));
			ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(7.5f, 7.5f, 7.5f));
			getRenderNode(1)->render(curs, ps);
			curs->setInt(loc, 0);
			ps.tranform_ = glm::mat4(1.0);
			ps.tranform_ = glm::translate(ps.tranform_, glm::vec3(0.0f, 0.0f, 5.0));
			ps.tranform_ = glm::rotate(ps.tranform_, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(0.5f));
			getRenderNode(0)->render(curs, ps);
		}
	}
	CHECK_GL_ERROR;

	ssaoFBO_->bindGLObj();
	{
		glClear(GL_COLOR_BUFFER_BIT);
		curs = shaders_[2];
		curs->turnOn();
		for (unsigned int i = 0; i < 64; ++i)
		{
			curs->setFloat3V ( curs->getVariable(("samples[" + std::to_string(i) + "]").c_str()),1, math::value_ptr(ssaoKernel[i]));
		}
		curs->setMatrix4(curs->getVariable("projection"),1,false, math::value_ptr(getCamera()->getProjectionMatrix()));
		glActiveTexture(GL_TEXTURE0);
		gbuffer_->position_->bind();
		CHECK_GL_ERROR;

		glActiveTexture(GL_TEXTURE1);
		gbuffer_->normal_->bind();
		CHECK_GL_ERROR;

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseText_->getTexture());
		getRenderNode(2)->render(curs, ps);
	}
	CHECK_GL_ERROR;

	ssaoBlurFBO_->bindGLObj();
	{
		curs = shaders_[3];
		curs->turnOn();
		glClear(GL_COLOR_BUFFER_BIT);
		curs->turnOn();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoFBO_->position_->getTexture());
		getRenderNode(2)->render(curs, ps);
	}
	CHECK_GL_ERROR;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	curs = shaders_[1];
	curs->turnOn();
	
	applyPointLight(*(PointLight*)(plights_[0].addr()), curs, plocation);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbuffer_->position_->getTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gbuffer_->normal_->getTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gbuffer_->specular_->getTexture());
	glActiveTexture(GL_TEXTURE3); 
	glBindTexture(GL_TEXTURE_2D, ssaoBlurFBO_->position_->getTexture());
	getRenderNode(2)->render(curs, ps);
	curs->turnOff();
	CHECK_GL_ERROR;

	Scene::render(ps);
}

int main()
{
	 
	SSAOScene* scene = new SSAOScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "ssao";
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
