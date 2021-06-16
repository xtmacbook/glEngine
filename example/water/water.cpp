#include "water.h"
#include "timerManager.h"
#include "shader.h"
#include "resource.h"
#include "IGeometry.h"
#include "log.h"
#include "camera.h"
#include "helpF.h"
#include "gls.h"
#include <algorithm>

// math constants //
#define EPSILON 1.0e-8
#define DELTA 1.0e-1

class WaterShader :public Shader
{
public:
	WaterShader(const char*v,const char*f,const char*g = nullptr);

	~WaterShader();

	//attribute
	GLuint m_APosition;
	GLuint m_ANormal;

	GLuint m_vmpTransform;

	GLuint m_WaterColor_loc;
	GLuint m_USpecularColor_loc;
	GLuint m_UAmbientColor_loc;
	GLuint m_UDiffuseColor_loc;

	GLuint m_UAmbientFactor_loc;
	GLuint m_USpecularFactor_loc;
	GLuint m_USpecularExponent_loc; 

	GLuint m_ULightDirection_loc;
	GLuint m_UCameraPosition_loc;

	static V3f m_WaterColor;
	static V3f m_AmbientColor;
	static V3f m_DiffuseColor;
	static V3f m_SpecularColor;

	static float m_AmbientFactor;
	static float m_SpecularFactor;

	static int m_SpecularExponent;
};

V3f WaterShader::m_WaterColor		= V3f(0.0f, 0.5f, 1.0f);
V3f WaterShader::m_AmbientColor		= V3f(0.160f, 0.909f, 0.960f);
V3f WaterShader::m_DiffuseColor		= V3f(0.160f, 0.909f, 0.960f);
V3f WaterShader::m_SpecularColor	= V3f(1.0f, 1.0f, 1.0f);

float WaterShader::m_AmbientFactor  = 0.2f;
float WaterShader::m_SpecularFactor = 1.0f;
int   WaterShader::m_SpecularExponent = 20;

WaterShader::WaterShader(const char*v, const char*f, const char*g /*= nullptr*/):
	Shader(v,f,g)
{

}

WaterShader::~WaterShader()
{

}
 

class WaterGemoetry : public CommonGeometry
{
public:

	WaterGemoetry(Water*,WaterShader*);

	~WaterGemoetry();
	virtual void updateGeometry();
	virtual void initGeometry();
	virtual void drawGeoemtry(const DrawInfo&);
private:

	unsigned		num_indices_;

	Water*			water_;
	WaterShader*	shader_;
	GLuint			position_vbo_;
	GLuint			element_vbo_;
	GLuint			normal_vbo_;
};

void WaterGemoetry::initGeometry()
{
	std::vector<Water::Triangle> indices;
	unsigned limit = water_->num_verts_ - water_->xStride_;
	for (unsigned int i = 0; i < limit;) {
		indices.push_back(Water::Triangle(i, i + 1, i + water_->xStride_));
		++i;
		indices.push_back(Water::Triangle(i, i + water_->xStride_, i + water_->xStride_ - 1));
		unsigned vertices_left = (i + 1) % water_->xStride_;
		if (vertices_left == 0)
			++i;
	}

	num_indices_ = indices.size() * 3;

	bindVAO();
	glGenBuffers(1, &position_vbo_);
	glGenBuffers(1, &element_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_vbo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Water::Triangle) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// buffer data
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Water::Vertex) * water_->vertexData_.size(), water_->vertexData_.data(), GL_STREAM_DRAW);
	

	// attributes
	glVertexAttribPointer(shader_->m_APosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(shader_->m_APosition);
	glVertexAttribPointer(shader_->m_ANormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
				(void *)(3 * water_->num_verts_ * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(shader_->m_ANormal);
	// unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR;
	
}

void WaterGemoetry::drawGeoemtry(const DrawInfo&di)
{
	GLint  location = di.draw_shader_->getVariable("model");
	if (location != -1)
	{
		Matrixf transform = di.matrix_* getModelMatrix();
		di.draw_shader_->setMatrix4(location, 1, GL_FALSE, math::value_ptr(transform));
	}
	//update data
	glBindBuffer(GL_ARRAY_BUFFER, position_vbo_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Water::Vertex) * water_->vertexData_.size(), water_->vertexData_.data());
	
	bindVAO();
	glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_INT, nullptr);
	unBindVAO();
	CHECK_GL_ERROR;
}

WaterGemoetry::~WaterGemoetry()
{
		
}


void WaterGemoetry::updateGeometry()
{

}

WaterGemoetry::WaterGemoetry(Water*w, WaterShader*s):water_(w),
shader_(s),CommonGeometry(false)
{

}

