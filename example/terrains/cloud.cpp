#include "cloud.h"
#include <resource.h>
#include <log.h>
#include <helpF.h>
#include <timerManager.h>

extern const int  SKYDOME_SLICES;
extern const int  SKYDOME_STACKS;
extern const int  SKYDOME_RADIUS;

struct LV
{
	V3f position_;
	V4f color_;
};

Cloud::Cloud():vertexBuffer_Ao_(0),
mCloudMove_(0, 0)
{
	vb_[0] = vb_[1] = 0;
}

Cloud::~Cloud()
{
	if (vb_[0])
	{
		glDeleteBuffers(2, vb_);
	}
	if (vertexBuffer_Ao_)
	{
		glDeleteVertexArrays(1, &vertexBuffer_Ao_);
	}
}

void Cloud::init()
{
	int vertices_count = 1 + SKYDOME_SLICES;
	trianglesCount_ = SKYDOME_SLICES;
	LV * vert_buf = new LV[vertices_count];
	unsigned int * idx_buf = new unsigned int[trianglesCount_ * 3];
	LV * v = vert_buf;

	float fz = 2.0f;
	float radius = sqrtf(SKYDOME_RADIUS * SKYDOME_RADIUS - fz * fz);

	v->position_ = V3f(0.0f,fz, 0.0f);
	v->color_ = V4f(0.5f, 0.5f, 0.0f, 0.2f); // r, g: texture coordinates, b: not used, a: alpha
	v++;

	float rad_step = (math::Const<float>::pi() * 2.0f / (float)SKYDOME_SLICES);
	for (int i = 0; i < SKYDOME_SLICES; ++i) {
		float rad = i * rad_step;

		float fx = radius * sinf(rad);
		float fy = radius * cosf(rad);

		float t0 = (sinf(rad) + 1.0f) * 0.5f;
		float t1 = 1.0f - (cosf(rad) + 1.0f) * 0.5f;

		v->position_ = V3f(fx, fz, fy);
		v->color_ = V4f(t0, t1, 0.0f, 0.0f);//alpha 0: cloud edge fade out
		v++;
	}

	// repeat cloud texture
	for (int i = 0; i < vertices_count; ++i) 
	{
		vert_buf[i].color_[0] *= 16.0f;
		vert_buf[i].color_[1] *= 16.0f;
	}

	unsigned int * t = idx_buf;
	for (int i = 0; i < SKYDOME_SLICES; ++i) {
		t[0] = 0;
		t[1] = i + 1;
		t[2] = i + 2;

		if ((int)t[2] >= vertices_count) {
			t[2] = 1;
		}

		t += 3;
	}

	glGenVertexArrays(1, &vertexBuffer_Ao_);
	glBindVertexArray(vertexBuffer_Ao_);

	glGenBuffers(2, vb_);
	glBindBuffer(GL_ARRAY_BUFFER, vb_[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LV) * vertices_count, vert_buf, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LV), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LV), (void*)offsetof(LV, color_));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb_[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * trianglesCount_ * 3, idx_buf, GL_STATIC_DRAW);
	glBindVertexArray(0);
}


void Cloud::initTexture()
{
	std::string tx = get_media_BasePath() + "textures/cloud.png";
	texture_ = new Texture(tx.c_str());
	texture_->target_ = GL_TEXTURE_2D;
	if (texture_->loadData())
	{
		texture_->createObj();
		texture_->bind();
		texture_->mirrorRepeat();
		texture_->filterLinear();
		if (!texture_->context(NULL))
		{
		}
		CHECK_GL_ERROR;
	}
}

void Cloud::render(const CameraBase*camera)
{
	shader_->turnOn();
	GLint location = shader_->getVariable("projection");
	if (location != -1)
		shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(camera->getProjectionMatrix()));

	location = shader_->getVariable("view");
	if (location != -1)
	{
		V3f viewDir = camera->getViewDir();
		Matrixf mvp = math::lookAt(V3f(0.0, 0.0, 0.0), viewDir, V3f(0.0, 1.0, 0.0));
		shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(mvp));
	}
	location = shader_->getVariable("g_move");
	if (location != -1)
	{
		mCloudMove_[0] += 0.01f * TimeManager::instance().getDeltaTime();
		mCloudMove_[1] += 0.01f * TimeManager::instance().getDeltaTime();
		shader_->setFloat2(location, mCloudMove_[0], mCloudMove_[1]);
	}
	glBindVertexArray(vertexBuffer_Ao_);
	glActiveTexture(GL_TEXTURE0);
	texture_->bind();
	glDrawElements(GL_TRIANGLES, trianglesCount_ * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	CHECK_GL_ERROR;
}

void Cloud::initShader()
{
	shader_ = new Shader;
	//因为obj模型原因此处使用normal作为color
	char vertShder[] = "#version 330 core \n"
		"layout(location = 0) in vec3 position;"
		"layout(location = 1) in vec4 vcolor;"

		"out vec4 ocolor;"
		"uniform mat4 view;"
		"uniform mat4 projection;"

		"void main()"
		"{"
		"	gl_Position = projection * view  * vec4(position, 1.0f);"
		"	ocolor = vcolor;"
		"}";

	char fragShader[] = "#version 330 core \n"
		"in vec4 ocolor;"
		"uniform sampler2D map;"
		"uniform vec2 g_move;"
		"out vec4 color;"
		"void main()"
		"{"
			"vec2 tex_coord = vec2(ocolor.x,ocolor.y) + g_move;"
			"color = texture(map,tex_coord);"
			"color.w = ocolor.w;"
		"}";

	shader_->loadShaderSouce(vertShder, fragShader, NULL);
	shader_->turnOn();
	GLint loc = shader_->getVariable("map");
	if (loc != -1)
		shader_->setInt(loc, 0);
}
