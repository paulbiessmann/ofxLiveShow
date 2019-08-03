//
//  nodeLines.cpp
//  ClusterDuo_v1-1_Main
//
//  Created by Paul Bießmann on 20.07.19.
//

#include <stdio.h>
#include "ofApp.h"


bool    bDOF        = false;
bool    bCheckFFT   = false;
float   spectrumSmoothedNodes[256];

//--------------------------------------
void ofApp::setupNodeLines(){
    ofSetBackgroundAuto(true);

    ofSetBackgroundColor(255);

    
    ofEnableDepthTest();
    baseNode.setPosition(0, 0, 0);
    childNode.setParent(baseNode);
    childNode.setPosition(0, 300, 200);
    grandChildNode.setParent(childNode);
    grandChildNode.setPosition(-100,100,0);
    
    depthOfField.setup(ofGetWidth(), ofGetHeight());
    
    cam.setTarget(grandChildNode);
    camDistSpeed = 0;
    
    line.clear();
    line2.clear();
    
    glEnable(GL_LINE_SMOOTH);
    texScreen.clear();
    //    texScreen.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    texScreen.allocate(1600, 1000, GL_RGB);
    ofSetVerticalSync(true);
    
    // shader
    invertShader.load("invertShader");
    
    
    
}


void ofApp::updateNodeLines(){
    
    //MIDI
    if(midiMessage.control == 1){ // fader 1 audio Reaction value
        audioReaction  = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    
    // ----  Sound -----
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    for ( int i=0; i < numOfVerts; i++ ) {
        spectrumSmoothedNodes[i] *= 0.97;    //Slow decreasing 0.97
        spectrumSmoothedNodes[i] = max( spectrumSmoothedNodes[i], audioData[i] );
    }
    
    volLow = 0;
    volMid = 0;
    volHigh = 0;
    int lowCnt = 0;
    int midCnt = 0;
    int highCnt = 0;
    
    for (int i = 0; i<numVertsGlobal; i++){
        
        if (i < numOfVerts/15){
            lowCnt++;
            volLow += spectrumSmoothedNodes[i];
        }
        else{
            midCnt++;
            volMid += spectrumSmoothedNodes[i];
        }
//        else{
//            highCnt++;
//            volHigh += spectrumSmoothedNodes[i];
//        }
        
    }
    volLow /= lowCnt;
    volMid /= midCnt;
   // volHigh /= highCnt;
    
    volLow *= audioReaction;
    volMid *= audioReaction;
    
    delete [] audioData;
    //---- end sound ----

    
    noiseMax = ((float)mouseX / 200) + 0.1;
    
    depthOfField.setFocalDistance(myfocaldistance);
    depthOfField.setFocalRange(myfocalrange);
    
    int lineSize = (danceSize - 0.15) * 500 + 100; //2000
    
    //cout << "lineSize " << lineSize << "\n";
    
    float xoff, yoff, r;
    float t = ofGetElapsedTimef() * 0.01;
    
    baseNode.move(sin(t*0.01) * 5, sin(t*0.01) * 5, sin(t*0.01) * 5 );
    baseNode.pan(sin(t*0.05 ) * 4);
    childNode.tilt(ofNoise(-100,100) + sin(t * 0.03) *2);
    childNode.pan(ofNoise(-100,100));
  //  grandChildNode.orbit(sin(t*0.1)*40 + ofNoise(50)  , 30 + ofNoise(50) , 20 + ofNoise(50), childNode);
    
    line.addVertex(grandChildNode.getGlobalPosition());
    if (line.size() > lineSize){
        while(line.size() > lineSize){
            line.getVertices().erase( line.getVertices().begin() );
        }
    }
    int cnt = 0;
    for (auto &vert : line.getVertices() ){
        cnt++;
        vert.x += ofNoise(-50,50);
        vert.y += ofNoise(-100,100);
        vert.z += ofNoise(-100,100);
        vert.x += sin(cnt * 0.3 + t * 0.2 * volMid) * 0.5 * volLow + sin(1 + cnt * 0.01 + t * 0.1 * volMid) * 2.1 * volMid + sin(t*4.6)* 3 * volMid ;
        
        vert.x += sin(cnt * sin(cnt * 0.02 * t * volMid)) * volMid * 1;

        vert.y += cos(cnt * 0.05 + t * 10.1 * volMid + t) * 13 * volMid;
        vert.y += cos(cnt * 1.5 + t * 1.1 * volMid *  cos(cnt * t * 0.2 + 10)) * 10 * volMid ;

        vert.z += cos(cnt * 0.02 + t * 5.1 * volMid) * volMid  * 1;
        
        
    }
    
    //line 2
    line2.addVertex(childNode.getGlobalPosition());
    if (line2.size() > lineSize){
        while(line2.size() > lineSize){
            line2.getVertices().erase( line2.getVertices().begin() );
        }
    }
    
    cnt=0;
    
    for (auto &vert : line2.getVertices() ){
        cnt++;
        vert.x += ofNoise(-100,100);
        vert.y += ofNoise(-100,100);
        vert.z += ofNoise(-100,100);
        vert.x += cos(cnt * 0.01 + t * 4.4  * volLow) * 2.3  * volLow+ sin(cnt * 0.01 + t * 5.2 * volLow) * 1.2 * volLow;
        vert.y += sin(cnt * 0.05 + t * 5.5 * volLow ) * 4 * volLow + sin(cnt * 0.6 + t * 4.5 * volLow + t) * 0.4 * volMid;
//
        vert.y += sin(cnt * 1.5 + t * 5.1 * volLow + 10) * 0.04 * volLow;

        vert.z += sin(cnt * 0.02 + t * 5.1 * volLow * sin(cnt * 0.01 * t * volMid * 10)) * 2 * volLow ;


    }
    
    point1 = childNode.getGlobalPosition();
    point2 = grandChildNode.getGlobalPosition();
    
    
}



void ofApp::drawNodeLines(){
    
    if(bDrawNet){
        ofSetBackgroundColor(0);
    }
    
    ofEnableAlphaBlending();
    
    ofEnableDepthTest();
    ofEnableAntiAliasing();
    
    if(bDOF){
        depthOfField.begin();
        cam.begin(depthOfField.getDimensions());
    }else{
        cam.begin();
    }
    
    cam.setFarClip(20000);
    
    if(!bChangeNode){
        cam.setTarget(childNode);
    }
    else{
        cam.setTarget(grandChildNode);
    }
//    
    if(bAutoCam){
//        cam.getTarget();
//
//        camDistSpeed = cam.getDistance();
//        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        camLatSpeed += camLatSpeedUpdate;
//        //cam.orbit(camRotSpeed , camLatSpeed, camDistSpeed);
        camDistSpeed = cam.getDistance();
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        
        cam.roll(camRotSpeed);
        
    cam.setDistance(camDistSpeed);
        
    }
    
//uncomment these 3 lines to understand how nodes are moving
//    baseNode.draw();
//    childNode.draw();
//    grandChildNode.draw();


    
    int numLinesRes = 1;
    
   // ofSetColor( 250, 240,250, 40);
    
    if(bDrawNet){
        ofSetLineWidth(2);
        
        ofSetColor(220, 210, 210, 200);
//        ofSetColor(10, 8, 10, 60);
    
        for (int i = 0; i < line2.size(); i += numLinesRes ){
            auto & vert = line2.getVertices();
            
            ofPushMatrix();

            ofDrawLine(vert[i].x - (i/2) , vert[i].y, vert[i].z , vert[i].x + (i/2), vert[i].y , vert[i].z);
            
            ofPopMatrix();
            
            
        }



    
        ofSetColor(220, 210, 210, 200);
        
        //ofSetColor(5, 4, 7, 60);
        
        for (int i = 0; i < line.size(); i += numLinesRes){
            auto & vert = line.getVertices();
            
            ofPushMatrix();
            ofDrawLine(vert[i].x - (i/2) + sin(i) , vert[i].y, vert[i].z, vert[i].x + (i/2) , vert[i].y , vert[i].z);
            ofPopMatrix();

        }
        
    }
    
    else{
        ofSetLineWidth(3);
        //    ofSetColor(250, 240, 250 );
        ofSetColor(0);
        

        line.draw();
        //  ofSetColor(220, 210, 210);
       
        //ofSetColor(255,0,0);
        line2.draw();
    }
    cam.end();
    
    if(bDOF){
        depthOfField.end();
        depthOfField.getFbo().draw(0, 0);
    }
    
    for ( int i=0; i<numVertsGlobal; i++ ) {
        
        // check FFT:
        if(bCheckFFT){
            ofSetColor(0);
            ofDrawRectangle(i*5, 500, 5, spectrumSmoothedNodes[i]*100);
        }
    }
    
    
    
 if(0){
     
        int width = ofGetWidth();
        int height = ofGetHeight();
        int numTexWins = numScreens; // min: 1
        int texWidth = width/numTexWins;
        int texHeight = height/numTexWins;
        
        texScreen.loadScreenData(width/numTexWins, height/numTexWins, texWidth, texHeight);
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
