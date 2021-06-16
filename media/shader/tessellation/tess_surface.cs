#version 410 core

layout(vertices = 16) out;
in vec3 vposition[];
in vec3 vnormal[]; 

out vec3 tcNormal[];

patch out mat4 cx, cy, cz;
uniform mat4 B, BT; //u_four_bezier_basis_matrix

void main()
{ 
    tcNormal[gl_InvocationID] = vnormal[gl_InvocationID];
    if (ID > 2) return;

    mat4 P;
    for (int idx = 0; idx < 16; ++idx) {
      P[idx/4][idx%4] = vposition[idx][gl_InvocationID];
    }

    P = B * P * BT;

    // Drivers have trouble with varying arrays, so...
    if (gl_InvocationID == 0) cx = P;
    if (gl_InvocationID == 1) cy = P;
    if (gl_InvocationID == 2) cz = P;
    
    gl_TessLevelInner[0] = gl_TessLevelInner[1] = 8;
    gl_TessLevelOuter[0] = gl_TessLevelOuter[1] = 8;
    gl_TessLevelOuter[2] = gl_TessLevelOuter[3] = 8;
}
