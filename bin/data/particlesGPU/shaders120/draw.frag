#version 120
#extension GL_ARB_texture_rectangle : enable

void main()
{

    if (dot(gl_PointCoord - 0.5, gl_PointCoord - 0.5) > 0.55)       //makes points rounded
        discard;
    else
        
    gl_FragColor = vec4(1.0, 0.4, 1.0, 0.2);
        
}
