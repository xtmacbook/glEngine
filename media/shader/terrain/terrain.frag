#version 330 core

in vec2 vTexCoord;
in vec2 vTexCoord2;
in vec3 w_position;

out vec4 color;

uniform sampler2D g_hightmap;
uniform sampler2D g_detailmap;

uniform vec4 g_fog;

void main()
{
		vec4 flog_color = vec4(g_fog.xyz,1.0f);
		float	fog_density = g_fog.w;
		
		vec4 base_color =  texture(g_hightmap, vTexCoord);  
		vec4 detail_color =  texture(g_detailmap, vTexCoord2);
		
		vec4	combined_color = mix(base_color, detail_color, 0.5f);
	
		float distance = length(w_position);
		float visibility = exp(-pow((distance * fog_density), 0.5f));
		visibility = clamp(visibility,0.0,1.0);

		float a = 1.0 - visibility;
		color = mix(combined_color, flog_color, a);
}
