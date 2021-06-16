

/**
   https://github.com/martin-pr/possumwood/wiki/Geometry-Shader-Tessellation-using-PN-Triangles
   https://alex.vlachos.com/graphics/CurvedPNTriangles.pdf
   PN (Point-Normal) Triangles with Geometry Shader
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
#include "windowManager.h"
#include "log.h"
#include "helpF.h"
#include <Inputmanager.h>
#include <glinter.h>

struct ShaderVLocation
{
	GLint eyeWorldPosLocation;
	GLint vPLocation;

	GLint gessellationLevel;

	GLint numPointLightsLocation;
	GLint numSpotLightsLocation;
};

ShaderVLocation svl;
ShaderVLocation svl2;

static const unsigned int MAX_POINT_LIGHTS = 2;
static const unsigned int MAX_SPOT_LIGHTS = 2;
static const float FieldDepth = 20.0f;

DirectionLightLocation  m_dirLightLocation;
PointLightLocation  m_pointLightsLocation[MAX_POINT_LIGHTS];
SpotLightLocation  m_spotLightsLocation[MAX_SPOT_LIGHTS];

DirectionLightLocation  m_dirLightLocation2;
PointLightLocation  m_pointLightsLocation2[MAX_POINT_LIGHTS];
SpotLightLocation  m_spotLightsLocation2[MAX_SPOT_LIGHTS];

bool g_showMatrial = false;

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

enum RenderTech
{
	NORMAL_TECH,
	POLYGON_SUBDIVISION_TECH,
	PN_TRIANGLES_SUBDIVISION_TECH,
	PN_TRIANGLES_QUADRATIC_NORMLAL_SUBDIVISION_TECH
};

RenderTech						renderTech_ = NORMAL_TECH;

bool keyEvent(int key)
{
	static int t = 0;

	if (key == 49)
	{
		t++;
		if (t == 0)
			renderTech_ = NORMAL_TECH;
		else if (t == 1)
			renderTech_ = POLYGON_SUBDIVISION_TECH;
		else if (t == 2)
			renderTech_ = PN_TRIANGLES_SUBDIVISION_TECH;
		else if (t == 3)
			renderTech_ = PN_TRIANGLES_QUADRATIC_NORMLAL_SUBDIVISION_TECH;
		else t = -1;
	}
	else if (key == 50)
	{
		g_showMatrial = !g_showMatrial;
	}

	return true;
}

class PNTTessScene :public Scene
{

protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initLight(const SceneInitInfo&);
	virtual bool					initKeyCallback();

public:
	virtual void					render(PassInfo&);

	void							normalRender(PassInfo&);
	void							polyGonRender(PassInfo&);
	void							pnTriangleRender(PassInfo&);
	void							pnTriangleQuadraticRender(PassInfo&);

private:
	void							realRender(PassInfo&, Shader*,int lightindx = -1);

	float							m_tessellationLevel = 5.0f;
	bool							m_isWireframe = false;
	float							m_scale = 0.0f;

};

bool PNTTessScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);  //normal geo shader and show the geo shader 
	Shader * shader = shaders_[0];
	shader->addShader(Shader::VERTEX,   "./tessellation/pn_triangles_geo.vert"); CHECK_GL_ERROR;
	shader->addShader(Shader::FRAGMENT, "./tessellation/pn_triangles_geo.frag"); CHECK_GL_ERROR;
	shader->addShader(Shader::GEOMETRY, "./tessellation/pn_triangles_geo.geo"); CHECK_GL_ERROR;
	shader->linkProgram();
	shader->checkProgram();
	CHECK_GL_ERROR;


	shaders_.push_back(new Shader); // polygon subdivison
	shader = shaders_[1];
	shader->addShader(Shader::VERTEX, "./tessellation/pn_triangles_geo.vert"); CHECK_GL_ERROR;
	shader->addShader(Shader::FRAGMENT, "./tessellation/pn_triangles_geo.frag"); CHECK_GL_ERROR;
	shader->addShader(Shader::GEOMETRY, "./tessellation/pn_triangles_geo_polygon_subdivision.geo"); CHECK_GL_ERROR;
	glProgramParameteriEXT(shader->getShaderId(), GL_MAX_GEOMETRY_OUTPUT_VERTICES, 1024);
	shader->linkProgram();
	shader->checkProgram();
	CHECK_GL_ERROR;


	shaders_.push_back(new Shader); // pn triangle subdivison
	shader = shaders_[2];
	shader->addShader(Shader::VERTEX, "./tessellation/pn_triangles_geo_pn.vert"); CHECK_GL_ERROR;
	shader->addShader(Shader::FRAGMENT, "./tessellation/pn_triangles_geo_pn.frag"); CHECK_GL_ERROR;
	shader->addShader(Shader::GEOMETRY, "./tessellation/pn_triangles_geo_pn.geo"); CHECK_GL_ERROR;
	shader->linkProgram();
	shader->checkProgram();
	CHECK_GL_ERROR;


	shaders_.push_back(new Shader); // pn triangle  subdivison and normal generation with  quadratic patch
	shader = shaders_[3];
	shader->addShader(Shader::VERTEX, "./tessellation/pn_triangles_geo_pn.vert"); CHECK_GL_ERROR;
	shader->addShader(Shader::FRAGMENT, "./tessellation/pn_triangles_geo_pn_quadriatic_normal.frag"); CHECK_GL_ERROR;
	shader->addShader(Shader::GEOMETRY, "./tessellation/pn_triangles_geo_pn_quadratic_normal.geo"); CHECK_GL_ERROR;
	shader->linkProgram();
	shader->checkProgram();
	CHECK_GL_ERROR;

	shader = shaders_[2];
	shader->turnOn();
	svl.eyeWorldPosLocation = shader->getVariable("gEyeWorldPos");//cs
	svl.numPointLightsLocation = shader->getVariable("gNumPointLights");
	svl.numSpotLightsLocation = shader->getVariable("gNumSpotLights");
	getDirectionLightShaderLocation(0, shader, m_dirLightLocation);
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLightsLocation); i++)
		getPointLightShaderLocation(i, shader, m_pointLightsLocation[i]);
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_spotLightsLocation); i++)
		getSpotLightShaderLocation(i, shader, m_spotLightsLocation[i]);
	shader->turnOff();

	shader = shaders_[3];
	shader->turnOn();
	svl2.eyeWorldPosLocation = shader->getVariable("gEyeWorldPos");//cs
	svl2.numPointLightsLocation = shader->getVariable("gNumPointLights");
	svl2.numSpotLightsLocation = shader->getVariable("gNumSpotLights");
	getDirectionLightShaderLocation(0, shader, m_dirLightLocation2);
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLightsLocation2); i++)
		getPointLightShaderLocation(i, shader, m_pointLightsLocation2[i]);
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_spotLightsLocation2); i++)
		getSpotLightShaderLocation(i, shader, m_spotLightsLocation2[i]);
	shader->turnOff();

	renderTech_ = NORMAL_TECH;
	
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
	GLint maxGeometryVertices;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryVertices);
	printf("max geometry output vertices is : %d \n", maxGeometryVertices);
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

bool PNTTessScene::initKeyCallback()
{
	Scene::initKeyCallback();

	KEY_CALLBACK(GLU_KEY_1, base::NewPermanentCallback(keyEvent)); //key_1
	KEY_CALLBACK(GLU_KEY_2, base::NewPermanentCallback(keyEvent)); //key_2
	return true;
}

void PNTTessScene::render(PassInfo&ps)
{
	switch (renderTech_)
	{
	case NORMAL_TECH:
		normalRender(ps);
		break;
	case POLYGON_SUBDIVISION_TECH:
		polyGonRender(ps);
		break;
	case PN_TRIANGLES_SUBDIVISION_TECH:
		pnTriangleRender(ps);
		break;
	case PN_TRIANGLES_QUADRATIC_NORMLAL_SUBDIVISION_TECH:
		pnTriangleQuadraticRender(ps);
		break;
	default:
		break;
	}
}

void PNTTessScene::normalRender(PassInfo&ps)
{
	Shader * cshader = shaders_[0];
	realRender(ps, cshader);
}

void PNTTessScene::polyGonRender(PassInfo&ps)
{
	Shader * cshader = shaders_[1];
	realRender(ps, cshader);
}

void PNTTessScene::pnTriangleRender(PassInfo&ps)
{
	Shader * cshader = shaders_[2];
	realRender(ps, cshader,1);
}

void PNTTessScene::pnTriangleQuadraticRender(PassInfo&ps)
{
	Shader * cshader = shaders_[3];
	realRender(ps, cshader,2);
}

void PNTTessScene::realRender(PassInfo&ps, Shader*cshader, int lightindx)
{
	cshader->turnOn();

	initUniformVal(cshader);

	GLint location = cshader->getVariable("showMatrial");
	if (location != -1)
	{
		cshader->setFloat(location,g_showMatrial);
	}
	CHECK_GL_ERROR;
	if (lightindx == -1) {}
	else if (lightindx == 0)
	{
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
		CHECK_GL_ERROR;

	}
	else if (lightindx == 1)
	{
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
		CHECK_GL_ERROR;

	}
	else
	{
		applyDirectionLight(*(DirectionalLight*)(dlights_[0].addr()), cshader, m_dirLightLocation2);

		cshader->setInt(svl.numPointLightsLocation, plights_.size());
		for (unsigned int i = 0; i < plights_.size(); i++)
		{
			applyPointLight(*(PointLight*)(plights_[i].addr()), cshader, m_pointLightsLocation2[i]);
		}
		cshader->setInt(svl.numSpotLightsLocation, slights_.size());
		for (unsigned int i = 0; i < slights_.size(); i++)
		{
			applySpointLight(*(SpotLight*)(slights_[i].addr()), cshader, m_spotLightsLocation2[i]);
		}
		CHECK_GL_ERROR;
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

	application.initialize(&wc, &dc);
	application.initScene();
	pCamera->setClipPlane(0.1f, 500.0f);
	application.start();

	return 0;
}
