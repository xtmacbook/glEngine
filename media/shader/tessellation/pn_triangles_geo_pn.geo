#version 410 core

// 1 = no subdivision (pass-thru)
// 2 = 4x subdiv
// 3 = 9x subdiv
// 4 = 16x subdiv
// ...

//#define SUBDIV_LEVEL 1
//#define MAX_VERTS 3 // 3

//#define SUBDIV_LEVEL 2
//#define MAX_VERTS 8 // 3+5

#define SUBDIV_LEVEL 15
#define MAX_VERTS 255 // arithmetic progression 15*(6+(15-1)*2)/2

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTS) out;

uniform mat4 view;
uniform mat4 projection;

in vec3 normal[];
in vec3 position[];
in vec2 texCoord[];

out vec3 vertexNormal;
out vec3 vertexPosition;
out vec2 vertexTexCoord;

struct Coeffs {
	vec3 b300, b030, b003,
		b210, b120, b021, b012, b102, b201,
		b111;
};

vec3 weights(int s, int t) {
	vec3 result;
	result.x = float(s) / float(SUBDIV_LEVEL);
	result.y = float(t) / float(SUBDIV_LEVEL);
	result.z = 1.0 - result.x - result.y;

	return result;
}

void emitVertex(int s, int t, Coeffs c) {
	vec3 w = weights(s, t);

	vertexPosition =
		c.b300 * pow(w[2], 3) + c.b030 * pow(w[0], 3) + c.b003*pow(w[1], 3) +
		c.b210*3*w[2]*w[2]*w[0] + c.b120*3*w[2]*w[0]*w[0] + c.b201*3*w[2]*w[2]*w[1] +
		c.b021*3*w[0]*w[0]*w[1] + c.b102*3*w[2]*w[1]*w[1] + c.b012*3*w[0]*w[1]*w[1] +
		c.b111*6*w[0]*w[1]*w[2];

	w = weights(s, t);

	vertexNormal = normalize(normal[0]*w[2] + normal[1]*w[0] + normal[2]*w[1]); //this is linear normal variation

	vertexTexCoord = texCoord[0]*w[2] + texCoord[1]*w[0] + texCoord[2]*w[1];

	gl_Position = projection * view * vec4(vertexPosition, 1);

	EmitVertex();
}

void main() {
	Coeffs c;
	c.b300 = position[0];
	c.b030 = position[1];
	c.b003 = position[2];

	vec3 n[3];
	n[0] = normalize(normal[0]);
	n[1] = normalize(normal[1]);
	n[2] = normalize(normal[2]);

	float w12 = dot(position[1] - position[0], n[0]);
	float w21 = dot(position[0] - position[1], n[1]);
	float w23 = dot(position[2] - position[1], n[1]);
	float w32 = dot(position[1] - position[2], n[2]);
	float w31 = dot(position[0] - position[2], n[2]);
	float w13 = dot(position[2] - position[0], n[0]);

	c.b210 = (2*position[0] + position[1] - w12*n[0]) / 3.0;
	c.b120 = (2*position[1] + position[0] - w21*n[1]) / 3.0;
	c.b021 = (2*position[1] + position[2] - w23*n[1]) / 3.0;
	c.b012 = (2*position[2] + position[1] - w32*n[2]) / 3.0;
	c.b102 = (2*position[2] + position[0] - w31*n[2]) / 3.0;
	c.b201 = (2*position[0] + position[2] - w13*n[0]) / 3.0;

	vec3 E = (c.b210 + c.b120 + c.b021 + c.b012 + c.b102 + c.b201) / 6.0;
	vec3 V = (position[0] + position[1] + position[2]) / 3.0;
	c.b111 = E + (E - V) / 2.0;

	for(int level = 0; level < SUBDIV_LEVEL; ++level) {
		for(int s = 0; s <= level; ++s) {
			emitVertex(s, level-s+1, c);
			emitVertex(s, level-s, c);
		}
		emitVertex(level+1, 0, c);

		EndPrimitive();
	}
}
