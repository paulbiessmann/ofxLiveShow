//
//  cloudsFbm.cpp
//  ClusterDuo_v1-1_Main
//
//  Created by Paul Bießmann on 19.07.19.
//

#include <stdio.h>
#include "ofApp.h"

float specSmoothed[256];          //Smoothed spectrum values


void ofApp::setupCloudsFbm(){
    // load the shader
    
    shaderFbmNoise.load("shader_fbm_noise");
    
    bAutoCam        = false;
    
    glitchFbm       = 0.1;
    lacunarityFbm   = 0.1;
    gainFbm         = 0.4;
    
    cloudTimeX      = 0.01;
    cloudTimeY      = 0.01;
    timeSpeedX      = 0.01;
    timeSpeedY      = 0.01;
    
    audioReaction   = 0.0;
    
    ofSetBackgroundAuto(true);
    
    fftLive.setMaxDecay(0.995);
    fftLive.setPeakDecay(0.995);
    
}

void ofApp::updateCloudsFbm(){
    //MIDI
    if(midiMessage.control == 0){ // fader 0 audio Reaction value
        audioReaction  = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    if(midiMessage.control == 4){ // fader 5 gain Fbm
        gainFbm  = ofMap(midiMessage.value, 0, 127, 0.4, 0.9);
    }
    
    if(midiMessage.control == 5){ // fader 6 lacunarity
        lacunarityFbm  = ofMap(midiMessage.value, 0, 127, 0.1, 0.5);
    }
    if(midiMessage.control == 6){ // fader 7 glitch
        glitchFbm  = ofMap(midiMessage.value, 0, 127, 0.1, 1.7);
    }
    
    if(midiMessage.control == 23){ // rad 8 : speed X
        timeSpeedX = ofMap(midiMessage.value, 0, 127, 0.15, -0.15);
    }
    if(midiMessage.control == 7){ // fader 8 time speed Y
        timeSpeedY  = ofMap(midiMessage.value, 0, 127, -0.15, 0.15);
    }
    
   
   
}

void ofApp::drawCloudsFbm(){
    
    // sound
    
    // Sound -----
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    for ( int i=0; i<numVertsGlobal; i++ ) {
        specSmoothed[i] *= 0.98;    //Slow decreasing 0.97
        specSmoothed[i] = max( specSmoothed[i], audioData[i] );
    }
    
    for ( int i=0; i<numVertsGlobal; i++ ) {
        specSmoothed[i] *= 1.05;    //Slow increasing 0.97
        specSmoothed[i] = min( specSmoothed[i], audioData[i] );
    }

    
    float audioEnergy = 0;
    
    int numUsedBands = numOfVerts / 10;
    
    for (int i = 0; i < numUsedBands; i++){
        audioEnergy += specSmoothed[i];
    }
    audioEnergy /= numUsedBands;
   
    //cout << "audio Rea 1 : " << audioReaction << "\n";
    
    audioEnergy *= audioReaction;
    
  //  cout << "audio E 2 : " << audioEnergy << "\n";
    
    shaderFbmNoise.begin();
    
    //lacunarityFbm = ofMap(mouseX, 0, ofGetWidth(), 0.3, 0.7 + audioEnergy);
   // lacunarityFbm = ofMap(mouseX, 0, ofGetWidth(), 0.1, 0.5 + audioEnergy);
    
    float f     =   ofGetElapsedTimef();
    
    uniVFbm = ofMap(mouseY, 0, ofGetWidth(), 0, 1);
    
   // glitchFbm = 3.0;
    
    float glitchNow  =  glitchFbm * (audioEnergy + 0.1) + 3 ;
    
    cloudTimeX   += timeSpeedX;
    cloudTimeY   += timeSpeedY;

    
    shaderFbmNoise.setUniform1f("u_gain", gainFbm);
    shaderFbmNoise.setUniform1f("u_glitchFbm", glitchNow);
    shaderFbmNoise.setUniform1f("u_uniV", uniVFbm);
    shaderFbmNoise.setUniform1f("u_lancidings", lacunarityFbm + audioEnergy);
    shaderFbmNoise.setUniform1f("u_time_x", cloudTimeX);
    shaderFbmNoise.setUniform1f("u_time_y", cloudTimeY);
    shaderFbmNoise.setUniform2f("u_resolution", ofGetWidth(), ofGetHeight());

    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    
    shaderFbmNoise.end();
    
    delete [] audioData;
    
}
