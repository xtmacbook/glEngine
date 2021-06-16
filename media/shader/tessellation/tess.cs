#version 410 core


//TCS invocation is only responsible for producing a single vertex of output to the output patch. 
//All inputs from vertex shaders to the TCS are aggregated into arrays, based on the size of the input patch. 
//The size of these arrays is the number of input patches provided by the patch primitive. 


//define the number of CPs in the output patch
 layout (vertices = 3) out; //output patches with a particular vertex count

 uniform vec3 gEyeWorldPos;

 // more than one CP in the input and output patches each attribute is defined using the array modifier []. 
 // attributes of the input CPs

 in vec3 WorldPos_CS_in[];
 in vec2 TexCoord_CS_in[];
 in vec3 Normal_CS_in[];

 //These can be per-vertex outputs and per-patch outputs to TES
 // attributes of the output CPs
 out vec3 WorldPos_ES_in[];
 out vec2 TexCoord_ES_in[];
 out vec3 Normal_ES_in[];

float GetTessLevel(float Distance0, float Distance1)
 {
     float AvgDistance = (Distance0 + Distance1) / 2.0;

     if (AvgDistance <= 2.0) 
     {
         return 10.0;
     }
     else if (AvgDistance <= 5.0) 
     {
         return 7.0;
     }
     else 
     {
         return 3.0;
     }
 }

//                         v1
//                         /\
//                        /  \
//                  edge0/    \edge2
//                      /      \
//                     /        \
//                    /          \
//                  v2------------ v0
//                          edge1                              

 void main()
 {
     // Set the control points of the output patch
     //gl_InvocationID is the index of the output patch vertex 
     TexCoord_ES_in[gl_InvocationID] = TexCoord_CS_in[gl_InvocationID];
     Normal_ES_in[gl_InvocationID] = Normal_CS_in[gl_InvocationID];
     WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];

      // Calculate the distance from the camera to the three control points
     float EyeToVertexDistance0 = distance(gEyeWorldPos, WorldPos_ES_in[0]);
     float EyeToVertexDistance1 = distance(gEyeWorldPos, WorldPos_ES_in[1]);
     float EyeToVertexDistance2 = distance(gEyeWorldPos, WorldPos_ES_in[2]);

     // Calculate the tessellation levels
     //gl_TessLevelOuter[] roughly determines the number of segments on each edge 
     //gl_TessLevelInner[0] roughly determines how many rings the triangle will contain(just for the domain  is triangle)
     //for the domain  is quads the in0   = horizontal tessellation,in1   = vertical tessellation 
    //http://ogldev.atspace.co.uk/www/tutorial30/tutorial30.html

     gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
     gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
     gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
     gl_TessLevelInner[0] = gl_TessLevelOuter[2];

     //in case of quad, you have to specify both gl_TessLevelInner[1] and //gl_TessLevelOuter[3]
 }
