#include "skydom.h"
#include "helpF.h"
#include "type.h"
#include <log.h>

#define ZENITH_COLOR		{ 0.1f, 0.5f, 0.9f, 1.0f }
#define HORIZONTAL_COLOR	{ 0.8f, 0.9f, 0.9f, 1.0f }
extern const int  SKYDOME_SLICES = 20;
extern const int  SKYDOME_STACKS = 15;
extern const int  SKYDOME_RADIUS = 64.0f;

SkyDom::SkyDom()
{

}

SkyDom::~SkyDom()
{

}


struct LV
{
	V3f position_;
	V4f color_;
};

void SkyDom::init()
{
	int vertices_count = 1 + (SKYDOME_STACKS + 1) * SKYDOME_SLICES;
	trianglesCount_ = SKYDOME_SLICES + SKYDOME_STACKS * SKYDOME_SLICES * 2;

	LV * vert_buf = new LV[vertices_count];
	unsigned int * idx_buf = new unsigned int[trianglesCount_ * 3];

	LV * v = vert_buf;
	unsigned int * i = idx_buf;

	float radius = SKYDOME_RADIUS;
	V4f zenith_color = ZENITH_COLOR;
	V4f horizontal_color = HORIZONTAL_COLOR;
	V4f color_delta = horizontal_color - zenith_color;

	v->position_ = V3f(.0f,  radius, 0.0f);
	v->color_ = zenith_color;

	float half_rad = math::degreeToRadin(90.0f);
	float stack_step = half_rad / (float)SKYDOME_STACKS;
	float slice_step = Const<float>::pi() * 2.0f / (float)SKYDOME_SLICES;

	for (int stack = 0; stack <= SKYDOME_STACKS; ++stack) {
		float stack_angle = (stack + 1) * stack_step;

		float z = radius * cosf(stack_angle);
		float stack_radius = radius * sinf(stack_angle);

		if (stack == SKYDOME_STACKS) { // the last stack
			z = -radius * 0.5f;
			stack_radius = radius;
		}

		for (int slice = 0; slice < SKYDOME_SLICES; ++slice) {
			float slice_angle = slice * slice_step;

			float x = stack_radius * cosf(slice_angle);
			float y = stack_radius * sinf(slice_angle);

			v++;

			float factor = 1.0f;
			if (stack < SKYDOME_STACKS) {
				float persent = ((float)stack + 1.0f) / (float)SKYDOME_STACKS;
				factor = powf(persent, 2.0f); // as exponential increase
			}

			v->position_ = V3f( x,z,y);
			v->color_ = color_delta * factor + zenith_color;

			if (0 == stack) {// triangle fan
				i[0] = 0;
				i[1] = 1 + (slice + 1) % SKYDOME_SLICES;
				i[2] = 1 + slice;

				i += 3;
			}
			else { // triangle strip
				   /* indices

				   zenith
				   / \
				   0---3
				   | \ |
				   1---2
				   */

				int idx0 = 1 /* first zenith vertex */ + (stack - 1) * SKYDOME_SLICES /* stack offset */ + slice /* current slice */;
				int idx1 = 1 + stack * SKYDOME_SLICES + slice;
				int idx2 = 1 + stack * SKYDOME_SLICES + (slice + 1) % SKYDOME_SLICES;
				int idx3 = 1 + (stack - 1) * SKYDOME_SLICES + (slice + 1) % SKYDOME_SLICES;

				i[0] = idx0;
				i[1] = idx2;
				i[2] = idx1;
				i[3] = idx0;
				i[4] = idx3;
				i[5] = idx2;

				i += 6;
			}
		}
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * trianglesCount_ * 3, idx_buf,GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void SkyDom::render(const CameraBase*camera)
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
	glBindVertexArray(vertexBuffer_Ao_);
	glDrawElements(GL_TRIANGLES, trianglesCount_ * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	CHECK_GL_ERROR;
}

void SkyDom::initShader()
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
		"out vec4 color;"
		"void main()"
		"{"
		"color = ocolor;"
		"}";

	shader_->loadShaderSouce(vertShder, fragShader, NULL);
}
