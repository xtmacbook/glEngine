/************************************************************************/
/* An Image Synthesizer
	Ken Perlin

	test
*/
/************************************************************************/

#include "perlineNoise.h"

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

class Image_Synthesizer_Scene :public Scene
{
public:

	virtual ~Image_Synthesizer_Scene() {}
protected:
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual bool					initThisDemo(const SceneInitInfo&);
	virtual void					processKeyboard(int key, int st, int action, int mods, float deltaTime);
	virtual bool					initTexture(const SceneInitInfo&);

public:
	virtual void					render(PassInfo&);

	Shader*							render_shader_ = nullptr;

	ImprovedPerlinNoise						noise_;
private:
};

bool Image_Synthesizer_Scene::initShader(const SceneInitInfo&si)
{
	bool r = Scene::initShader(si);
	render_shader_ = shaders_[0];

	return r;
}

bool Image_Synthesizer_Scene::initSceneModels(const SceneInitInfo&)
{
	return true;
}

V3f SpottedColor(float color)
{
	unsigned char n = static_cast<unsigned char>(color * 255);
	color = n / 255.0;
	return V3f(color, color, color);
}
V3f BozoColor(float color)
{
	static V3f bozo[6] = {
		V3f{0.5,0.6,0.7},
		V3f{ 0.3,0.3,0.7 },
		V3f{ 0.2,0.7,0.7 },
		V3f{ 0.1,0.6,0.4 },
		V3f{ 0.8,0.9,0.1 }, 
		V3f{ 0.8,0.9,0.9 }
	};

	int idx = (int)(color / 0.2);
	return bozo[idx];
}

bool Image_Synthesizer_Scene::initThisDemo(const SceneInitInfo&)
{
	ClassPerlineNoise cnoise(1,1,1,99);

	IO::LModelInfo data;
	if (IO::EngineLoad::loadNode((get_model_BasePath() + "sphere.obj").c_str(), data))
	{
		if (!data.meshs_.empty())
		{
			RenderNode_SP rn = new RenderNode;
			base::SmartPointer<MeshGeometry> mg = new MeshGeometry(rn);
			mg->meshs_ = data.meshs_;

			std::vector<Mesh_SP>& meshs = mg->meshs_;
			for (int i = 0; i < meshs.size(); i++)
			{
				Mesh_SP mesh = meshs[i];
				std::vector<std::vector<Vertex> >& vss = mesh->vertices_;
				for (int j = 0; j < vss.size();j++)
				{
					std::vector<Vertex>&vs = vss[j];
					for (int k = 0; k < vs.size(); k++)
					{
						Vertex& v = vs[k];
						//float color = (noise_.eval(v.Position * 256.0f * 40.f, V3f(0.0, 0.0, 0.0)) + 1) * 0.5 ;

						V3f pos = v.Position * 4096.0f;
						float color = cnoise.eval(pos[0], pos[1], pos[2]) * 4;
						color = (color + 1) * 0.5;
						v.Normal = V3f(color, color, color);
						//v.Normal = SpottedColor(color);
						//v.Normal = BozoColor(color);
					}

				}

			}

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

	return true;
}

void Image_Synthesizer_Scene::processKeyboard(int key, int st, int action, int mods, float deltaTime)
{
	Scene::processKeyboard(key, st, action, mods, deltaTime);

	if (action == GLU_PRESS)
	{
		if (key == GLU_KEY_1)
		{
			render_shader_ = shaders_[0];
		}
		if (key == GLU_KEY_2)
		{
			render_shader_ = shaders_[1];
		}
	}
}

bool Image_Synthesizer_Scene::initTexture(const SceneInitInfo&)
{
	/*std::string tx = get_texture_BasePath() + "diffuse.jpg";
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
	}*/
	return true;
}

void Image_Synthesizer_Scene::render(PassInfo&info)
{
	render_shader_->turnOn();
	initUniformVal(render_shader_);
	getRenderNode(0)->render(render_shader_, info);
}

int main()
{

	Image_Synthesizer_Scene* scene = new Image_Synthesizer_Scene;
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
