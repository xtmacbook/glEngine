
#version 330

//from opengl shading language :Stored Texture Shaders
const vec3 Xunitvec = vec3(1.0, 0.0, 0.0);
const vec3 Yunitvec = vec3(0.0, 1.0, 0.0);

uniform vec3 baseColor;
uniform sampler2D envMap;
uniform float exposure;

float mixRatio = 0.2;

in vec3 normal;

in vec3  eyeDir;
in float lightIntensity;

float pi = 3.1415926;

float gamma = 2.2;

out vec4 color;

void mainx()
{
    vec2 uv;
    uv.x = atan( normal.z, normal.x );
    uv.y = acos( normal.y );
    uv /= vec2( 2 * pi, pi );

    vec3 envColor = vec3(texture(envMap, uv));

    // Add lighting to base color and mix
    vec3 base = lightIntensity * baseColor;
    envColor = mix(envColor, base, mixRatio);

    color = vec4(envColor, 1.0);
}

void main()
{
    vec3 reflectDir = reflect(eyeDir, normal);

    // Compute altitude and azimuth angles
    vec2 index;

    index.t = dot(normalize(reflectDir), Yunitvec);
    reflectDir.y = 0.0;
    index.s = dot(normalize(reflectDir), Xunitvec) * 0.5;

    // Translate index values into proper range

    if (reflectDir.z >= 0.0)
        index = (index + 1.0) * 0.5;
    else
    {
        index.t = (index.t + 1.0) * 0.5;
        index.s = (-index.s) * 0.5 + 1.0;
    }
 

    vec3 envColor = vec3(texture(envMap, index));

    envColor = vec3(1.0) - exp(-envColor * exposure);
    
    envColor = pow(envColor, vec3(1.0 / gamma));

    vec3 base = lightIntensity * baseColor;
    envColor = mix(envColor, base, mixRatio);

    color = vec4(envColor, 1.0);
}
