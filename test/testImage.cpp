
#include <scene.h>
#include "resource.h"
#include <gLApplication.h>
#include <camera.h>
#include <windowManager.h>
#include <glinter.h>
#include <gli.h>
#include <log.h>
#include <renderNode.h>
#include <texture.h>
#include "jpgd.h"
#include "framebuffers.h"
#include "jpeg_gpu.h"
#include "debug.h"
#include "resource.h"
#define  MAX_CHANELS 3

static GLfloat quadVertices[] = {
	// positions   // texCoords
	-512.0f,  256.0f,  0.0f, 1.0f,
	-512.0f, -256.0f,  0.0f, 0.0f,
	512.0f, -256.0f,  1.0f, 0.0f,

	-512.0f,  256.0f,  0.0f, 1.0f,
	512.0f, -256.0f,  1.0f, 0.0f,
	512.0f,  256.0f,  1.0f, 1.0f
};
 
class TestImgScene : public Scene
{
protected:
	virtual void					preRender(PassInfo&);

	virtual bool					initSceneModels(const SceneInitInfo&);
	virtual void					render(PassInfo&);
	virtual bool					initTexture(const SceneInitInfo&);
	virtual bool					initShader(const SceneInitInfo&);
	virtual bool					update();

private:

	Jpeg_Data			jpegData_;
	Jpeg_Data			JpegDataHM_;

	UpdateInfo			 block_;

	base::SmartPointer<Texture>		targetTexture;
	base::SmartPointer<FrameBufferObject> frameBufferObj_;

	unsigned int vao_;
	unsigned int vbo_;
};


void TestImgScene::preRender(PassInfo&)
{

}

bool TestImgScene::initSceneModels(const SceneInitInfo&)
{
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	return true;
}

void TestImgScene::render(PassInfo&ps)
{
	Shader * shader = shaders_[0];
	shader->turnOn();
	initUniformVal(shader);
	glActiveTexture(GL_TEXTURE0);
	targetTexture->bind();
	
	glBindVertexArray(vao_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	shader->turnOff();
}

bool TestImgScene::initTexture(const SceneInitInfo&)
{
	std::string jpegFile = get_texture_BasePath() +  "/Clipmaps/Mars1k.jpg";
	enAssert(jpegData_.loadFile(jpegFile.c_str()));

	//std::string heightJpegFile = "D:/download/Clipmaps/Direct3D/Media/Clipmaps/MarsHm1k.jpg";
	//enAssert(JpegDataHM_.loadFile(heightJpegFile.c_str()));

	Jpeg_Data::initTechnique();
	block_.initialize(1);

	targetTexture = new Texture();
	targetTexture->width_ = jpegData_.width();
	targetTexture->height_ = jpegData_.height();
	targetTexture->baseInternalformat_ = GL_RGBA8;
	targetTexture->type_ = GL_HALF_FLOAT;
	targetTexture->format_ = GL_RGBA;
	targetTexture->createObj();
	targetTexture->bind();
	targetTexture->mirrorRepeat();
	targetTexture->filterLinear();
	targetTexture->contextNULL();
	targetTexture->unBind();

	frameBufferObj_ = new FrameBufferObject(GL_FRAMEBUFFER);
	frameBufferObj_->bindObj(true, false);
	frameBufferObj_->colorTextureAttachments(targetTexture);
	frameBufferObj_->bindObj(false, false);
	CHECK_GL_ERROR;


	return true;
}

bool TestImgScene::initShader(const SceneInitInfo&)
{
	Shader * shader = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec2 position;"
		"layout(location = 1) in vec2 texCoord;"

		"uniform mat4 view;"
		"uniform mat4 projection;"
		"out vec2 texCoords;"
		"void main()"
		"{"
			"gl_Position = projection * view *  vec4(position,1.0, 1.0f);"
			"texCoords = texCoord;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"out vec4 color;"
		"in vec2 texCoords;"
		"uniform sampler2D diffuseTex;"
		"void main()"
		"{"
		"color = texture(diffuseTex,texCoords);"
		"}";

	shader->loadShaderSouce(vertShder, fragShader, NULL);
	shaders_.push_back(shader);

	shader->turnOn();
	unsigned loc = shader->getVariable("diffuseTex");
	if (loc)
	{
		shader->setInt(loc, 0);
	}
	
	return true;
}

bool TestImgScene::update()
{
	int srcBlock[2] = { /*jpegData_.width(),jpegData_.height()*/0,0 };
	int dstBlock[2];
	block_.addBlock(srcBlock, dstBlock);

	jpegData_.allocateTextures(jpegData_.width() / 2, jpegData_.height() / 2);
	jpegData_.updateTextureData(1, jpegData_.height() / 2, block_.getSrcBlocksPointer());

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	jpegData_.uncompressTextureData();
	block_.reset();

	Shader* shader = Jpeg_Data::getTechnique("IDCT_RenderToBuffer");
	shader->turnOn();

	frameBufferObj_->bindObj(true, true);
	frameBufferObj_->clearBuffer();

	CHECK_GL_ERROR;

	glActiveTexture(GL_TEXTURE0);
	jpegData_.getFinalTarget(0)->bind();
	glActiveTexture(GL_TEXTURE1);
	jpegData_.getFinalTarget(1)->bind();
	glActiveTexture(GL_TEXTURE2);
	jpegData_.getFinalTarget(2)->bind();
	glViewport(0, 0, targetTexture->width(), targetTexture->heigh());
	Jpeg_Data::getQuad()->draw();
	frameBufferObj_->bindObj(false, true);

	CHECK_GL_ERROR;

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	return true;
}

int main()
{
	unsigned char uc = 210;
	char c = uc;

	TestImgScene * scene = new TestImgScene;
	Camera *pCamera = new Camera();
	scene->setMasterCamera(pCamera);
	WindowManager *pWindowManager = new WindowManager();
	GLApplication application(scene);
	application.setWindowManager(pWindowManager);

	WindowConfig wc;
	DeviceConfig dc;
	wc.title_ = "test_Image";
	wc.width_ = 1024;
	wc.height_ = 960;
	wc.pos_x_ = 50;
	wc.pos_y_ = 50;

	dc.glVersion_.glMaxJor_ = 3.0;
	dc.glVersion_.glMinJor_ = 3.0;

	application.initialize(&wc, &dc);

	application.initScene();
	pCamera->positionCamera(0.0, 0.0, 1000, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	pCamera->setClipPlane(0.1f, 1000.0f);
	application.start();

	return 0;
}
