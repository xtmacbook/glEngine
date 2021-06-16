

/**
 * note:  Tessellation process is placed between the vertex and geometric shaders. The tessellation shader is actually composed of 3 separate parts:
	a)Tessellation Control Shader (TCS) takes input from the VS (vertex shader) and is responsible for 2 tasks: determining the tessellation levels to be sent to the TE (Tessellation Engine) and generating data to be passed to the TES.
	b)Tessellation Engine (TE) [Primitive Generator(PG)] which does the actual tessellation, by breaking down the patch according to the levels set in the TCS
	c)Tessellation Evaluation Shader is the last link in the process, taking each new vertex created by the tessellation and giving it a position within the world

	The TCS works on a group of vertices called Control Points (CP). The group of CPs is usually called a Patch.The TCS takes an input patch and emits an output patch.In addition to the output patch the control shader calculates a set of numbers called Tessellation Levels (TL). The TLs determine the Tessellation level of detail - how many triangles to generate for the patch. For example, we can decide that the TLs will be 3 if the rasterized triangle is going to cover less than a 100 pixels, 7 in case of 101 to 500 pixels and 12.5 for everything above that.
	TE(PG):
		It doesn't really subdivides the output patch of the TCS. In fact, it doesn't even have access to it. Instead, it takes the TLs and subdivides what is called a Domain. The domain can either be a normalized (in the range of 0.0-1.0) square of 2D coordinates or an equilateral triangle defined by 3D barycentric coordinates.Barycentric coordinates of a triangle is a method of defining a location inside a triangle as a combination of the weight of the three vertices.The PG takes the TLs and based on their values generates a set of points inside the triangle. Each point is defined by its own barycentric coordinate.Its only output are barycentric coordinates and their connectivity.

	TES:This shader stage has access both to the output patch of the TCS and the barycentric coordinates that the PG generated.The TES is similar to the VS in the sense that it always has a single input (the barycentric coordinate) and a single output (the vertex). The TES cannot generate more than one vertex per invocation nor can it decide to drop the vertex. The main purpose of the TES that the architects of Tessellation in OpenGL envisioned is to evaluate the surface equation at the given domain location
	web:http://ogldev.atspace.co.uk/www/tutorial30/tutorial30.html
	*/
#include <vector>
#include <string>
#include <algorithm>

#include "windowManager.h"
#include "gLApplication.h"
#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "resource.h"
#include "log.h"
#include "engineLoad.h"
#include "image.h"
#include "helpF.h"
#include "callback.h"
#include "Inputmanager.h"
#include "renderNode.h"
#include <util.h>
#include <light.h>
#include <glinter.h>
static const unsigned int MAX_POINT_LIGHTS = 2;
static const unsigned int MAX_SPOT_LIGHTS = 2;
static const float FieldDepth = 20.0f;

DirectionLightLocation  m_dirLightLocation;
PointLightLocation  m_pointLightsLocation[MAX_POINT_LIGHTS];
SpotLightLocation  m_spotLightsLocation[MAX_SPOT_LIGHTS];

class TessScene :public Scene
{
public:

protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initLight(const SceneInitInfo&);
public:
	virtual void					render(PassInfo&);

private:
	float							m_dispFactor = 0.25f;
	float							m_scale = 0.0f;
};

struct ShaderVLocation
{
	GLint worldMatrixLocation;
	GLint eyeWorldPosLocation;
	GLint vPLocation;

	GLint displacementMapLocation;
	GLint colorTextureLocation;

	GLint dispFactorLocation;

	GLint numPointLightsLocation;
	GLint numSpotLightsLocation;
};

ShaderVLocation svl;

bool TessScene::initShader(const SceneInitInfo&)
{
	shaders_.push_back(new Shader);
	Shader* shader = shaders_[0];
	CHECK_GL_ERROR;

	if (!shader->addShader(Shader::VERTEX, "./tessellation/tess.vert")) return false; CHECK_GL_ERROR;
	if (!shader->addShader(Shader::FRAGMENT, "./tessellation/tess.frag")) return false; CHECK_GL_ERROR;
	if (!shader->addShader(Shader::TESS_TCL, "./tessellation/tess.cs")) return false; CHECK_GL_ERROR;
	if (!shader->addShader(Shader::TESS_TES, "./tessellation/tess.es")) return false; CHECK_GL_ERROR;

	shader->linkProgram();
	shader->checkProgram();

	svl.eyeWorldPosLocation = shader->getVariable("gEyeWorldPos");//cs

	svl.vPLocation = shader->getVariable("gVP");
	svl.displacementMapLocation = shader->getVariable("gDisplacementMap");//es
	svl.dispFactorLocation = shader->getVariable("gDispFactor");

	svl.colorTextureLocation = shader->getVariable("gColorMap0");

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

bool TessScene::initSceneModels(const SceneInitInfo&)
{
	{
		IO::LModelInfo data;
		std::string modelPath = get_model_BasePath() + "quad/quad.obj";
		if (IO::EngineLoad::loadNode(modelPath.c_str(), data))
		{
			if (!data.meshs_.empty())
			{
				RenderNode_SP rn = new RenderNode;
				base::SmartPointer<MeshGeometry> mg = new MeshGeometry();
				mg->meshs_ = data.meshs_;
				for_each(mg->meshs_.begin(), mg->meshs_.end(), [](Mesh_SP msh) {
					msh->rmode() = GL_PATCHES;
				});
				mg->initGeometry();
				if (rn)
				{
					rn->setGeometry(mg);
					addRenderNode(rn);
				}
			}
		}
		return true;

	}
}

bool TessScene::initThisDemo(const SceneInitInfo&)
{
	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	return true;
}

bool TessScene::initTexture(const SceneInitInfo&)
{
	std::string tx = get_texture_BasePath() + "heightmap.jpg";
	Texture* curTexObj = new Texture(tx.c_str());
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

	tx = get_texture_BasePath() + "diffuse.jpg";
	curTexObj = new Texture(tx.c_str());
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
			return true;
		}
		CHECK_GL_ERROR;
		return false;
	}
	return true;
}

