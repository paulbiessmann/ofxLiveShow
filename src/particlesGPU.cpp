//
//  particlesGPU.cpp
//  Enlivenment_Main
//
//  Created by Paul Bießmann on 14.07.19.
//

#include <stdio.h>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setupParticlesGPU()
{
    // sound
    fftLive.setup();
    fftLive.setMirrorData(true);
    //#
    
    
    ofHideCursor();
    ofSetBackgroundColor(0);
    ofSetFrameRate(60);
    
    // 1,000,000 particles
    unsigned w = 450;
    unsigned h = 450;
    
    particles.init(w, h);
    
   
    if (ofIsGLProgrammableRenderer()) particles.loadShaders("particlesGPU/shaders330/update", "particlesGPU/shaders330/draw");
    else particles.loadShaders("particlesGPU/shaders120/update", "particlesGPU/shaders120/draw");
    
    // initial positions
    // use new to allocate 4,000,000 floats on the heap rather than
    // the stack
    float* particlePosns = new float[w * h * 4];
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            unsigned idx = y * w + x;
            particlePosns[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
            particlePosns[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
            particlePosns[idx * 4 + 2] = 0.f; // particle z
            particlePosns[idx * 4 + 3] = 0.f; // dummy
        }
    }
    particles.loadDataTexture(ofxGpuParticles::POSITION, particlePosns);
    delete[] particlePosns;
    
    // initial velocities
    particles.zeroDataTexture(ofxGpuParticles::VELOCITY);
    
    // listen for update event to set additonal update uniforms
    ofAddListener(particles.updateEvent, this, &ofApp::onParticlesUpdate);
}

//--------------------------------------------------------------
void ofApp::updateParticlesGPU()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    particles.update();
    
    //MIDI
    
    if(midiMessage.control == 7){ // fader 7 Time Speed
       timeSpeed  = ofMap(midiMessage.value, 0, 127, 0.0, 2.0);
    }
    
}

// set any update uniforms in this function
void ofApp::onParticlesUpdate(ofShader& shaderParticles)
{
    //
   // fftLive.update();
    
//    // Sound -----
//    int numOfVerts = 512;
//
//    float * audioData = new float[numOfVerts];
//    fftLive.getFftPeakData(audioData, numOfVerts);
//    float audioEnergy = 0;
//
//    for (int i = 0; i < numOfVerts; i++){
//        audioEnergy += audioData[i];
//    }
//    audioEnergy *= 10;
    
    // cout << "FFT " << audioEnergy << "\n";
    //
    float elapsed;
    elapsed = ofGetLastFrameTime() * timeSpeed;
    

        // elapsed = 1;
        // cam.orbit(ofGetElapsedTimef() * 2, 1000, 1000);

    
    
    ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY() , 0.f);
    shaderParticles.setUniform3fv("mouse", mouse.getPtr());
    shaderParticles.setUniform1f("elapsed", elapsed);
    shaderParticles.setUniform1f("radiusSquared", 500.f * 500.f);
//    shaderParticles.setUniform1f("audioEnergy", audioEnergy);
    
    //    if (bCenterForce){
    //        centerForce = 10;
    //    }else {
    //        centerForce = 0;
    //    }
   // cout << "centerF " << centerForce << "\n";
    shaderParticles.setUniform1f("centerForce", centerForce);
    
    
    
//    delete [] audioData;
    
}

//--------------------------------------------------------------
void ofApp::drawParticlesGPU()
{
    
    if(bAutoCam){
        camDistSpeed = cam.getDistance();
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        cam.orbit(camRotSpeed , abs(camRotSpeed/3), camDistSpeed, ofVec3f(0,0,0) );
        cam.getTarget();
        
    }
    
    
    cam.begin();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    particles.draw();
    ofDisableBlendMode();
    cam.end();
    
    
}
