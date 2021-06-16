

/**
   https://www.gamasutra.com/view/feature/131389/bezier_triangles_and_npatches.php?page=2
 * http://ogldev.atspace.co.uk/www/tutorial31/tutorial31.html
   https://alex.vlachos.com/graphics/CurvedPNTriangles.pdf
   PN (Point-Normal) Triangles
   We are going to use a special case of a Bezier Surface called a Bezier Triangle which has the following form:

 */
#include <algorithm>


#include "scene.h"
#include "RenderNode.h"
#include "shader.h"
#include "engineLoad.h"
#include "resource.h"
#include "gLApplication.h"
#include "camera.h"
#include "log.h"
#include "helpF.h"
#include "windowManager.h"
#include "gls.h"
#include "glinter.h"
struct ShaderVLocation
{
	GLint worldMatrixLocation;
	GLint eyeWorldPosLocation;
	GLint vPLocation;

	GLint gessellationLevel;

	GLint numPointLightsLocation;
	GLint numSpotLightsLocation;
};

ShaderVLocation svl;
static const unsigned int MAX_POINT_LIGHTS = 2;
static const unsigned int MAX_SPOT_LIGHTS = 2;
static const float FieldDepth = 20.0f;

DirectionLightLocation  m_dirLightLocation;
PointLightLocation  m_pointLightsLocation[MAX_POINT_LIGHTS];
SpotLightLocation  m_spotLightsLocation[MAX_SPOT_LIGHTS];

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
	unsigned int diffuseNr = 0;
	unsigned int specularNr = 0;
	unsigned int normalNr = 0;
	unsigned int heightNr = 0;

	for (unsigned int i = 0; i < t_indices_.size(); i++)
	{
		std::string number;
		std::string name;
		Texture_Sp texture = rs_->textures_[t_indices_[0]];
		TEXTURE_TYPE tt = texture->t_type_;
		switch (tt)
		{
		case T_DIFFUSE:
			name = "gColorMap";
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
	mesh->rmode() = GL_PATCHES;
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

class PNTTessScene :public Scene
{
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initLight(const SceneInitInfo&);
public:
	virtual void					render(PassInfo&);

	float							m_tessellationLevel = 5.0f;
	bool							m_isWireframe = false;
	float							m_scale = 0.0f;

};

bool PNTTessScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);
	Shader * shader = shaders_[0];
	shader->addShader(Shader::VERTEX, "./tessellation/pn_triangle_light.vert"); CHECK_GL_ERROR;
	shader->addShader(Shader::FRAGMENT, "./tessellation/tess.frag"); CHECK_GL_ERROR;
	shader->addShader(Shader::TESS_TCL, "./tessellation/pn_triangle_light.cs"); CHECK_GL_ERROR;
	shader->addShader(Shader::TESS_TES, "./tessellation/pn_triangle_light.es"); CHECK_GL_ERROR;
	shader->linkProgram();
	shader->checkProgram();
	CHECK_GL_ERROR;

	shader->linkProgram();
	shader->checkProgram();

	svl.eyeWorldPosLocation = shader->getVariable("gEyeWorldPos");//cs

	svl.vPLocation = shader->getVariable("gVP");
	svl.gessellationLevel = shader->getVariable("gTessellationLevel");

	svl.numPointLightsLocation = shader->getVariable("gNumPointLights");
	svl.numSpotLightsLocation = shader->getVariable("gNumSpotLights");

	getDirectionLightShaderLocation(0, shader, m_dirLightLocation);
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLightsLocation); i++)
	{
		getPointLightShaderLocation(i, shader, m_pointLightsLocation[i]);
	}

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_spotLightsLocation); i++)
	{
		getSpotLightShaderLocation(i, shader, m_spotLightsLocation[i]);
	}
	return true;
}

bool PNTTessScene::initSceneModels(const SceneInitInfo&)
{
	IO::LModelInfo data;
	std::string modelPath = get_model_BasePath() + "spider/spider.obj";
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
			return true;
		}
	}
	return false;
}

bool PNTTessScene::initThisDemo(const SceneInitInfo&)
{
	return true;
}

bool PNTTessScene::initTexture(const SceneInitInfo&)
{
	return true;
}

bool PNTTessScene::initLight(const SceneInitInfo&)
{
	DirectionalLight * dl = new DirectionalLight;
	dl->color_ = V3f(1.0f, 1.0f, 1.0f);
	dl->ambientIntensity_ = 1.0f;
	dl->diffuseIntensity_ = 0.01f;
	dl->direction_ = V3f(1.0f, -1.0, 0.0);
	addLight(dl);

	PointLight* point0 = new PointLight;
	point0->diffuseIntensity_ = 0.5f;
	point0->color_ = V3f(1.0f, 0.5f, 0.0f);
	point0->position_ = V3f(3.0f, 1.0f, FieldDepth * (cosf(m_scale) + 1.0f) / 2.0f);
	point0->atten_.linear_ = 0.1f;
	addLight(point0);

	PointLight* point1 = new PointLight;
	point1->diffuseIntensity_ = 0.5f;
	point1->color_ = V3f(0.0f, 0.5f, 1.0f);
	point1->position_ = V3f(7.0f, 1.0f, FieldDepth * (sinf(m_scale) + 1.0f) / 2.0f);
	point1->atten_.linear_ = 0.1f;
	addLight(point1);

	SpotLight * sp0 = new SpotLight;
	sp0->diffuseIntensity_ = 0.9f;
	sp0->color_ = V3f(0.0f, 1.0f, 1.0f);
	sp0->atten_.linear_ = 0.1f;
	sp0->spotCutoff_ = 10.0f;
	addLight(sp0);

	SpotLight * sp1 = new SpotLight;
	sp1->diffuseIntensity_ = 0.9f;
	sp1->color_ = V3f(1.0f, 1.0f, 1.0f);
	sp1->position_ = V3f(5.0f, 3.0f, 10.0f);
	sp1->direction_ = V3f(0.0f, -1.0f, 0.0f);
	sp1->atten_.linear_ = 0.1f;
	sp1->spotCutoff_ = 20.0f;
	addLight(sp1);

	return true;
}

void PNTTessScene::render(PassInfo&ps)
{
	Shader * cshader = shaders_[0];
	cshader->turnOn();
	V3f pos = camera_->getPosition();
	cshader->setFloat3(svl.eyeWorldPosLocation, pos.x, pos.y, pos.z);
	cshader->setMatrix4(svl.vPLocation, 1, false, math::value_ptr(camera_->getProjectionMatrix() * camera_->getViewMatrix()));
	cshader->setFloat(svl.gessellationLevel, m_tessellationLevel);

	applyDirectionLight(*(DirectionalLight*)(dlights_[0].addr()), cshader, m_dirLightLocation);

	cshader->setInt(svl.numPointLightsLocation, plights_.size());
	for (unsigned int i = 0; i < plights_.size(); i++)
	{
		applyPointLight(*(PointLight*)(plights_[i].addr()), cshader, m_pointLightsLocation[i]);
	}
	cshader->setInt(svl.numSpotLightsLocation, slights_.size());
	for (unsigned int i = 0; i < slights_.size(); i++)
	{
		applySpointLight(*(SpotLight*)(slights_[i].addr()), cshader, m_spotLightsLocation[i]);
	}
	renderNodes_[0]->render(cshader, ps);

	cshader->turnOff();
}

int main()
{

	PNTTessScene* scene = new PNTTessScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);

	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "pn_triangle_tess";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;
	dc.glVersion_.glMaxJor_ = 4;
	dc.glVersion_.glMinJor_ = 3;

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
