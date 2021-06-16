
#include <scene.h>
#include <gLApplication.h>
#include <camera.h>
#include <shader.h>
#include <resource.h>
#include <log.h>
#include <gbuffer.h>
#include <renderNode.h>
#include <engineLoad.h>
#include <algorithm>
#include <light.h>
#include <glinter.h>
#include <windowManager.h>

const unsigned int NR_LIGHTS = 32;
std::vector<glm::vec3> objectPositions;

PointLightLocation locaton[NR_LIGHTS];

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
			number = std::to_string(normalNr++); // transfe++99/r unsigned int to stream
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

class DeferredScene : public Scene
{
	virtual ~DeferredScene();
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					initUniformVal(Shader*shader, bool modelNeed = true);
	virtual bool					initLight(const SceneInitInfo&);

public:
	virtual void					render(PassInfo&);

	base::SmartPointer<GBuffer>		gbuffer_;
};

DeferredScene::~DeferredScene()
{
	if (gbuffer_)
		gbuffer_->delRef();
}

bool DeferredScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader("./basic.vert", "./basic.frag")); //first shader for test
	shaders_.push_back(new Shader("./light/g_buffer.vert", "./light/g_buffer.frag"));
	shaders_.push_back(new Shader("./light/deferred_shading.vert", "./light/deferred_shading.frag"));
	shaders_.push_back(new Shader("./light/deferred_light_box.vert", "./light/deferred_light_box.frag"));

	Shader & shaderLightingPass = *shaders_[2];
	shaderLightingPass.turnOn();
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gPosition"), 0);
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gNormal"), 1);
	shaderLightingPass.setInt(shaderLightingPass.getVariable("gAlbedoSpec"), 2);
	
	for (unsigned int i = 0; i < plights_.size(); i++)
	{
		getPointLightShaderLocation(i, &shaderLightingPass, locaton[i]);
	}
	shaderLightingPass.turnOff();
	return true;
}

bool DeferredScene::initSceneModels(const SceneInitInfo&)
{
	IO::LModelInfo data;
	std::string modelPath = get_model_BasePath() + "nanosuit/nanosuit.obj";
	if (IO::EngineLoad::loadNode(modelPath.c_str(), data))
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

			//quad
			base::SmartPointer<Quad> quad = new Quad;
			quad->initGeometry();
			RenderNode_SP quadRenderNode = new RenderNode;
			quadRenderNode->setGeometry(quad);
			addRenderNode(quadRenderNode);

			//cub
			base::SmartPointer<Cub> cub = new Cub;
			cub->initGeometry();
			RenderNode_SP cubRenderNode = new RenderNode;
			cubRenderNode->setGeometry(cub);
			addRenderNode(cubRenderNode);

			return true;
		}
	}
	return false;
}

bool DeferredScene::initTexture(const SceneInitInfo&)
{
	return true;
}


bool DeferredScene::initThisDemo(const SceneInitInfo&info)
{
	gbuffer_ = new GBuffer(info.w_, info.h_);
	gbuffer_->initBuffer();
	gbuffer_->createGLObj();

	gbuffer_->position_->texture_->eMinFilter = gbuffer_->position_->texture_->eMagFilter = GL_NEAREST;
	gbuffer_->normal_->texture_->eMinFilter = gbuffer_->normal_->texture_->eMagFilter = GL_NEAREST;
	gbuffer_->specular_->texture_->eMinFilter = gbuffer_->specular_->texture_->eMagFilter = GL_NEAREST;

	gbuffer_->position_->texture_->iWrapR = gbuffer_->position_->texture_->iWrapS = gbuffer_->position_->texture_->iWrapT = GL_REPEAT;
	gbuffer_->normal_->texture_->iWrapR = gbuffer_->normal_->texture_->iWrapS = gbuffer_->normal_->texture_->iWrapT = GL_REPEAT;
	gbuffer_->specular_->texture_->iWrapR = gbuffer_->specular_->texture_->iWrapS = gbuffer_->specular_->texture_->iWrapT = GL_REPEAT;

	
	if (!gbuffer_->initGLObj()) return false;
	CHECK_GL_ERROR;
	gbuffer_->bindGLObj();
	gbuffer_->drawToBuffer();
	gbuffer_->unBindGLObj();
	CHECK_GL_ERROR;
	//many obj
	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

	return true;
}

void DeferredScene::initUniformVal(Shader*shader, bool modelNeed /*= true*/)
{
	BaseScene::initUniformVal(shader);
}

bool DeferredScene::initLight(const SceneInitInfo&)
{
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0

		PointLight * light = new PointLight;
		light->color_ = V3f(rColor, gColor, bColor);
		light->position_ = V3f(xPos, yPos, zPos);
		light->atten_.linear_ = 0.7;
		light->atten_.quad_ = 1.8;
		addLight(light);
	}

	return true;
}

void DeferredScene::render(PassInfo&ps)
{
	if (0) //for test
	{
		Shader * currentShader = shaders_[0]; //normal pass
		currentShader->turnOn();
		initUniformVal(currentShader);
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			ps.tranform_ = glm::mat4(1.0);
			ps.tranform_ = glm::translate(ps.tranform_, objectPositions[i]);
			ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(0.25f));
			getRenderNode(0)->render(currentShader, ps);
		}

		return;
	}
	gbuffer_->bindGLObj();
 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader * currentShader = shaders_[1]; //normal pass
	currentShader->turnOn();
	CHECK_GL_ERROR;
	initUniformVal(currentShader);
	CHECK_GL_ERROR;
	for (unsigned int i = 0; i < objectPositions.size(); i++)
	{
		ps.tranform_ = glm::mat4(1.0);
		ps.tranform_ = glm::translate(ps.tranform_, objectPositions[i]);
		ps.tranform_ = glm::scale(ps.tranform_, glm::vec3(0.25f));
		getRenderNode(0)->render(currentShader, ps);
	}
	CHECK_GL_ERROR;
	currentShader->turnOff();
	gbuffer_->unBindGLObj();
	CHECK_GL_ERROR;
	
	//light pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	currentShader = shaders_[2];//deferred 
	currentShader->turnOn();
	glActiveTexture(GL_TEXTURE0);
	gbuffer_->position_->bind();
	glActiveTexture(GL_TEXTURE1);
	gbuffer_->normal_->bind();
	glActiveTexture(GL_TEXTURE2);
	gbuffer_->specular_->bind();

	/*for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		PointLight * pl = (PointLight *)(plights_[i].addr());

		Shader* shader = currentShader; int idx = i;
		applyPointLight(*pl, shader, locaton[i]);
	}*/
	currentShader->setFloat3V(currentShader->getVariable("viewPos", false), 1, &(getCamera()->getPosition()[0]));
	getRenderNode(1)->render(currentShader, ps);
	currentShader->turnOff();
	CHECK_GL_ERROR;

	//copy depth buffer to default buffer
	gbuffer_->copyDepthToDefaultBuffer();

	//render light
	currentShader = shaders_[3];
	currentShader->turnOn();
	initUniformVal(currentShader);
	for (unsigned int i = 0; i < plights_.size(); i++)
	{
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, ((PointLight*)(plights_[i].addr()))->position_);
		model = glm::scale(model, glm::vec3(0.125f));
		ps.tranform_ = model;
		currentShader->setFloat3V(currentShader->getVariable("lightColor"),1, &((PointLight*)(plights_[i].addr()))->color_[0]);
		getRenderNode(2)->render(currentShader, ps);
	}
	currentShader->turnOff();
}

int main()
{
	DeferredScene * scene = new DeferredScene;

	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "deferred";
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
