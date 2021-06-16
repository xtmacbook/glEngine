
//https://github.com/martin-pr/possumwood/wiki/Geometry-Shader-Tessellation-using-PN-Triangles

#version 410 core

#define SUBDIV_LEVEL 15
#define MAX_VERTS 255 // arithmetic progression 15*(6+(15-1)*2)/2

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTS) out;

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

vec3 weights(int s, int t) {
	vec3 result;
	result.x = float(s) / float(SUBDIV_LEVEL);
	result.y = float(t) / float(SUBDIV_LEVEL);
	result.z = 1.0 - result.x - result.y;

	return result;
}

void emitVertex(int s, int t) {
	vec3 w = weights(s, t);

	vertexNormal = normalize(normal[0]*w[0] + normal[1]*w[1] + normal[2]*w[2]);
	vec3 vertexPosition = position[0]*w[0] + position[1]*w[1] + position[2]*w[2];
	vertexTexCoord = texCoord[0]*w[0] + texCoord[1]*w[1] + texCoord[2]*w[2];
	gl_Position = projection * vec4(vertexPosition, 1);

	EmitVertex();
}

void main() {
	for(int level = 0; level < SUBDIV_LEVEL; ++level) {
		polygonColour = colour(gl_PrimitiveIDIn*SUBDIV_LEVEL + level);

		for(int s = 0; s <= level; ++s) {
			emitVertex(s, level-s+1);
			emitVertex(s, level-s);
		}
		emitVertex(level+1, 0);

		EndPrimitive();
	}
}
