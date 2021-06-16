--TESS_TCL-0
layout (vertices = 1) out;  //one vertex per patch
in vec2 posV[];
out vec2 posTC[];

void main()
{
	posTC[gl_InvocationID] = posV[gl_InvocationID];
	gl_TessLevelOuter = ivec4 (64);
	gl_TessLevelInner = ivec4 (64);
}

--TESS_TES-0
layout (quads,fractional_even_spacing,cw) in;

uniform sampler2D heightMap;
uniform float heightStep; //scale factor
uniform float gridSpacing; // Units between two consecutive grid points
uniform int scaleFactor; //Number of height samples between two consecutive texture texels
uniform mat4 mvp;

in vec2 posTC[];
out vec2 uvTE; //texture coordinate

void main()
{
	ivec2 tSize = textureSize(heightMap,0) * scaleFactor;
}
