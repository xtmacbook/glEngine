#version 330 core

out vec4 color;

in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec3 polygonColour;

uniform bool showMatrial;
uniform sampler2D gColorMap0;   

void main() {
	vec3 norm = normalize(vertexNormal);
	float val = abs(norm.z);
	if(showMatrial)
	{
		color = texture(gColorMap0, vertexTexCoord.xy);
	}
	else
		color = vec4(polygonColour * val, 1);
}