Water::Water(unsigned x_stride, unsigned z_stride) :
	xStride_(x_stride),
	zStride_(z_stride),
	num_verts_(xStride_ * zStride_)
{

}

void Water::setStride(unsigned x, unsigned z)
{
	xStride_ = x;
	zStride_ = z;
	num_verts_ = (xStride_ * zStride_);
}

void Water::prepareGridData()
{
	vertexData_.clear();
	vertexData_.reserve(num_verts_);

	float x_min = 0.0f;
	float x_max = xStride_ - 1.0f;
	float z_min = 0.0f;
	float z_max = zStride_ - 1.0f;
	
	float x = x_min;
	float y = 0.0;
	float z = z_min;
	for (unsigned i = 0; i < num_verts_; ++i) {
		// Epsilon to avoid floating point error
		if (x > (x_max + EPSILON)) {
			x = x_min;
			z += 1.0f;
		}
		vertexData_.push_back(Vertex(x, y, z));
		x += 1.0f;
	}
	for (unsigned i = 0; i < num_verts_; ++i) {
		vertexData_.push_back(Vertex(0.0,1.0,0.0));
	}
}

void Water::prepareShader()
{
	shader_ = new WaterShader(std::string(get_shader_BasePath() + "water/water.vert").c_str(),
		std::string(get_shader_BasePath() + "water/water.frag").c_str());
	if (shader_)
	{
		shader_->m_APosition		= shader_->getAttribute("APosition");
		shader_->m_ANormal			= shader_->getAttribute("ANormal");

		shader_->m_vmpTransform		= shader_->getVariable("vmp");

		shader_->m_WaterColor_loc		= shader_->getVariable("UWaterColor");
		shader_->m_USpecularColor_loc   = shader_->getVariable("USpecularColor");
		shader_->m_UAmbientColor_loc	= shader_->getVariable("UAmbientColor");
		shader_->m_UDiffuseColor_loc	= shader_->getVariable("UDiffuseColor");

		shader_->m_USpecularFactor_loc	= shader_->getVariable("USpecularFactor");
		shader_->m_UAmbientFactor_loc   = shader_->getVariable("UAmbientFactor");

		shader_->m_USpecularExponent_loc = shader_->getVariable("USpecularExponent");

		shader_->m_ULightDirection_loc	= shader_->getVariable("ULightDirection");

		shader_->m_UCameraPosition_loc	= shader_->getVariable("UCameraPosition");
		
	}
	else
	{
		Log::instance()->printMessage("water shader create error \n");
	}
}

void Water::initGeometry()
{
	geometry_ = new WaterGemoetry(this,shader_);
	geometry_->initGeometry();
}

void Water::removeWave(Wave *w)
{

}

void Water::addWave(Wave w)
{
	waves_.push_back(w);
}

void Water::update()
{
	unsigned  stride_index = 0;

	for (unsigned x = 0; x < xStride_; x++)
	{
		for (unsigned z = 0; z < zStride_; z++)
		{
			std::pair<V3f, V3f> offset;

			getSumWave(x, z, TimeManager::instance().getApplicationDeltaTime(), offset);

			//set the new position
			vertexData_[stride_index].X_ = (float) x +  offset.first.x;
			vertexData_[stride_index].Y_ = offset.first.y;
			vertexData_[stride_index].Z_ = (float) z +  offset.first.z;

			//set the new normal
			unsigned normal_index = stride_index + num_verts_;
			vertexData_[normal_index].X_ = -offset.second.x;
			vertexData_[normal_index].Y_ =  offset.second.y;
			vertexData_[normal_index].Z_ = -offset.second.z;

			stride_index++;
		}
	}

}

void Water::getSumWave(float x, float z, float t, std::pair<V3f, V3f>&full_offset)
{
	full_offset.first  = V3f(0.0f, 0.0f, 0.0f);
	full_offset.second = V3f(0.0f, 0.0f, 0.0f);

	std::for_each (waves_.cbegin(),waves_.cend(),[&full_offset,x,z,t](const Wave&wave)
    {
        if (wave.active_)
            {
                std::pair<V3f, V3f> offset = wave.updateGerstner(x, z, t);
                full_offset.first += offset.first;
                full_offset.second += offset.second;
            }

    });

}

