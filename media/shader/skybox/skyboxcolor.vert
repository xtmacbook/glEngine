#version 330 core
precision mediump float;

layout(location = 0) in vec4 a_vPosition;   
// OUTPUT
out vec3 v_vCubemapCoord;

// UNIFORMS

uniform mat4 projection;   
uniform mat4 verseViewMat; 
void main()
{
    gl_Position = a_vPosition;
    // Force all depths to just inside the far clip plane. If we put z=0.999
    // in the vertex attributes, it messes up the cube projection below.
    // Nobbling it here is easier.
    gl_Position.z = 0.9999;

    vec4 vPos = projection * a_vPosition;
    vPos /= vPos.w;

    v_vCubemapCoord = (mat3(verseViewMat) * vPos.xyz);
}
