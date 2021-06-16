--VERTEX-sphere
#version 330 core

uniform mat4 view;
uniform mat4 projection; 

int g_SphereMeridianSlices = 128;
int g_SphereParallelSlices = 128;

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
    
    texCoord = vec2(meridianPart, parallelPart );
    gl_Position = projection * view * vec4(VertexPosition,1.0);
}
