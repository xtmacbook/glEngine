#version 330 core

in vec2 texCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;
void main()
{
    vec3 baseColor = texture(texture_diffuse1,texCoords).xyz;

    color = vec4(baseColor, 1.0f);
}
