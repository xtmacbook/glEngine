
#ifndef light_h
#define light_h


#include "BaseObject.h"
#include "type.h"
#include "color.h"
#include "smartPointer.h"
#include "decl.h"

#include <string>

using namespace base;

class Shader;

struct LightProperties {
	bool	isEnabled_; // true to apply this light in this invocation
	bool	isLocal_; // true for a point light or a spotlight false for a positional light
	bool	isSpot_; // true if the light is a spotlight
	V3f		ambient_; // light¡¯s contribution to ambient light
	V3f		color_; // color of light
	V3f		position_; // location of light, if is Local is true, otherwise the direction toward the light
	V3f		halfVector_; // direction of highlights for directional light

	V3f		coneDirection_; // spotlight attributes
	float	spotCosCutoff_;
	float	spotExponent_;

	float	constantAttenuation_; // local light attenuation coefficients
	float	linearAttenuation_;
	float	quadraticAttenuation_;
	// other properties you may desire
};

struct Attenuation
{
	float constant_ = 1.0f;
	float linear_ = 0.0f;
	float quad_ = 0.0f;
} ;

/**
 * float specular = max(0.0, dot(Normal, HalfVector));
// surfaces facing away from the light (negative dot products)
// won¡¯t be lit by the directional light
	if (diffuse == 0.0)
		specular = 0.0;
	else
		specular = pow(specular, Shininess); // sharpen the highlight
	vec3 reflectedLight = LightColor * specular * Strength;

 */

enum LightType
{
	LIGHT_DIRECTION,
	LIGHT_SPOT,
	LIGHT_POINT
};

struct LIBENIGHT_EXPORT BaseLight: public base::BaseObject
{
	std::string name_ = "light";

	bool  isEnabled_; // true to apply this light in this invocation
	
	V3f   color_;
	V3f	  ambient_; // light¡¯s contribution to ambient light 

	float   ambientIntensity_;
	float   diffuseIntensity_;

	float strength_; //adjust shiness(mult the specular)
	float shininess_; // exponent for sharping highlights
	BaseLight()
	{
		isEnabled_ = true;
		ambientIntensity_ = diffuseIntensity_ = 1.0f;
		ambient_ = V3f(0.0f, 0.0f, 0.0f);
		strength_ = 1.0f;
		shininess_ = 32.0f;
	}
	LightType type_;
};

struct LIBENIGHT_EXPORT DirectionalLight : public BaseLight
{
	V3f		direction_;
	V3f		halfVector_; // direction of highlights for directional light
	DirectionalLight()
	{
		direction_ = V3f(0.0f, 0.0f, 0.0f);
		halfVector_ = V3f(0.0f, 0.0f, 0.0f);
		type_ = LIGHT_DIRECTION;
	}
};

struct LIBENIGHT_EXPORT PointLight : public BaseLight
{
	V3f position_;
	Attenuation atten_;
	PointLight()
	{
		position_ = V3f(0.0f, 0.0f, 0.0f);
		type_ = LIGHT_POINT;
	}
};

struct LIBENIGHT_EXPORT SpotLight : public PointLight
{
	V3f   direction_;
	float spotCutoff_;
	float spotExponent_; 
	SpotLight()
	{
		direction_ = V3f(0.0f, 0.0f, -1.0f);
		spotCutoff_ = 180.0f;
		spotExponent_ = 0.0;
		type_ = LIGHT_SPOT;
	}
};


typedef base::SmartPointer<BaseLight>   Light_SP;
typedef base::SmartPointer<DirectionalLight>  DLight_SP;
typedef base::SmartPointer<PointLight>  PLight_SP;
typedef base::SmartPointer<SpotLight>  SLight_SP;


struct LIBENIGHT_EXPORT BaseLightLocation
{
	int enable_;

	int color_;
	int ambient_;

	int strength_;
	int shiness_;
	
	int ambientIntensity_;
	int diffuseIntensity_;
};

struct LIBENIGHT_EXPORT DirectionLightLocation :public BaseLightLocation
{
	int direction_;
	int halfVec_;
} ;

struct  LIBENIGHT_EXPORT AttenLocation
{
	int constant_;
	int linear_;
	int quat_;
};

struct LIBENIGHT_EXPORT PointLightLocation :public BaseLightLocation
{
	int position_;
	AttenLocation atten_;
} ;

struct LIBENIGHT_EXPORT SpotLightLocation :public PointLightLocation
{
	int direction_;
	int spotCutOff_;
	int spotExponent_;
} ;


void LIBENIGHT_EXPORT applyBaseLight(const BaseLight& light, Shader * shader, const BaseLightLocation&);
void LIBENIGHT_EXPORT applyPointLight( const PointLight& light, Shader * shader, const PointLightLocation&);
void LIBENIGHT_EXPORT applyDirectionLight(const DirectionalLight&light, Shader*shader, const DirectionLightLocation&);
void LIBENIGHT_EXPORT applySpointLight(const SpotLight&light, Shader*shader, const SpotLightLocation&);

void  LIBENIGHT_EXPORT getPointLightShaderLocation(int idx, const Shader*shader, PointLightLocation&);
void  LIBENIGHT_EXPORT getSpotLightShaderLocation(int idx, const Shader*shader, SpotLightLocation&);
void  LIBENIGHT_EXPORT getDirectionLightShaderLocation(int idx, const Shader*shader, DirectionLightLocation&);

#endif
