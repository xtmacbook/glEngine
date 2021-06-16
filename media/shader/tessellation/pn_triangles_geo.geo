#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out; //layout(outpu_primitive,max_vertices)

uniform mat4 projection;

in vec3 normal[];
in vec3 position[];
in vec2 texCoord[];

out vec3 vertexNormal;
out vec2 vertexTexCoord;
out vec3 polygonColour;


vec3 colour(int id) {
	return vec3(
		float(id % 4) / 3.0,
		float((id/4) % 4) / 3.0,
		float((id/16) % 4) / 3.0
	);
}

void main() {
	for(int i = 0; i < 3; i++) 
	{
		vertexNormal = normal[i];
		vertexTexCoord = texCoord[i];
		polygonColour = colour(gl_PrimitiveIDIn + 1);
		gl_Position = projection * vec4(position[i], 1);
		EmitVertex();
	}
	EndPrimitive();
}
