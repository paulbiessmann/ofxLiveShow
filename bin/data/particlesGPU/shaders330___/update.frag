#version 330

// ping pong inputs
uniform sampler2DRect particles0;
uniform sampler2DRect particles1;

uniform vec3 mouse;
uniform float radiusSquared;
uniform float elapsed;
uniform float audioEnergy;
uniform float centerForce;

in vec2 texCoordVarying;

layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;

float dx, dy, dz;


void main()
{
    vec3 pos = texture(particles0, texCoordVarying.st).xyz;
    vec3 vel = texture(particles1, texCoordVarying.st).xyz;
    
    //get closest
    
    
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
      //  force = vec3 (0,0,0);
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
    
    dx = (y- a*x +b*y*z) * elapsed * 0.1 ;//- audioEnergy * 0.1;
    dy = (c*y -x*z +z) * elapsed * 0.1; // - audioEnergy * 0.1;
    dz = (d*x*y - e*z ) * elapsed * 0.1;// - audioEnergy * 0.1;
    
    vec3 force2 = vec3(dx, dy, dz) ;
    
    if (centerForce > 0){
        direction = vec3(1, 1, 1) - pos.xyz;
        force +=  direction * centerForce;
    }
    
    // gravity
    force += vec3(0.0, 0.0, 0.0);
    
    
    // accelerate
    vel += elapsed * (force - force2 /2);
   // vel += elapsed * force2;
    
    
    // bounce off the sides
    vel.x *= step(abs(pos.x), 3000.0) * 2.0 - 1.0;
    vel.y *= step(abs(pos.y), 3000.0) * 2.0 - 1.0;
    vel.z *= step(abs(pos.z), 3000.0) * 2.0 - 1.0;
//    vel.z *= step(abs(pos.z), 1000.0) * 2.0 - 1.0;

//    if ( pos.x > 1000 && pos.x > 1000 && pos.z > 1000){
//        vel.xyz -= direction * centerForce;
//    }

    
    // damping
    vel *= 0.980;
    
    
    
    // move
    pos += elapsed * vel;
    
    posOut = vec4(pos, 1.0);
    velOut = vec4(vel, 0.0);
}

