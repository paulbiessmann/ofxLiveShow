//
//  noiseBlocks2D.cpp
//  ClusterDuo_v1-1_Main
//
//  Created by Paul Bießmann on 20.07.19.
//

#include <stdio.h>
#include "ofApp.h"

void ofApp::setupNoiseBlocks2D(){
    // ofSetBackgroundColor(150);
    ofBackground(220,220,210);

    
    
    noiseValueX = 0.03;
    noiseValueY = 0.0005;
    
    cam.reset();
    
}

void ofApp::updateNoiseBlocks2D(){
    //MIDI
    
    if(midiMessage.control == 21){ // rad 6 noiseValue X
        noiseValueX = ofMap(midiMessage.value, 0, 127, 0.0001, 0.05);
    }
    if(midiMessage.control == 22){ // rad 7 noiseValue Y
        noiseValueY = ofMap(midiMessage.value, 0, 127, 0.0001, 0.05);
    }
    if(midiMessage.control == 6){ // fader 7 für Noise Z, also Geschwindigkeit des Noise
        noiseValueZ = ofMap(midiMessage.value, 0, 127, 0.0, 0.01);
    }
    if(midiMessage.control == 5){ // fader 6 für Audio displacement amount
        audioDisplacement = ofMap(midiMessage.value, 0, 127, 0, 0.5);
    }
    
}



void ofApp::drawNoiseBlocks2D(){
    
    int width = 500;
    int height = 300;
    int resolution = 10;


    // Sound -----
    int numOfVerts = numVertsGlobal;

    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    volLow = 0;
    volMid = 0;
    volHigh = 0;

    for (int i = 0; i<numOfVerts; i++){
        
        if (i < numOfVerts/3){
            volLow += audioData[i];
        }
        else if (i > 2*numOfVerts/3){
            volHigh += audioData[i];
        }
        else{
            volMid += audioData[i];
        }
        
        // check FFT:
        if(0){
            ofSetColor(255);
            ofDrawRectangle(i*10, 500, 10, audioData[i]*100);
        }
    }

    float volume = volLow + volMid + volHigh;
    // --- end sound



    ofFill();
    ofDisableLighting();
    ofEnableDepthTest();

    cam.begin();
    ofTranslate(-width/2, -height/2, 400);

    // noiseZadding += noiseValueZ;
    noiseZadding +=  audioDisplacement * volume * 0.0005;

    
    for (int i=0; i < width; i+=resolution){
        for (int j=0; j < height; j+=resolution){
            
            ofSetColor(255);
            
            
            
            
            
            auto noise_value = ofNoise(i * noiseValueX, j * noiseValueY, noiseZadding) ;
            auto rotate_x = 0;
            if (noise_value < 0.25) {
                
             //   rotate_x = ofMap(noise_value, 0, 0.25, -180, 0);
                rotate_x = -180;
               //  rotate_x = -90;
                
            }
            else if (noise_value > 0.75) {
                
             //   rotate_x = ofMap(noise_value, 0.75, 1.0, 0, 180);
                rotate_x = 180;
                //rotate_x = 90;
            }
            
            ofPushMatrix();
           
            ofTranslate(i,j,0);
            
          
            ofRotateX(rotate_x);

            
            //ofRotateY(audioDisplacement * volume * rotate_x);
            
            
            
            // ofRotateX(ofGetElapsedTimef()*50+j+i);
            
            ofFill();
            ofSetColor(10);
            ofBeginShape();
            ofVertex(0, 0);
            ofVertex(resolution, 0);
            ofVertex(resolution, resolution);
            ofVertex(0, 0);
            ofEndShape(true);
            
            if(0){
                ofFill();
                ofSetColor(50);
                ofDrawBox(0, 0, 0, resolution, resolution/2, resolution);
                ofNoFill();
                ofSetColor(230);
                ofDrawBox(0, 0, 0, resolution, resolution/2, resolution);
            }
            
            ofPopMatrix();
            
            //            ofNoFill();
            //            ofSetColor(255);
            //            ofBeginShape();
            //            ofVertex(-9, -10);
            //            ofVertex(9, -10);
            //            ofVertex(9, 10);
            //            ofVertex(-9, 10);
            //            ofEndShape(true);
            //
            //            ofDrawBox(0,0,0, 5,5,0);
            
           
        }
    }
    
    cam.end();
    //    material.end();
    //    ofDisableLighting();
    

}
