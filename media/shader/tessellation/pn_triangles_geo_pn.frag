#version 330 core

out vec4 color;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform bool showMatrial;
uniform sampler2D gColorMap0;   

uniform vec3 gEyeWorldPos;   

const int MAX_POINT_LIGHTS = 2;                                                             
const int MAX_SPOT_LIGHTS = 2;                                                              
                                                                                            
struct BaseLight                                                                            
{   
    bool  IsEnable;
    vec3  Color;                                                                             
    vec3  Ambient;
    float AmbientIntensity;                                                                 
    float DiffuseIntensity; 
    float Strength;
    float Shiness;                                                                
};                                                                                          
                                                                                            
struct DirectionalLight                                                                     
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Direction;
    vec3 HalfVec;                                                                         
};                                                                                          
                                                                                            
struct Attenuation                                                                          
{                                                                                           
    float Constant;                                                                         
    float Linear;                                                                           
    float Exp;                                                                              
};                                                                                          
                                                                                            
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};                                                                                          
                                                                                            
struct SpotLight                                                                            
{                                                                                           
    PointLight Base;                                                                 
    vec3 Direction;                                                                         
    float Cutoff;
    float Exponent;                                                                           
};                                                                                          
             

uniform int gNumPointLights;                                                                
uniform int gNumSpotLights;       

uniform DirectionalLight gDirectionalLight;                                                 
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];       

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)            
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - vertexPosition);                        
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                              
        if (SpecularFactor > 0) {                                                           
            SpecularFactor = pow(SpecularFactor, Light.Shiness);                               
            SpecularColor = vec4(Light.Color * Light.Strength * SpecularFactor, 1.0f);
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + DiffuseColor + SpecularColor);                                   
}                                                                                           
                                                                                            
vec4 CalcDirectionalLight(vec3 Normal)                                                      
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  
}                                                                                           
                                                                                            
vec4 CalcPointLight(PointLight l, vec3 Normal)                                       
{                                                                                           
    vec3 LightDirection = vertexPosition - l.Position;                                      
    float Distance = length(LightDirection);                                                
    LightDirection = normalize(LightDirection);                                             
                                                                                            
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);                         
    float Attenuation =  l.Atten.Constant +                                                 
                         l.Atten.Linear * Distance +                                        
                         l.Atten.Exp * Distance * Distance;                                 
                                                                                            
    return Color / Attenuation;                                                             
}                                                                                           
                                                                                            
vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                         
{                                                                                           
    vec3 LightToPixel = normalize(vertexPosition - l.Base.Position);                        
    float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
    if (SpotFactor > l.Cutoff) {                                                            
        vec4 Color = CalcPointLight(l.Base, Normal);                                        
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
    }                                                                                       
    else {                                                                                  
        return vec4(0,0,0,0);                                                               
    }                                                                                       
}                                                                                           
                                                                                            

void main() {
    
    vec3 rcolor;
    if(showMatrial)
    {
        
        vec4 TotalLight = CalcDirectionalLight(vertexNormal);                                         
                                                                                            
        for (int i = 0 ; i < gNumPointLights ; i++) {                                           
            TotalLight += CalcPointLight(gPointLights[i], vertexNormal);                              
        }                                                                                       
                                                                                            
        for (int i = 0 ; i < gNumSpotLights ; i++) {                                            
            TotalLight += CalcSpotLight(gSpotLights[i], vertexNormal);                                
        }                                                              

		rcolor = (texture(gColorMap0, vertexTexCoord.xy) * TotalLight).xyz;
    }
    else
    {
 		vec3 norm = normalize(cross(dFdx(vertexPosition), dFdy(vertexPosition)));
   		 float val = abs(norm.z);
   		 rcolor = vec3(val,val,val);
    }
   
    color = vec4(rcolor, 1);
}
