--VERTEX-base
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

ivec2 heightMapHalfMap= ivec2(2048,2048);

uniform vec2 levelOffset; 							
uniform vec2 localOffset; 
uniform	float scale;  
 
uniform sampler2D heightmap;

out vec3 worldPosition;
out vec3 pcolor;
void main()
{

	vec3 xyz = ( model * vec4( position, 1.0 ) ).xyz * scale;
	
	vec2 localXZ = localOffset * scale + xyz.xz;
	
	vec2 worldXZ = levelOffset + localXZ;
	
	float height_sample = texelFetch( heightmap, ivec2( worldXZ ) + heightMapHalfMap, 0 ).r * 3.0;
	
	worldPosition = vec3(worldXZ.x,height_sample,worldXZ.y);
	
  gl_Position = projection * view * vec4(worldPosition,1.0);
  
  pcolor = color;
}

--FRAGMENT-base
#version 330 core

//uniform sampler2D diffmap;
in vec3 worldPosition;
in vec3 pcolor;
out vec4 color;
void main()
{
	 //vec3 diffColor = texelFetch(diffmap,ivec2(worldPosition.xy),0).rgb;
	
  color = vec4(pcolor, 0.8f);
 	//	color = vec4(diffColor,0.8);
}
