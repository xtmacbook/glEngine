#version 330 core

out vec3 normal;
out vec3 position;
out vec2 texCoord;

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 t;

uniform mat4 model;


void main() 
{
	normal = (model * vec4(n.x, n.y, n.z, 0)).xyz;
	position = (model * vec4(p.x, p.y, p.z, 1)).xyz;
	texCoord = t;
}
