#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 vnormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat3 normalMatrix;

out vec3  normal;
out vec3  eyeDir;
out float lightIntensity;

uniform vec3  lightPos;

void main(void)
{
   	gl_Position = projection * view * vec4(position,1.0);
    normal         = normalize(normalMatrix * vnormal);
    vec4 pos       = view * vec4(position,1.0);
    eyeDir         = pos.xyz;
    lightIntensity = max(dot(normalize(lightPos - eyeDir), normal), 0.0);
}
