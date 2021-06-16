--VERTEX-Compiled
#version 330 core

uniform mat4 view;
uniform mat4 projection; 

uniform vec3 g_EyePosition;
uniform vec3 g_LightPosition;

uniform int g_SphereMeridianSlices;
uniform int g_SphereParallelSlices;

out vec2 texCoord;
out vec3 viewVectorTangent;
out vec3 lightVectorTangent;

void main()
{
    
    float meridianPart = ( gl_VertexID % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( gl_VertexID / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    
    float angle1 = meridianPart * 3.14159265 * 2.0;
    float angle2 = ( parallelPart - 0.5) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    vec3 VertexPosition;
  
    VertexPosition.x = sin_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    VertexPosition.z = cos_angle1 * cos_angle2;
    
    texCoord = vec2(meridianPart,1 -  parallelPart );
    gl_Position = projection * view * vec4(VertexPosition,1.0);
    
  vec3 tangent = vec3(cos_angle1,0.0,-sin_angle1);
  vec3 binormal = vec3(-sin_angle2 * sin_angle1,-cos_angle2,-cos_angle1 * sin_angle2);
  
  vec3 viewnormal = normalize(g_EyePosition - VertexPosition);
    
  viewVectorTangent.x =  dot(viewnormal,tangent);
  viewVectorTangent.y =  dot(viewnormal,binormal);
  viewVectorTangent.z =  dot(viewnormal,viewnormal);
    
  vec3 lightVector = normalize( g_LightPosition );
    
    lightVectorTangent.x = dot( lightVector, tangent );
    lightVectorTangent.y = dot( lightVector, binormal);
    lightVectorTangent.z = dot( lightVector, VertexPosition );
    
}



--FRAGMENT-trilinear
#version 330 

#define MAX_ANISOTROPY 16
#define MIP_LEVELS_MAX 7

uniform sampler2D PyramidTexture;
uniform sampler2D PyramidTextureHM;
uniform sampler2DArray StackTexture;

uniform float g_ScreenAspectRatio;

uniform ivec2 g_TextureSize;
uniform vec2 g_StackCenter;
uniform uint g_StackDepth;
uniform vec2 g_ScaleFactor;

uniform vec3 g_WorldRight;
uniform vec3 g_WorldUp;

uniform vec3 g_MipColors[MIP_LEVELS_MAX];
uniform float test_;

out vec4 color;
in vec2 texCoord;
in vec3 viewVectorTangent;
in vec3 lightVectorTangent;

int getMinimumStackLevel(vec2 coordinates)
{
    vec2 distance;
    
    distance.x = abs( coordinates.x - g_StackCenter.x );
    distance.x = min( distance.x, 1.0 - distance.x );
    
    distance.y = abs( coordinates.y - g_StackCenter.y );
    distance.y = min( distance.y, 1.0 - distance.y );
            
    return int(max( log2( distance.x * g_ScaleFactor.x * 4.2 ), log2( distance.y * g_ScaleFactor.y * 4.2 ) ));
}

void main()
{
  vec2 pixelCoord = vec2(texCoord.x * g_TextureSize.x, texCoord.y * g_TextureSize.y );
    vec2 dx = dFdx( pixelCoord );
    vec2 dy = dFdy( pixelCoord );
    float d = max( length( dx ), length( dy ) );
     
    float mipLevel = max( log2( d ), getMinimumStackLevel( texCoord ) );
    float blendGlobal = clamp(float(g_StackDepth) - mipLevel,0.0,1.0);
            
     
    float diffuse = clamp( lightVectorTangent.z,0.0,1.0 );
    diffuse = max( diffuse, 0.5 );
    
    vec4 color0 = texture(PyramidTexture, texCoord );
    
    // Make early out for cases where we don't need to fetch from clipmap stack
    if( blendGlobal == 0.0 )
    {
        if(test_ == 0.0)
          color = vec4(1.0,0.0,0.0,1.0);
        else
          color = color0 * diffuse; 
    }
    else
    {
    
    float blendLayers = modf( mipLevel, mipLevel );
        blendLayers = clamp(blendLayers,0.0,1.0);
        
        float nextMipLevel = mipLevel + 1.0;
        float isd = float(g_StackDepth) - 1.0;
        nextMipLevel = clamp( nextMipLevel, 0.0, isd );
        mipLevel = clamp( mipLevel, 0.0, isd );
            
        vec2 clipTexCoord = texCoord / pow( 2, mipLevel );
        clipTexCoord *= g_ScaleFactor;
        vec4 color1 = texture( StackTexture, vec3( clipTexCoord + 0.5, mipLevel ) );
            
        clipTexCoord = texCoord / pow( 2, nextMipLevel );
        clipTexCoord *= g_ScaleFactor;
        vec4 color2 = texture( StackTexture, vec3( clipTexCoord + 0.5, nextMipLevel ) );
        
        color = mix( color0, mix( color1, color2, blendLayers ), blendGlobal ) * diffuse;
     }
}
