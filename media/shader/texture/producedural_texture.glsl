--VERTEX-ver

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

uniform float texw, texh;
out vec2 dims;  // In absence of textureSize()
out vec2 one;
out vec2 st;

void main( void )
{
  st = texCoord;
  dims = vec2(texw, texh);
  one = 1.0 / dims; // Saves two divisions in the fragment shader
  gl_Position = projection * view * vec4(position,1.0);
}

--FRAGMENT-1
#version 330 core
uniform float time;
in vec2 dims;
in vec2 one;
in vec2 st;

void main( void )
{
  float pattern = smoothstep(0.3, 0.32, length(fract(5.0*st)-0.5));
  gl_FragColor = vec4(pattern,pattern,pattern, 1.0);
}

--FRAGMENT-2
#version 330 core
uniform float time;
in vec2 dims;
in vec2 one;
in vec2 st;

void main( void )
{
  float pattern = smoothstep(0.4, 0.5, 
  			max( abs(fract(8.0 *st.x - 0.5* mod(floor(8.0*st.y),2.0)) - 0.5),
  					 abs(fract(8.0*st.y) - 0.5))
  );
  gl_FragColor = vec4(pattern,pattern,pattern, 1.0);
}