bool TessScene::initLight(const SceneInitInfo&)
{
	DirectionalLight* dl = new DirectionalLight;
	dl->color_ = V3f(1.0f, 1.0f, 1.0f);
	dl->ambientIntensity_ = 1.0f;
	dl->diffuseIntensity_ = 0.01f;
	dl->direction_ = V3f(1.0f, -1.0, 0.0);
	dl->name_ = "director0";
	addLight(dl);

	PointLight* point0 = new PointLight;
	point0->diffuseIntensity_ = 0.5f;
	point0->color_ = V3f(1.0f, 0.5f, 0.0f);
	point0->position_ = V3f(3.0f, 1.0f, FieldDepth * (cosf(m_scale) + 1.0f) / 2.0f);
	point0->atten_.linear_ = 0.1f;
	point0->name_ = "point0";
	addLight(point0);

	PointLight* point1 = new PointLight;
	point1->diffuseIntensity_ = 0.5f;
	point1->color_ = V3f(0.0f, 0.5f, 1.0f);
	point1->position_ = V3f(7.0f, 1.0f, FieldDepth * (sinf(m_scale) + 1.0f) / 2.0f);
	point1->atten_.linear_ = 0.1f;
	addLight(point1);

	SpotLight* sp0 = new SpotLight;
	sp0->diffuseIntensity_ = 0.9f;
	sp0->color_ = V3f(0.0f, 1.0f, 1.0f);
	sp0->atten_.linear_ = 0.1f;
	sp0->spotCutoff_ = 10.0f;
	sp0->name_ = "spotlight0";
	addLight(sp0);

	SpotLight* sp1 = new SpotLight;
	sp1->diffuseIntensity_ = 0.9f;
	sp1->color_ = V3f(1.0f, 1.0f, 1.0f);
	sp1->position_ = V3f(5.0f, 3.0f, 10.0f);
	sp1->direction_ = V3f(0.0f, -1.0f, 0.0f);
	sp1->atten_.linear_ = 0.1f;
	sp1->spotCutoff_ = 20.0f;
	addLight(sp1);

	return true;
}

void TessScene::render(PassInfo& ps)
{
	Shader* cshader = shaders_[0];
	cshader->turnOn();
	V3f pos = camera_->getPosition();
	cshader->setFloat3(svl.eyeWorldPosLocation, pos.x, pos.y, pos.z);
	cshader->setMatrix4(svl.vPLocation, 1, false, math::value_ptr(camera_->getProjectionMatrix() * camera_->getViewMatrix()));
	cshader->setFloat(svl.dispFactorLocation, m_dispFactor);

	applyDirectionLight(*(DirectionalLight*)(dlights_[0].addr()), cshader, m_dirLightLocation);

	cshader->setFloat(svl.numPointLightsLocation, plights_.size());
	for (unsigned int i = 0; i < plights_.size(); i++)
	{
		applyPointLight(*(PointLight*)(plights_[i].addr()), cshader, m_pointLightsLocation[i]);
	}
	cshader->setFloat(svl.numSpotLightsLocation, slights_.size());
	for (unsigned int i = 0; i < slights_.size(); i++)
	{
		applySpointLight(*(SpotLight*)(slights_[i].addr()), cshader, m_spotLightsLocation[i]);
	}

	glActiveTexture(GL_TEXTURE0);
	texturesObj_[1]->bind();
	cshader->setInt(svl.colorTextureLocation, 0);
	glActiveTexture(GL_TEXTURE1);
	texturesObj_[0]->bind();
	cshader->setInt(svl.displacementMapLocation, 1);

	renderNodes_[0]->render(cshader, ps);

	cshader->turnOff();
}

int main()
{
	TessScene* scene = new TessScene;
	Camera* pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager* pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	dc.glVersion_.glMaxJor_ = 4;
	dc.glVersion_.glMinJor_ = 3;
	wc.title_ = "Tess";
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

