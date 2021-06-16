--VERTEX-Quad
#version 330 core

out vec2 texCoord;
void main()
{
   gl_Position = vec4(0.0,0.0,0.0,0.0);
   texCoord = vec2(0.0,0.0);
}

--GEOMETRY-Quad
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2 texCoord[];
out vec2 texCoords;
void main()
{
	gl_Position = vec4( -1.0,1.0,0.5,1.0);
	texCoords = vec2( 0.0, 1.0 );
    EmitVertex();
    
    gl_Position = vec4( -1.0,-1.0,0.5,1.0);
	texCoords = vec2( 0.0, 0.0 );
    EmitVertex();
    
    gl_Position = vec4( 1.0,1.0,0.5,1.0);
	texCoords = vec2( 1.0, 1.0 );
    EmitVertex();
    
    gl_Position = vec4( 1.0,-1.0,0.5,1.0);
	texCoords = vec2( 1.0, 0.0 );
    EmitVertex();
    
    EndPrimitive();
}

--GEOMETRY-GL_Quad
#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec2 texCoord[];
out vec2 texCoords;
void main()
{
	gl_Position = vec4( -1.0,1.0,0.5,1.0);
	texCoords = vec2( 0.0, 0.0 );
    EmitVertex();
    
    gl_Position = vec4( -1.0,-1.0,0.5,1.0);
	texCoords = vec2( 0.0, 1.0 );
    EmitVertex();
    
    gl_Position = vec4( 1.0,1.0,0.5,1.0);
	texCoords = vec2( 1.0, 0.0 );
    EmitVertex();
    
    gl_Position = vec4( 1.0,-1.0,0.5,1.0);
	texCoords = vec2( 1.0, 1.0 );
    EmitVertex();
    
    EndPrimitive();
}

--FRAGMENT-Quad
#version 330 
out vec4 color;
in vec2 texCoords;
void main()
{
    color = vec4(0.5,1.0,0.1,1.0f);
}

--FRAGMENT-PS_IDCT_Rows
#version 330 
layout (location = 0) out mediump vec4 color0;
layout (location = 1) out mediump vec4 color1;
in vec2 texCoords;
uniform sampler2D TextureDCT;
uniform sampler2D QuantTexture;

uniform float g_ColScale;

void main()
{
		float d[8];
	// Read row elements
	d[0] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(-4, 0)).r;
	d[1] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(-3, 0)).r;
	d[2] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(-2, 0)).r;
	d[3] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(-1, 0)).r;
	d[4] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(0, 0)).r;
	d[5] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(1, 0)).r;
	d[6] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(2, 0)).r;
	d[7] = 128.0 * textureOffset(TextureDCT,texCoords, ivec2(3, 0)).r;

	// Perform dequantization
	d[0] *= texture(QuantTexture, vec2(0.0625, texCoords.y * g_ColScale)).r * 256.0f;
	d[1] *= texture(QuantTexture, vec2(0.1875, texCoords.y * g_ColScale)).r * 256.0f;
	d[2] *= texture(QuantTexture, vec2(0.3125, texCoords.y * g_ColScale)).r * 256.0f;
	d[3] *= texture(QuantTexture, vec2(0.4375, texCoords.y * g_ColScale)).r * 256.0f;
	d[4] *= texture(QuantTexture, vec2(0.5625, texCoords.y * g_ColScale)).r * 256.0f;
	d[5] *= texture(QuantTexture, vec2(0.6875, texCoords.y * g_ColScale)).r * 256.0f;
	d[6] *= texture(QuantTexture, vec2(0.8125, texCoords.y * g_ColScale)).r * 256.0f;
	d[7] *= texture(QuantTexture, vec2(0.9375, texCoords.y * g_ColScale)).r * 256.0f;
	
	float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
	float tmp10, tmp11, tmp12, tmp13;
	float z5, z10, z11, z12, z13;

	tmp0 = d[0];
	tmp1 = d[2];
	tmp2 = d[4];
	tmp3 = d[6];

	tmp10 = tmp0 + tmp2;
	tmp11 = tmp0 - tmp2;

	tmp13 = tmp1 + tmp3;
	tmp12 = (tmp1 - tmp3) * 1.414213562 - tmp13;

	tmp0 = tmp10 + tmp13;
	tmp3 = tmp10 - tmp13;
	tmp1 = tmp11 + tmp12;
	tmp2 = tmp11 - tmp12;

	tmp4 = d[1];
	tmp5 = d[3];
	tmp6 = d[5];
	tmp7 = d[7];

	z13 = tmp6 + tmp5;
	z10 = tmp6 - tmp5;
	z11 = tmp4 + tmp7;
	z12 = tmp4 - tmp7;

	tmp7 = z11 + z13;
	tmp11 = (z11 - z13) * 1.414213562;

	z5 = (z10 + z12) * 1.847759065;
	tmp10 = 1.082392200 * z12 - z5;
	tmp12 = -2.613125930 * z10 + z5;

	tmp6 = tmp12 - tmp7;
	tmp5 = tmp11 - tmp6;
	tmp4 = tmp10 + tmp5;

	
	color0.x = tmp0 + tmp7;
	color0.y = tmp1 + tmp6;
	color0.z = tmp2 + tmp5;
	color0.w = tmp3 - tmp4;
	
	
	color1.x = tmp3 + tmp4;
	color1.y = tmp2 - tmp5;
	color1.z = tmp1 - tmp6;
	color1.w = tmp0 - tmp7;
}

