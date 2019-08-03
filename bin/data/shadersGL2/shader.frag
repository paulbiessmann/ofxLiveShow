#version 120

uniform sampler2DRect tex0;

varying vec2 texCoordVarying;

uniform vec3 sphereColor;

void main()
{
    
//    gl_FragColor = texture2DRect(tex0, texCoordVarying);
    
    vec4 textureColor = texture2DRect(tex0 , texCoordVarying);
    
    textureColor[0] *= sphereColor[0];
//    textureColor[1] *= sphereColor[1];
//    textureColor[2] *= sphereColor[2];
    textureColor[2] = sphereColor[2];
    
    
    
    gl_FragColor =   textureColor ;

}
