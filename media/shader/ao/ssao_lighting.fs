#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

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
                                                                                        
uniform PointLight gPointLights[1];

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    vec3 lighting  = ambient; 
    vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)
    // diffuse
    vec3 lightDir = normalize(gPointLights[0].Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * gPointLights[0].Base.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    vec3 specular = gPointLights[0].Base.Color * spec;
    // attenuation
    float distance = length(gPointLights[0].Position - FragPos);
    float attenuation = 1.0 / (1.0 + gPointLights[0].Atten.Linear * distance + gPointLights[0].Atten.Exp * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}
