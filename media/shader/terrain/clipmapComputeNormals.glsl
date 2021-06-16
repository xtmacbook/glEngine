--VERTEX-base
#version 330 core
layout(location = 0) in vec2 position;

out vec2 fsTextureCoordinates;

uniform mat4 viewportOrthographicMatrix;
uniform vec2 updateSize;
uniform vec2 origin;
uniform vec2 oneOverHeightMapSize;
uniform mat4 model;

void main()                     
{
    vec2 coordinates = position * updateSize + origin;
    gl_Position = viewportOrthographicMatrix * vec4(coordinates, 0.0, 1.0);
    fsTextureCoordinates = coordinates * oneOverHeightMapSize;
}
--FRAGMENT-base
#version 330 core

in vec2 fsTextureCoordinates;
in vec2 fsPosition;
out vec3 normalOutput;

uniform float heightExaggeration;
uniform float postDelta;
uniform vec2 oneOverHeightMapSize;
uniform sampler2D texture0;

void main()
{
    float top = texture(texture0, fsTextureCoordinates + vec2(0.0, oneOverHeightMapSize.y)).r * heightExaggeration;
    float bottom = texture(texture0, fsTextureCoordinates + vec2(0.0, -oneOverHeightMapSize.y)).r * heightExaggeration;
    float left = texture(texture0, fsTextureCoordinates + vec2(-oneOverHeightMapSize.x, 0.0)).r * heightExaggeration;
    float right = texture(texture0, fsTextureCoordinates + vec2(oneOverHeightMapSize.x, 0.0)).r * heightExaggeration;

    normalOutput = vec3(left - right, bottom - top, 2.0 * postDelta);
}
