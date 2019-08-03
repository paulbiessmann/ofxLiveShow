//
//  bezierDrawing.cpp
//  ClusterDuo_v1-1_Main
//
//  Created by Paul Bießmann on 19.07.19.
//

#include <stdio.h>
#include "ofApp.h"

void ofApp::setupBezierLines(){
    ofBackground(220,220,210);
    
    linesBezier.clear();
    linesBezierMirror.clear();
    
    aLine.clear();
    bLine.clear();
    
    aLineMirror.clear();
    bLineMirror.clear();
    
    ofDisableLighting();
    
    ofSetLineWidth(2);
    
    
}

void ofApp::updateBezierLines(){
    
}

void ofApp::drawBezierLines(){
    
    ofDisableDepthTest();
    ofDisableLighting();
    
    ofEnableAlphaBlending();
    ofSetColor(20,20,20,80);
    
    ofSetLineWidth(1);
    
    int cnt = 0;
    for(auto line : linesBezier){
        
        if(cnt < linesBezier.size()/2  ){
            aLine.addVertex(line.a);
        }else{
            bLine.addVertex(line.b);
        }
        cnt++;
        ofDrawLine(line.a, line.b);
    }
    
    cnt = 0;
    for(auto line : linesBezierMirror){
        
        if(cnt < linesBezierMirror.size()/2  ){
            aLineMirror.addVertex(line.a);
        }else{
            bLineMirror.addVertex(line.b);
        }
        cnt++;
        ofDrawLine(line.a, line.b);
    }
    
    if(bRotate && aLine.size() > 0){
        ofSetLineWidth(1);
        
        for ( int i = 0; i < aLine.size()-1; i += 400){
            
            ofPoint aa = aLine[i % aLine.size()];
            ofPoint bb = aLine[i+1 % aLine.size()];
            ofPoint cc = bLine[(i + mouseY)  % bLine.size()];
            ofPoint dd = bLine[(i+1 + mouseY)  % bLine.size()];
            
            float dist = (aa - cc).length();
            
            ofPoint extended = bb + (aa-bb).normalize() * 60;
            ofPoint extended2 = dd + (cc-dd).normalize() * 60;
            
            ofNoFill();
            ofDrawBezier(bb.x, bb.y, extended.x, extended.y, extended2.x, extended2.y, dd.x, dd.y);
            
        }
        
        for ( int i = 0; i < aLineMirror.size()-1; i += 500){
            
            ofPoint aa = aLineMirror[i % aLineMirror.size()];
            ofPoint bb = aLineMirror[i+1 % aLineMirror.size()];
            ofPoint cc = bLineMirror[(i + mouseY)  % bLineMirror.size()];
            ofPoint dd = bLineMirror[(i+1 + mouseY)  % bLineMirror.size()];
            
            float dist = (aa - cc).length();
            
            ofPoint extended = bb + (aa-bb).normalize() * 60;
            ofPoint extended2 = dd + (cc-dd).normalize() * 60;
            
            ofNoFill();
            ofDrawBezier(bb.x, bb.y, extended.x, extended.y, extended2.x, extended2.y, dd.x, dd.y);
            
        }
    }
    
    
}


void ofApp::closeBezierLines(){
    
    bDrawBezierLine     = false;
    linesBezier.clear();
    linesBezierMirror.clear();
    drawnPointsBezier.clear();
    drawnPointsBezierMirror.clear();
    
    aLine.clear();
    bLine.clear();
    
    aLineMirror.clear();
    bLineMirror.clear();
}
