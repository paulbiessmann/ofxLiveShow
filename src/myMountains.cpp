//
//  myMountains.cpp
//  ClusterDuo_v1-1_Main
//
//  Created by Paul Bießmann on 19.07.19.
//

#include <stdio.h>
#include "ofApp.h"


void ofApp::setupMountains(){
    noiseMaxMountains = 0.5;
    ofSetBackgroundAuto(false);
    

    
    for(int a = 0; a < TWO_PI*40; a+=1) {
        
        ofVec3f p;
        
        p.x = ofRandom(ofGetWidth());
        p.y = ofRandom(ofGetHeight());
        p.z = ofRandom(500);
        
        addPointMountains(p.x, p.y, p.z);
        
    }
    
    mountainMoveX = 0;
    mountainMoveY = 0;
    mountainX = 0;
    mountainY = 0;
    
    pointsMountains.clear();
    sizesMountains.clear();

}

void ofApp::updateMountains(){
    
    //MIDI
    if(midiMessage.control == 0){ // fader 1 audio Reaction value
        audioReaction  = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    
    
    noiseMaxMountains = ((float)mouseX / 2000) + 0.1;
    phaseMountains += (float)mouseY / 4000;
    
    
    if(midiMessage.control == 6){ // fader 7 move X
        mountainMoveX  = ofMap(midiMessage.value, 0, 127, -1.0, 1.0);
    }
    if(midiMessage.control == 7){ // fader 8 move Y
        mountainMoveY  = ofMap(midiMessage.value, 0, 127, 1.0, -1.0);
    }
    
}

void ofApp::drawMountains(){
    
    
    // Sound -----
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    volLow = 0;
    volMid = 0;
    volHigh = 0;
    
    for (int i = 0; i<numOfVerts; i++){
        
   
        volLow += audioData[i];
        
        // check FFT:
        if(0){
            ofSetColor(255);
            ofDrawRectangle(i*10, 500, 10, audioData[i]*100);
        }
    }
    
    float volume = volLow / ((float) numOfVerts/2);
    
    volume *= audioReaction;
    
   //cout << "audio Rea 1 : " << audioReaction << "\n";
    
    delete [] audioData;
    // --- end sound

    
    noiseMaxMountains *= volume + 0.4;
    phaseMountains += volume * 0.4;
    
    ofSetBackgroundAuto(false);
    
    ofSetBackgroundColor(0);
    
    
    float t = ofGetElapsedTimef() * 1;
    
    int width = ofGetWidth();
    int height = ofGetHeight();
    
    // cam.begin();
    
    mountainX += mountainMoveX;
    mountainY += mountainMoveY;
    
    ofTranslate(mountainX, mountainY, -200);
    
    
    ofRotateZ(180);
    
    
    ofFill();
    ofSetLineWidth(2);
    
    float xoff, yoff,r, x, y;;
    float maxDrawnPoints = TWO_PI*20;
    for(int a = 0; a < maxDrawnPoints; a+=1) {
        float aa = (float) a/10;
        xoff    = ofMap(cos(aa + phaseMountains), -1, 1, 0, noiseMaxMountains);
        yoff    = ofMap(sin(aa ), -1, 1, 0, noiseMaxMountains);
        r       = ofMap(ofNoise(xoff, yoff), 0, 1, 50, 150);
        
        pointsMountains[a].x       = r * sin(aa);
        pointsMountains[a].y       = r * ofNoise(aa);
        pointsMountains[a].y       += sin(pointsMountains[a].x*1.72 + t*1.121)*4.0;
        
        pointsMountains[a].z       = aa / 10;
        
        if(a < maxDrawnPoints - 1 ){
            float sat, bri;
            sat = abs(maxDrawnPoints/2 - a );
            bri = (sin(t*0.01 + aa)*100) + 50;
            
            
            ofColor colHsb = ofColor::fromHsb(20, sat, bri , 80);
            
            ofSetColor(colHsb);
            ofDrawLine(pointsMountains[a], pointsMountains[a+1]);
            
        }
        
        
    }
}

void ofApp::addPointMountains(float x, float y, float z) {
    ofVec3f p(x, y, z);
    pointsMountains.push_back(p);
    
    // we are passing the size in as a normal x position
    float size = ofRandom(5, 50);
    sizesMountains.push_back(ofVec3f(size));
}
