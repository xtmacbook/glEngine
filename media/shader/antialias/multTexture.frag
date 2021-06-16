
#version 330 core

uniform sampler2DMS screenTextureMS;

in vec2 TexCoords;

out vec4 outColor;
void main()
{
    vec4 colorSample ;

    //for(int i =0;i < 4;i++)
        colorSample = texelFetch(screenTextureMS,ivec2(TexCoords),3); //4th subsample

    outColor = colorSample;
}
