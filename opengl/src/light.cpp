#include "light.h"
#include "shader.h"
#include "helpF.h"
#include "util.h"
#include "gls.h"

#include <string>


void  applyBaseLight(const BaseLight& light, Shader * shader, const BaseLightLocation&location)
{
	if (location.enable_ != -1) shader->setInt(location.enable_, light.isEnabled_);

	if (location.color_ != -1)  shader->setFloat3(location.color_, light.color_.x, light.color_.y, light.color_.z);
	if (location.ambient_ != -1) shader->setFloat3(location.ambient_, light.ambient_.x, light.ambient_.y, light.ambient_.z);

	if (location.ambientIntensity_ != -1) shader->setFloat(location.ambientIntensity_, light.ambientIntensity_);
	if (location.diffuseIntensity_ != -1) shader->setFloat(location.diffuseIntensity_, light.diffuseIntensity_);

	if (location.shiness_ != -1) shader->setFloat(location.shiness_, light.shininess_);
	if (location.strength_ != -1) shader->setFloat(location.strength_, light.strength_);
}

void applyPointLight(const PointLight& pLights, Shader * shader, const PointLightLocation&pointLightsLocation)
{
	applyBaseLight(pLights, shader, pointLightsLocation);

	if (pointLightsLocation.position_ != -1)
		shader->setFloat3(pointLightsLocation.position_, pLights.position_.x, pLights.position_.y, pLights.position_.z);

	if (pointLightsLocation.atten_.constant_ != -1) shader->setFloat(pointLightsLocation.atten_.constant_, pLights.atten_.constant_);
	if (pointLightsLocation.atten_.linear_ != -1) shader->setFloat(pointLightsLocation.atten_.linear_, pLights.atten_.linear_);
	if (pointLightsLocation.atten_.quat_ != -1) shader->setFloat(pointLightsLocation.atten_.quat_, pLights.atten_.quad_);
}

void applyDirectionLight(const DirectionalLight&light, Shader*shader, const DirectionLightLocation&location)
{
	applyBaseLight(light, shader, location);
	V3f Direction = light.direction_; math::normalizeVec3(Direction);
	if (location.direction_ != -1)  shader->setFloat3(location.direction_, Direction.x, Direction.y, Direction.z);
	if (location.halfVec_ != -1)  shader->setFloat3(location.halfVec_, light.halfVector_.x, light.halfVector_.y, light.halfVector_.z);
}

void applySpointLight(const SpotLight&light, Shader*shader, const SpotLightLocation&sl)
{
	applyPointLight(light, shader, sl);

	if (sl.direction_ != -1) shader->setFloat3(sl.direction_, light.direction_.x, light.direction_.y, light.direction_.z);
	if (sl.spotCutOff_ != -1) shader->setFloat(sl.spotCutOff_, light.spotCutoff_);
	if (sl.spotExponent_ != -1) shader->setFloat(sl.spotExponent_, light.spotExponent_);
}

void getPointLightShaderLocation(int i, const Shader*shader, PointLightLocation&pointLightsLocation)
{
	GLint loc = -1;
	char Name[128];
	memset(Name, 0, sizeof(Name));

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.IsEnable", i);
	pointLightsLocation.enable_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
	pointLightsLocation.color_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.Ambient", i);
	pointLightsLocation.ambient_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
	pointLightsLocation.ambientIntensity_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
	pointLightsLocation.diffuseIntensity_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.Strength", i);
	pointLightsLocation.strength_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Base.Shiness", i);
	pointLightsLocation.shiness_ = shader->getVariable(Name);

	///////////////////////////////////////////////////////////////////////////////

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Position", i);
	pointLightsLocation.position_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
	pointLightsLocation.atten_.constant_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
	pointLightsLocation.atten_.linear_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
	pointLightsLocation.atten_.quat_ = shader->getVariable(Name);
}

void getSpotLightShaderLocation(int i, const Shader*shader, SpotLightLocation&spotLightsLocation)
{
	GLint loc = -1;
	char Name[128];
	memset(Name, 0, sizeof(Name));

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.IsEnable", i);
	spotLightsLocation.enable_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
	spotLightsLocation.color_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Ambient", i);
	spotLightsLocation.ambient_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
	spotLightsLocation.ambientIntensity_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
	spotLightsLocation.diffuseIntensity_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Strength", i);
	spotLightsLocation.strength_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Shiness", i);
	spotLightsLocation.shiness_ = shader->getVariable(Name);

	//////////////////////////////////////////////////////////////////////////////
	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Position", i);
	spotLightsLocation.position_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
	spotLightsLocation.atten_.constant_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
	spotLightsLocation.atten_.linear_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
	spotLightsLocation.atten_.quat_ = shader->getVariable(Name);

	//////////////////////////////////////////////////////////////////////////////

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
	spotLightsLocation.direction_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
	spotLightsLocation.spotCutOff_ = shader->getVariable(Name);

	SNPRINTF(Name, sizeof(Name), "gSpotLights[%d].Exponent", i);
	spotLightsLocation.spotExponent_ = shader->getVariable(Name);
}

void getDirectionLightShaderLocation(int idx, const Shader*shader, DirectionLightLocation&dll)
{
	dll.enable_ = shader->getVariable("gDirectionalLight.Base.IsEnable");

	dll.color_ = shader->getVariable("gDirectionalLight.Base.Color");
	dll.ambient_ = shader->getVariable("gDirectionalLight.Base.Ambient");

	dll.ambientIntensity_ = shader->getVariable("gDirectionalLight.Base.AmbientIntensity");
	dll.diffuseIntensity_ = shader->getVariable("gDirectionalLight.Base.DiffuseIntensity");

	dll.strength_ = shader->getVariable("gDirectionalLight.Base.Strength");
	dll.shiness_ = shader->getVariable("gDirectionalLight.Base.Shiness");

	dll.direction_ = shader->getVariable("gDirectionalLight.Direction");
	dll.halfVec_ = shader->getVariable("gDirectionalLight.HalfVec");
}
