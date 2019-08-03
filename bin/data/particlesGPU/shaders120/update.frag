#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform vec3 mouse;
uniform float radiusSquared;
uniform float elapsed;
uniform float audioEnergy;
uniform float centerForce;
float dx, dy, dz;


void main()
{
    vec3 pos = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    vec3 vel = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
    
    // mouse attraction
    vec3 direction = mouse - pos.xyz;
    float distSquared = dot(direction, direction);
    float magnitude = 900.0 * (1.0 - distSquared / radiusSquared);
    vec3 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
    
    if (distSquared < 8000) {
        force = -force;
    }
    
    if (distSquared > 6000 && distSquared < 15000) {
        force = -force * distSquared * 0.000001;
    }
    
    
    // get the previous position
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    
    // Dedras
    float a = 3;//3;
    float b = 2.7;//2.7;
    float c = 1.7;
    float d = 2;
    float e = 9;//9;
    
    dx = (y- a*x +b*y*z) * elapsed * 0.1 ; //audioEnergy * 0.1;
    dy = (c*y -x*z +z) * elapsed * 0.1 ;// audioEnergy * 0.1;
    dz = (d*x*y - e*z ) * elapsed * 0.1 ;// audioEnergy * 0.1;
    
    vec3 force2 = vec3(dx, dy, dz) ;
    
    if (centerForce > 0){
        direction = vec3(1, 1, 1) - pos.xyz;
        force +=  direction * centerForce;
    }
    // gravity
    force += vec3(0.0, 0.0, 0.0);
    
    // accelerate
    vel += elapsed * force - (elapsed * force2 /10);
    vel += elapsed * (force);

    // bounce off the sides
    vel.x *= step(abs(pos.x), 800.0) * 2.0 - 1.0001;
    vel.y *= step(abs(pos.y), 800.0) * 2.0 - 1.0001;
    vel.z *= step(abs(pos.z), 800.0) * 2.0 - 1.0001;

    
    // damping
    vel *= 0.985;
    
    // move
    pos += elapsed * vel;
    
    gl_FragData[0] = vec4(pos, 1.0);
    gl_FragData[1] = vec4(vel, 0.0);
}
