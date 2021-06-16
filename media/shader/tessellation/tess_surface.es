#version 410 core

layout(quad, equal_spacing, ccw) in;

in vec3 tcNormal[];

uniform mat4 view;
uniform mat4 projection;

patch out mat4 cx, cy, cz;

//Oregon State University (Mike Bailey :Tessllation Shaders)

void main()
{
	 float u = gl_TessCoord.x;
	 float v = gl_TessCorrd.y;
	 
	 vec4 U = vec4(u * u * u, u * u, u , 1);
	 vec4 V = vec4(v * v * v, v * v, v , 1);
	 
	 float x = dot(cx * V, U);
   float y = dot(cy * V, U);
   float z = dot(cz * V, U);
   
   gl_Position = projection * view * vec4(x,y,z,1.0);
}
