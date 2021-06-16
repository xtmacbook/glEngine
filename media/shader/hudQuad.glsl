--VERTEX-hud
#version 330 core

out vec3 position;
out vec3 texCoord;
void main()
{
	position = vec3(0.0,0.0,0.0);
	texCoord = vec3(0.0,0.0,1.0);
}

--GEOMETRY-hud
#version 330 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec3 texCoords;

uniform float sizeY;
uniform float sizeX;
uniform float offsetX;
uniform float offsetY;
uniform float stackZ;

void main()
{
	
	gl_Position.z = 0.5;
	gl_Position.w = 1.0;
 	texCoords.z = stackZ;
	
	gl_Position.x = offsetX;
	gl_Position.y = offsetY;
	texCoords.xy = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position.x = offsetX;
	gl_Position.y = offsetY - sizeY;
	texCoords.xy = vec2(0.0, 0.0);
	EmitVertex();

	gl_Position.x = offsetX + sizeX;
	gl_Position.y = offsetY;
	texCoords.xy = vec2(1.0, 1.0);
	EmitVertex();

	gl_Position.x = offsetX + sizeX;
	gl_Position.y = offsetY - sizeY;
	texCoords.xy = vec2(1.0, 0.0);
	EmitVertex();

	EndPrimitive();

}

--FRAGMENT-hud
#version 330 
out vec4 color;
in vec3 texCoords;
uniform sampler2DArray StackTexture;
void main()
{
    color = texture( StackTexture, texCoords );
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
