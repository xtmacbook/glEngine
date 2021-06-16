
#version 330 core 
layout(location = 0) in vec3 position;
		
out vec2 vTexCoord;
out vec2 vTexCoord2;
out vec3 w_position;

uniform vec2 tex;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 wp  = view * vec4(position,1.0f);
	w_position = wp.xyz;
	gl_Position = projection * wp;
		 	
	vTexCoord.x = position.x / tex.x;
	vTexCoord.y = 1.0f - position.z / tex.y;
		 	
	vTexCoord2.x = position.x / 16.0f;
	vTexCoord2.y = (tex.y - position.z) / 16.0f;
}
