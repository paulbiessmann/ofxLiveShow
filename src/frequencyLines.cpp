//
//  frequencyLines.cpp
//  Enlivenment_Main
//
//  Created by Paul Bießmann on 13.07.19.
//

#include <stdio.h>
#include "ofApp.h"

float numP = 700;

//const int Nbands = ;          //Number of bands in spectrum
float spectrumSmoothed[256];          //Smoothed spectrum values


//--------------------------------------------------------------
void ofApp::setupFrequencyLine(){
    

    ofEnableDepthTest();
    noiseMax = 0.5;
    
    for(int a = 0; a < numP; a+=1) {
        
        ofVec3f pLine;
        
        pLine.x = ofRandom(ofGetWidth());
        pLine.y = ofRandom(ofGetHeight());
        pLine.z = ofRandom(100);
        
        addPoint(pLine.x, pLine.y, pLine.z);
    }
    
    glEnable(GL_LINE_SMOOTH);
    texScreen.clear();
  //  texScreen.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    texScreen.allocate(1600, 1000, GL_RGB);
    ofSetVerticalSync(true);
    
    
    // shader
    invertShader.load("invertShader");
    
    // sound ohne FFT
    int bufferSize = 256;

    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);

    //Set spectrum values to 0
    for (int i=0; i<numVertsGlobal; i++) {
        spectrumSmoothed[i] = 0.0f;
    }
  
    numScreens      = 1;
    if(bDrawFreqCircles){
        bAutoCam        = true;
        cam.reset();
    }else{
        bAutoCam        = false;
    }
    
    bInvertColor    = true;
//    ofSetColor(0);
//    ofDrawRectangle(0,0,1600,844);
//    ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::updateFrequencyLine(){
    
    //MIDI
    if(midiMessage.control == 0){ // fader 0 audio Reaction value
        audioReaction  = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    
    ofSetBackgroundColor(ofColor(0));
    
    noiseMax = ((float)mouseX / 2000) + 0.1;
    
    phase += (float)mouseY / 8000;
    
}

//--------------------------------------------------------------
void ofApp::drawFrequencyLine(){
    
    
// Sound -----
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    for ( int i=0; i<numVertsGlobal; i++ ) {
        spectrumSmoothed[i] *= 0.97;    //Slow decreasing 0.97
        spectrumSmoothed[i] = max( spectrumSmoothed[i], audioData[i] );
    }
    
    volLow = 0;
    volMid = 0;
    volHigh = 0;
    int lowCnt = 0;
    int midCnt = 0;
    int highCnt = 0;
    
    for (int i = 0; i<numVertsGlobal; i++){
        
        if (i < numOfVerts/6){
            lowCnt++;
            volLow += spectrumSmoothed[i];
        }
        else if (i > 2*numVertsGlobal/6){
            midCnt++;
            volHigh += spectrumSmoothed[i];
        }
        else{
            highCnt++;
            volMid += spectrumSmoothed[i];
        }
        // check FFT:
        if(0){
            ofSetColor(255);
            ofDrawRectangle(i*10, 500, 10, audioData[i]*100);
        }
    }
    volLow /= lowCnt;
    volMid /= midCnt;
    volHigh /= highCnt;
    
    volLow *= audioReaction;
    volMid *= audioReaction;
    volHigh *= audioReaction;
    
    delete [] audioData;

// --- end sound
    
    if(bDrawFreqCircles){
        cam.begin();
   
        if(bAutoCam){
            
            camDistSpeed = cam.getDistance();
            camDistSpeed += camDistSpeedUpdate * 0.5; //cameraSpeed
            camRotSpeed += camRotSpeedUpdate * 0.5;
            cam.orbit(camRotSpeed , abs(camRotSpeed/3), camDistSpeed, ofVec3f(0,0,0) );
            cam.getTarget();

        }
     }
    
    float zMove = sin(ofGetElapsedTimef() * 0.5) * 1000 + 1300;
    float yMove = sin(ofGetElapsedTimef() * 0.1) * 500;
    
 
    if(bDrawFreqCircles){
        drawFreqCircles(25);
    }
    else{
        drawSingle(3);
    }
    
    if(bDrawFreqCircles){
        cam.end();
    }
    
    int width = ofGetWidth();
    int height = ofGetHeight();
    int numTexWins = numScreens; // min: 1
    int texWidth = width/numTexWins;
    int texHeight = height/numTexWins;

    
    
    if(1){
      
        texScreen.loadScreenData(0, 10, texWidth, texHeight);
        for (int i = 0; i< numTexWins; i++){
            for(int j = 0; j< numTexWins; j++){
                
    //             texScreen.loadScreenData((i+1) * texWidth , (j + 1) * texHeight, texWidth, texHeight);
                
                if(i%2 == 0 && j%2 == 0){
                    if(bInvertColor){
                        invertShader.begin();
                        invertShader.setUniformTexture("textureInvert", texScreen,1);
                        texScreen.draw((i) * texWidth, (j) * texHeight, texWidth,texHeight);
                        shader.end();
                    }else{
                        texScreen.draw((i) * texWidth, (j) * texHeight, texWidth,texHeight);
                    }
                }
                if(i%2 == 0 && j%2 != 0){
                    if(!bInvertColor){
                        invertShader.begin();
                        invertShader.setUniformTexture("textureInvert", texScreen,1);
                        texScreen.draw((i + 1) * texWidth, (j) * texHeight, texWidth,texHeight);
                        shader.end();
                    }else{
                        texScreen.draw((i + 1) * texWidth, (j) * texHeight, texWidth,texHeight);
                    }
                }
                if(i%2 != 0 && j%2 == 0){
                    if(!bInvertColor){
                        invertShader.begin();
                        invertShader.setUniformTexture("textureInvert", texScreen,1);
                        texScreen.draw((i) * texWidth, (j + 1) * texHeight, texWidth,texHeight);
                        shader.end();
                    }else{
                        texScreen.draw((i) * texWidth, (j + 1) * texHeight, texWidth,texHeight);

                    }
                    
                }
                
            }
        }
    }

}

