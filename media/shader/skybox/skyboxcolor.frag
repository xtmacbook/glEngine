
#version 330 core

precision mediump float;

// INPUT
in vec3 v_vCubemapCoord;

// OUTPUT
out vec4 o_vFragColor;

uniform samplerCube skybox; 

void main()
{
    o_vFragColor = texture(skybox, v_vCubemapCoord);
}
