#version 330

uniform mat4 modelViewProjectionMatrix;
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

in vec4  position;
in vec2  texcoord;

out vec2 texCoordVarying;
out vec4 vertColor;

void main()
{
    
    vertColor = vec4(0.5, 0.8, 0.5, 0.2);
    
    texCoordVarying = texcoord;
    gl_Position = modelViewProjectionMatrix * vec4(texture(particles0, texCoordVarying).xyz, 1.0);
    
   // gl_PointSize = 8.0;
}