void ofApp::drawSingle(int numShapes){
    
    //numShapes = 3;
    
    
   // ofTranslate(-ofGetWidth()/numShapes, -ofGetHeight()/numShapes);
    
    ofFill();
    ofSetLineWidth(2);
    
    for (int i = 1; i <= numShapes; i++){
        
        
        ofPushMatrix();
        // for loop for translation
        //ofTranslate(ofMap(i, numShapes/2 , numShapes, -ofGetWidth()/numShapes, ofGetWidth()/numShapes), 0, i*50);
       // ofTranslate(0, ofMap(i, 1, numShapes, 0, ofGetHeight()/i), 0);
        
        ofTranslate(0, 200* (i-1) * 2, 0);
   
        
        drawShape(i);
        ofPopMatrix();
        
        
    }
}

void ofApp::drawFreqCircles(int numShapes){
    pLine.clear();
    float xoff, yoff, r;
    float t = ofGetElapsedTimef();
    float vol;

    vol = volLow + volMid + volHigh;
    
    vol /= 3;
    
    
    ofSetLineWidth(2);
    
    for (int i = 1; i <= numShapes; i++){
        
        

        // for loop for translation
        //ofTranslate(ofMap(i, numShapes/2 , numShapes, -ofGetWidth()/numShapes, ofGetWidth()/numShapes), 0, -i*50);
        // ofTranslate(0, ofMap(i, 1, numShapes, 0, ofGetHeight()/i), 0);
        
        ofTranslate(0, 0,  sin(t * 0.5 +i) * 10 + vol);
    
        ofPushMatrix();
        
        for(int a = 0; a < TWO_PI *3; a+=1) {
            float aa = (float) a;
            xoff    = ofMap(cos(aa + phase), -1, 1, 0, noiseMax/2 );
            yoff    = ofMap(sin(aa + i), -1, 1, 0, noiseMax * vol*0.1);
            r       = ofMap(ofNoise(xoff, yoff), 0, 1, 50, 100) + i * 50;
            
            freqPoints[a].x       = r * cos(aa );
            freqPoints[a].y       = r * sin(aa );
//            freqPoints[a].x       += i * cos(t) * 10;
//            freqPoints[a].y       += i * sin(t) * 10;
            freqPoints[a].x      += sin(freqPoints[a].x*1.984 * i +  t*0.121 * i)*4.0 * vol *2 * i;
            freqPoints[a].y      += cos(freqPoints[a].x*0.221 * i * vol+ t*0.437 * i)*5.0 * vol *2 *i;
            
            freqPoints[a].z       = i * sin(t * 0.1) *10;
            freqPoints[a].z       +=   sin(t * 0.2 * i + vol ) * 50 ; //aa / 1;
            
            
            if(a < freqPoints.size() -1){
                ofSetColor(255, 255, 255);
                //   ofDrawLine(freqPoints[a], freqPoints[a+1]);
                //   ofDrawIcoSphere(freqPoints[a], 1);
                pLine.curveTo(freqPoints[a]);
                
            }
        }
        
        pLine.draw();
        ofPopMatrix();
        
        
    }
    
    

}

void ofApp::drawShape(int numOfShape){
    
    
    pLine.clear();
    float xoff, yoff, r;
    float t = ofGetElapsedTimef();
    float vol;
    
    if (numOfShape == 1){
        vol = volLow;
    }else if(numOfShape == 2){
        vol = volMid;
    }else{
        vol = volHigh;
    }
    
    vol *= 3;
    
    for(int a = 0; a < numP; a+=1) {
        float aa = (float) a/50;
        xoff    = ofMap(cos(aa + phase), -1, 1, 0, noiseMax/2 );
        yoff    = ofMap(sin(aa + numOfShape), -1, 1, 0, noiseMax * vol*0.1);
        r       = ofMap(ofNoise(xoff, yoff), 0, 1, 100, 200);
        
        freqPoints[a].x       = r * aa - numOfShape;
        freqPoints[a].y       = r - numOfShape;
        //freqPoints[a].y       = 0;
        freqPoints[a].y      += sin(freqPoints[a].x*1.984 * numOfShape +  t*1.121 * numOfShape)*4.0 * vol *10;
        freqPoints[a].y      += cos(freqPoints[a].x*2.221 * numOfShape * vol+ t*0.437 * numOfShape)*5.0 * vol *1;
        
        // freqPoints[a].z       = aa * sin(ofGetElapsedTimef()* 0.01);
        freqPoints[a].z       = 1; //aa / 1;
        
        if(a < freqPoints.size() ){
            ofSetColor(255, 255, 255);
            //   ofDrawLine(freqPoints[a], freqPoints[a+1]);
            //   ofDrawIcoSphere(freqPoints[a], 1);
            pLine.curveTo(freqPoints[a]);
            
        }
        
        
    }
    
    pLine.draw();
}

void ofApp::addPoint(float x, float y, float z) {
    ofVec3f pp(x, y, z);
    freqPoints.push_back(pp);
    
    // we are passing the size in as a normal x position
    float size = ofRandom(5, 50);
    sizes.push_back(ofVec3f(size));
}


