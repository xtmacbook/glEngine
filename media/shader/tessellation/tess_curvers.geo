#version 330 core

#define SUBDIV_LEVEL 15
#define MAX_VERTS 16 //  

layout(lines_adjacency) in;
layout(line_strip, max_vertices = MAX_VERTS) out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_four_bezier_basis_matrix; //u_
uniform bool beizer_line;
in vec3 vposition[]; 

vec3 decasteljau(vec3 P1, vec3 P2, vec3 P3, vec3 P4, float t) 
{ 
    // compute first tree points along main segments P1-P2, P2-P3 and P3-P4
    vec3 P12 = (1 - t) * P1 + t * P2; 
    vec3 P23 = (1 - t) * P2 + t * P3; 
    vec3 P34 = (1 - t) * P3 + t * P4; 
    // compute two points along segments P1P2-P2P3 and P2P3-P3P4
    vec3 P1223 = (1 - t) * P12 + t * P23; 
    vec3 P2334 = (1 - t) * P23 + t * P34; 
 
    // finally compute P
    return (1 - t) * P1223 + t * P2334; // P 
} 

vec3 bezier(vec3 P1, vec3 P2, vec3 P3, vec3 P4, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	vec4 vt = vec4(t3,t2,t,1);
	vec4 a = u_four_bezier_basis_matrix * vt  ;
	vec3 vertexPosition = P1 * a.x + P2 * a.y + P3 * a.z + P4 * a.w;
	return vertexPosition;
}

void emitVertex(float t,bool b) {
	vec3 vertexPosition;
	if(b)
	{
			vertexPosition = bezier(vposition[0],vposition[1],vposition[2],vposition[3],t);
	}
	else
	{
		vertexPosition = decasteljau(vposition[0],vposition[1],vposition[2],vposition[3],t);
	}
	gl_Position = projection * view * vec4(vertexPosition,1.0);
	
	EmitVertex();
}

void main() {
	 for(int level = 0; level < SUBDIV_LEVEL; ++level)
	 {
	 		float c = float(level);
	 		emitVertex( c / SUBDIV_LEVEL,beizer_line);
	 }
	 emitVertex(1.0f,beizer_line);
	 EndPrimitive();
}
