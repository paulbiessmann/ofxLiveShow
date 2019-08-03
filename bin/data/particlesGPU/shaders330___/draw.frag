#version 330

in vec4 vertColor;
out vec4 fragColor;


void main()
{
    if (dot(gl_PointCoord - 0.5, gl_PointCoord - 0.5) > 0.55)       //makes points rounded
        discard;
    else
        fragColor = vertColor;
    
    //fragColor = vec4(1.0, 0.3, 1.0, 0.1);
}