--FRAGMENT-PS_IDCT_Unpack_Rows
#version 330 
layout (location = 0) out mediump float color;
in vec2 texCoords;

uniform float g_RowScale;
uniform sampler2D RowTexture1;
uniform sampler2D RowTexture2;

void main()
{
	  vec4 values1 = texture(RowTexture1,texCoords);
    vec4 values2 = texture(RowTexture2,texCoords);
        
    // Calculate a single non-zero index to define an element to be used
    int iindex = int(fract( texCoords.x * g_RowScale ) * 8.0);
    
    vec4 indexMask1 = vec4(float(iindex == 0), float(iindex == 1),float(iindex == 2),float(iindex == 3) );
    vec4 indexMask2 = vec4(float(iindex == 4), float(iindex == 5),float(iindex == 6),float(iindex == 7) );
    
    float c = dot( values1, indexMask1 ) + dot( values2, indexMask2 );
    
    color = c;
}
--FRAGMENT-PS_IDCT_Columns
#version 330 
layout (location = 0) out mediump vec4 color0;
layout (location = 1) out mediump vec4 color1;
in vec2 texCoords;

uniform sampler2D TargetTexture;
void main()
{
    float d[8];
    
    // Read column elements
    d[0] = textureOffset( TargetTexture, texCoords, ivec2( 0, -4 ) ).r;
    d[1] = textureOffset( TargetTexture, texCoords, ivec2( 0, -3 ) ).r;
    d[2] = textureOffset( TargetTexture, texCoords, ivec2( 0, -2 ) ).r;
    d[3] = textureOffset( TargetTexture, texCoords, ivec2( 0, -1 ) ).r;
    d[4] = textureOffset( TargetTexture, texCoords, ivec2( 0, 0 ) ).r;
    d[5] = textureOffset( TargetTexture, texCoords, ivec2( 0, 1 ) ).r;
    d[6] = textureOffset( TargetTexture, texCoords, ivec2( 0, 2 ) ).r;
    d[7] = textureOffset( TargetTexture, texCoords, ivec2( 0, 3 ) ).r;
    
    float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    float tmp10, tmp11, tmp12, tmp13;
    float z5, z10, z11, z12, z13;
    
    tmp0 = d[0];
    tmp1 = d[2];
    tmp2 = d[4];
    tmp3 = d[6];

    tmp10 = tmp0 + tmp2;
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;
    tmp12 = (tmp1 - tmp3) * 1.414213562 - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;
    
    tmp4 = d[1];
    tmp5 = d[3];
    tmp6 = d[5];
    tmp7 = d[7];

    z13 = tmp6 + tmp5;
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;
    tmp11 = (z11 - z13) * 1.414213562;

    z5 = (z10 + z12) * 1.847759065;
    tmp10 = 1.082392200 * z12 - z5;
    tmp12 = -2.613125930 * z10 + z5;

    tmp6 = tmp12 - tmp7;
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    color0.x = tmp0 + tmp7;
    color1.w = tmp0 - tmp7;
    color0.y = tmp1 + tmp6;
    color1.z = tmp1 - tmp6;
    color0.z = tmp2 + tmp5;
    color1.y = tmp2 - tmp5;
    color1.x = tmp3 + tmp4;
    color0.w = tmp3 - tmp4;

}

--FRAGMENT-PS_IDCT_Unpack_Columns
#version 330 

layout (location = 0) out mediump float color;

in vec2 texCoords;

uniform float g_ColScale;

uniform sampler2D ColumnTexture1;
uniform sampler2D ColumnTexture2;
void main()
{
    
    // Get eight values storded in 2 textures
    vec4 values1 = texture( ColumnTexture1,texCoords );
    vec4 values2 = texture( ColumnTexture2, texCoords );
    
     // Calculate a single non-zero index to define an element to be used
    int index = int(fract( texCoords.y * g_ColScale ) * 8.0);
    
    vec4 indexMask1 = vec4(float(index == 0), float(index == 1),float(index == 2),float(index == 3) );
    vec4 indexMask2 = vec4(float(index == 4), float(index == 5),float(index == 6),float(index == 7) );
    
    float c = dot( values1, indexMask1 ) + dot( values2, indexMask2 );
    
	float d = clamp( c  * 0.125 + 128.0, 0.0, 256.0 );
		
    color = d;
}

--FRAGMENT-PS_IDCT_RenderToBuffer
#version 330 
layout (location = 0) out mediump vec4 color;

in vec2 texCoords; 

uniform sampler2D TextureY;
uniform sampler2D TextureCb;
uniform sampler2D TextureCr;

uniform sampler2D TextureHeight;

void main()
{
	float Y = texture( TextureY,texCoords).r;
    float Cb = texture( TextureCb, texCoords ).r;
    float Cr = texture( TextureCr, texCoords ).r;
        
    // Convert YCbCr -> RGB
    color.x = Y + 1.402 * ( Cr - 128.0 );
    color.y = Y - 0.34414 * ( Cb - 128.0 ) - 0.71414 * ( Cr - 128.0 );
    color.z = Y + 1.772 * ( Cb - 128.0 );
    color.w = texture( TextureHeight, texCoords).r;
        
    color.xyzw *= ( 1.0 / 256.0 );
}