void Water::render(CameraBase * camera)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_->turnOn();

	Matrixf vmp =  camera->getProjectionMatrix() * camera->getViewMatrix();
	shader_->setMatrix4(shader_->m_vmpTransform, 1, GL_FALSE, math::value_ptr(vmp));
	
	shader_->setFloat3(shader_->m_WaterColor_loc, WaterShader::m_WaterColor.x, WaterShader::m_WaterColor.y, WaterShader::m_WaterColor.z);
	shader_->setFloat3(shader_->m_UAmbientColor_loc, WaterShader::m_AmbientColor.x, WaterShader::m_AmbientColor.y, WaterShader::m_AmbientColor.z);
	shader_->setFloat3(shader_->m_USpecularColor_loc, WaterShader::m_SpecularColor.x, WaterShader::m_SpecularColor.y, WaterShader::m_SpecularColor.z);
	shader_->setFloat3(shader_->m_UDiffuseColor_loc, WaterShader::m_DiffuseColor.x, WaterShader::m_DiffuseColor.y, WaterShader::m_DiffuseColor.z);
	
	V3f camerapos = camera->getPosition();
	shader_->setFloat3(shader_->m_UCameraPosition_loc,camerapos.x,camerapos.y,camerapos.z);
	DrawInfo di;
	di.draw_shader_ = shader_;
	geometry_->drawGeoemtry(di);

	shader_->turnOff();
}

Water * WaterEnditer::m_Water = nullptr;

bool WaterEnditer::m_ChangingWaterColor = false;
bool WaterEnditer::m_ChangingLightColor = false;
bool WaterEnditer::m_Opening = false;

void WaterEnditer::SetWater(Water * water)
{
	m_Water = water;
}

void WaterEnditer::DisplayEditor()
{
	if (!m_Water) 
	{
		Log::instance()->printMessage("water don't exist int waterEnditer !\n");
		return;
	}

	//bool show = true;
	//ImGui::Begin("WaterEnditor", &show, ImGuiWindowFlags_MenuBar);
	//
	////meunbar
	//MenuBar();

	////wave
	//for (Wave & wave : m_Water->waves_)
	//	DisplayWave(wave);

	//ImGui::End();

	if(m_ChangingWaterColor)
		ChangeWaterColor();
	if(m_ChangingLightColor)
		ChangeLightColor();

}

WaterEnditer::WaterEnditer()
{

}

void WaterEnditer::MenuBar()
{
	/*if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open"))
				m_Opening = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Color")) 
		{
			if (ImGui::MenuItem("Water"))
				m_ChangingWaterColor = true;
			if (ImGui::MenuItem("Light"))
				m_ChangingLightColor = true;
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}*/
}

void WaterEnditer::DisplayWave(Wave & wave)
{
	/*if (ImGui::TreeNode(wave.name_.c_str())) {
		ImGui::Checkbox("Active", &wave.active_);

		ImGui::DragFloat("Amplitude", &wave.amplitude_, 0.05f, 0.0f);
		ImGui::DragFloat("Steepness", &wave.steepness_, 0.01f);

		float new_length = wave.length_;
		ImGui::DragFloat("Length", &new_length, 0.05f, 0.0f);
		wave.setWaveLength(new_length);

		float new_speed = wave.speed_;
		ImGui::DragFloat("Speed", &new_speed, 0.05f, 0.0f);
		wave.setSpeed(new_speed);

		float new_direction = wave.directionRadians_;
		ImGui::DragFloat("Direction", &new_direction, 0.01f);
		wave.setWaveDirection(new_direction);

		if (ImGui::Button("Remove"))
			m_Water->removeWave(&wave);

		ImGui::TreePop();
	}*/
}

void WaterEnditer::ChangeWaterColor()
{
	/*ImGui::Begin("Water Color");
	ChangeColor(&WaterShader::m_WaterColor);
	if (ImGui::Button("Done"))
		m_ChangingWaterColor = false;
	ImGui::End();*/
}

void WaterEnditer::ChangeLightColor()
{
	/*ImGui::Begin("Light Color");
	if (ImGui::TreeNode("Ambient")) 
	{
		ImGui::SliderFloat("Factor", &WaterShader::m_AmbientFactor, 0.0f, 1.0f);
		ImGui::Text("Color");
		ChangeColor(&WaterShader::m_AmbientColor);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Diffuse")) 
	{
		ImGui::Text("Color");
		ChangeColor(&WaterShader::m_DiffuseColor);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Specular")) 
	{
		ImGui::SliderFloat("Factor", &WaterShader::m_SpecularFactor, 0.0f, 1.0f);
		ImGui::SliderInt("Exponent", &WaterShader::m_SpecularExponent, 0, 100);
		ImGui::Text("Color");
		ChangeColor(&WaterShader::m_SpecularColor);
	}

	if (ImGui::Button("Done"))
		m_ChangingLightColor = false;
	ImGui::End();*/

}

void WaterEnditer::ChangeColor(glm::vec3 * color)
{
	//ImGui::SliderFloat("R", &(color->r), 0.0f, 1.0f);
	//ImGui::SliderFloat("G", &(color->g), 0.0f, 1.0f);
	//ImGui::SliderFloat("B", &(color->b), 0.0f, 1.0f);
}

void WaterEnditer::EditWaveName()
{

}
