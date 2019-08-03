/* Paul Bießmann 30.Nov 2018 -- Made for Cluster Electronic Session*/

#include "ofApp.h"
//#include <GLUT/GLUT.h>

//for DanceCloud:
const int N = 256;        //Number of bands in spectrum
float spectrum[N];    //Smoothed spectrum values
float Rad = 500;        //Cloud raduis parameter
float Vel = 0.1;        //Cloud points velocity parameter
int bandRad = 20;        //Band index in spectrum, affecting Rad value
int bandVel = 100;        //Band index in spectrum, affecting Vel value
int nBegin = 20; //2
const int nMax = 220; //max Number of cloud points 200
int n = nBegin;
//Offsets for Perlin noise calculation for points
float tx[nMax], ty[nMax], tz[nMax];
ofPoint p[nMax];            //Cloud's points positions
float time0 = 0;        //Time value, used for dt computing
float pointUpdateTime = 9; // every x seconds 1 new point
float guiNoiseVal = 0;



//--------------------------------------------------------------
void ofApp::setup(){
    
    
    depthOfField.setup(ofGetWidth(), ofGetHeight());
   //  depthOfField.setup(1920, 1080); //full HD 1080p
    //depthOfField.setup(1280, 720); // 720p
    ofSetBackgroundColor( 0 );    //Set up the background

    
//    ofSetFrameRate(40);
    fullscreen = false;
    
    
    //--------------- fhg
//    soundPlayer.loadSound("sound/Katy Perry_Birthday_48kHz.wav");
//    soundPlayer.setLoop(false);
//    soundPlayer.play();
    fftFile.setMirrorData(false);
    fftFile.setup();
    //-------------------
    
    //Set up sound
    soundStream.printDeviceList();
//    soundStream.setDeviceID(2); //2
    soundStream.getNumInputChannels();

 //   soundStream.setup(0, 2, 44100, 256, 4);
//    fftLive.ofBaseSoundInput::audioIn;
    fftLive.setup();
    fftLive.setMirrorData(false);

    // MIDI
    midiIn.listPorts();
    midiIn.openPort(3); //3
    midiIn.ignoreTypes(false, false, false);
    midiIn.addListener(this);
    midiIn.setVerbose(true);
    
    
    ofSetBackgroundAuto(true);
    fadeAmount = 50;

    // FBO for alpha fading
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbo.begin();
    fbo.clear();
    fbo.end();
    
    
    setupGui();
    
    
    // load XML parameter settings
    xml.load("mySettings.xml") ;


}

//-------------------------------------------------------------
void ofApp::setupGui(){
    
    string guiPath = "audio.xml";
    gui.setup("audio", guiPath, 20,20);
    gui.add(guiNoise.setup("guiNoise", 0.5, 0.0, 8.0));
    gui.add(danceSize.setup("danceSize", 0.3, 0.0, 10.0));
    gui.add(myfocaldistance.setup("focalDistance", 50, 10, 1000));
    gui.add(myfocalrange.setup("focalRange", 50, 10, 1000));
    gui.add(dLine.setup("drawLine", true));
    gui.add(bAutoCam.setup("bAutoCam", true));
    gui.add(cellSize.setup("cellSize", 0, 10, 1000));
    gui.add(camLatSpeedUpdate.setup("camLatSpeed",0.08, -2, 2 ));
    gui.add(camRotSpeedUpdate.setup("camRotSpeed",0.08, -2, 2 ));
    gui.add(camDistSpeedUpdate.setup("camDistSpeed",0.08, -2, 2 ));

    gui.add(useSound.setup("useSound", true));
    gui.add(audioThreshold.setup("audioThreshold", 1.0, 0.0, 1.0));
    gui.add(audioPeakDecay.setup("audioPeakDecay", 0.95, 0.0, 1.0));
    gui.add(audioMaxDecay.setup("audioMaxDecay", 0.995, 0.0, 1.0));
    gui.add(meshDisplacement.setup("meshDisplacement", 200, 0, 400));
    gui.loadFromFile(guiPath);
    
    oldDanceSize = newDanceSize = danceSize;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(useSound){
        fftLive.setThreshold(audioThreshold);
        fftLive.setPeakDecay(audioPeakDecay);
        fftLive.setMaxDecay(audioMaxDecay);
        fftLive.update();
    }
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    if(bHideCursor){
        ofHideCursor();
    }else{
        ofShowCursor();
    }
    
    if (bDrawDanceCloud ){
        pointUpdateTime = 12;
//        nMax = 200;
        updateDanceCloud();
    }else if (bDrawTriangleCloud){
        pointUpdateTime = 7; //10
//        nMax = 30;
        updateDanceCloud();
    }
    else if(bDrawRibbonBoids){
        updateRibbonBoids();
    }else if(bDrawSpectroSphere){
        updateSpectroSphere();
    }
    
    if(bDrawFleeingCells){
        updateFleeingCells();
    }
    
    if(bDrawGrowingLines){
        updateGrowingLines();
    }
    
    if(bDrawStringOfPearls){
        updateStringOfPearls();
    }
    
    if(bDrawGlitchRect){
        updateGlitchRect();
    }
    
    if(bDrawNoiseBlocks){
        updateNoiseBlocks();
    }
    
    if(bDrawFrequencyLine){
        updateFrequencyLine();
    }
    
    if(bDrawParticlesGPU){
        updateParticlesGPU();
    }
    
    if(bDrawCloudsFbm){
        updateCloudsFbm();
    }
    
    if(bDrawBezierLine){
        updateBezierLines();
    }
    
    if(bDrawMountains){
        updateMountains();
    }
    
    if(bDrawNodeLines){
        updateNodeLines();
    }
    
    if(bDrawNoiseBlocks2D){
        updateNoiseBlocks2D();
    }
    
    //----------------- global MIDI  ------------------
    if(midiMessage.control == 0){ // Nanokontrol fader 1 = cc 0  dancesize
        oldDanceSize = danceSize;
        newDanceSize = ofMap(midiMessage.value, 0, 127, 0.1, 10.0);
        newValue = true;
    }
    if(midiMessage.control == 16){ // rad 1 = cc16  guinoise
        guiNoise = ofMap(midiMessage.value, 0, 127, 0.0, 8.0);
    }
    if(midiMessage.control == 1){ // fader 2    focaldist
        myfocaldistance  = ofMap(midiMessage.value, 0, 127, 10, 1000);
    }
    if(midiMessage.control == 17){ // rad 2     focalrange
        myfocalrange  = ofMap(midiMessage.value, 0, 127, 10, 1000);
    }
    
    if(midiMessage.control == 33 && midiMessage.value == 127) { // solo 2   focalassist true
        bDrawFocusAssist = true;
    }
    if(midiMessage.control == 49 && midiMessage.value == 127) { // mute 2   focalassist false
        bDrawFocusAssist = false;
    }
    if(midiMessage.control == 23){ // rad 8 : color fading
        fadeAmount = ofMap(midiMessage.value, 0, 127, 0, 255);
    }
    if(midiMessage.control == 3){  // fader 4   cam zoom speed
        camDistSpeedUpdate = ofMap(midiMessage.value, 0, 127, 3.0, -3.0);
    }
    if(midiMessage.control == 18){ //rad 3      cam rot speed
        camLatSpeedUpdate = ofMap(midiMessage.value, 0, 127, -2.0, 2.0);
    }
    if(midiMessage.control == 19){ //rad 4      cam rot speed
        camRotSpeedUpdate = ofMap(midiMessage.value, 0, 127, 2.0, -2.0);
    }

    if(midiMessage.control == 35 && midiMessage.value == 127){ //solo 4  auto cam on
        bAutoCam = true;
    }
    if(midiMessage.control == 51 && midiMessage.value == 127){ //mute 4  auto cam off
        bAutoCam = false;
    }

    
    //----------------- end global MIDI  ------------------
    // MIDI Value smoothening
    if (newValue){
        if (danceSize < newDanceSize - 0.1){
            danceSize = oldDanceSize + 0.05;
        }
        if (danceSize > (newDanceSize + 0.1) ){
            danceSize = oldDanceSize - 0.05;
        }
        else{
           // danceSize = newDanceSize;
            newValue = false;
        }
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if ( bDrawDanceCloud || bDrawTriangleCloud){
        drawDanceCLoud();

    }else if(bDrawRibbonBoids){
        ofSetBackgroundColor(250);

        ofEnableAlphaBlending();
        ofSetColor(0,0,0,255-fadeAmount);
        ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        ofSetColor(255,255,200,fadeAmount);
        drawRibbonBoids();
        
    }else if(bDrawSpectroSphere){
        ofSetBackgroundAuto(true);
        drawSpectroSphere();
    }
    else if(bDrawFleeingCells){
        drawFleeingCells();
    }
    
    else if(bDrawGrowingLines){
        drawGrowingLines();
    }
    
    else if(bDrawStringOfPearls){
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(0);
        ofEnableAlphaBlending();
        ofSetColor(0,0,0,255-fadeAmount);
        ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        ofSetColor(255,255,255,fadeAmount);

        drawStringOfPearls();
  
    }
    
    else if(bDrawGlitchRect){
        
        ofSetBackgroundAuto(false);
        ofSetBackgroundColor(0);
        ofEnableAlphaBlending();
        ofSetColor(0,0,0,255-fadeAmount);
        ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
        ofSetColor(255,255,255,fadeAmount);
        drawGlitchRect();
    }
    
    else if(bDrawNoiseBlocks){
        drawNoiseBlocks();
    }
    
    else if(bDrawFrequencyLine){
        drawFrequencyLine();
    }
    
    else if(bDrawParticlesGPU){
        drawParticlesGPU();
    }
    
    else if(bDrawCloudsFbm){
        drawCloudsFbm();
    }
    
    else if(bDrawBezierLine){
        drawBezierLines();
    }
    
    else if(bDrawMountains){
        drawMountains();
    }
    
    else if(bDrawNodeLines){
        drawNodeLines();
    }
    
    else if(bDrawNoiseBlocks2D){
        drawNoiseBlocks2D();
    }
    
    else if(bDrawBlack){
        ofSetBackgroundAuto(true);
        ofSetBackgroundColor(0);
    }
    
    
}
    

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs & args){
    
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
    
    ofSetBackgroundColor(30);
    ofSetColor(255);
    
    gui.draw();

    text << "SceneNr: " << sceneNumber << " Scene: " << sceneName;
    ofDrawBitmapString(text.str(), 250, 20);
    text.str(""); // clear
    
    
    camera.getDistance();
    
    text << "Cam Distance: " << camera.getDistance();;
    ofDrawBitmapString(text.str(), 250, 40);
    text.str(""); // clear
    
    text << "camRotSpeed: " << camRotSpeed;
    ofDrawBitmapString(text.str(), 250, 60);
    text.str(""); // clear
    
    text << "camRotSpeed: " << camDistSpeed;
    ofDrawBitmapString(text.str(), 250, 80);
    text.str(""); // clear
    
    text << "drawAntiBoid: " << drawAntiboids;
    ofDrawBitmapString(text.str(), 250, 100);
    text.str(""); // clear
    
    text << "newBoid: " << newBoid;
    ofDrawBitmapString(text.str(), 250, 120);
    text.str(""); // clear

    
    text << "Growing Lines: " << bGrowLine;
    ofDrawBitmapString(text.str(), 250, 150);
    text.str(""); // clear
    
    

    
    
}


//--------------------------------------------------------------
void ofApp::setupRibbonBoids(){
    ofSetBackgroundAuto(false);
    ofSetVerticalSync(true);
   //ofBackground(255);
   // ofBackgroundGradient( ofColor(255,255,102),ofColor(0), OF_GRADIENT_CIRCULAR);

    
    ofEnableAlphaBlending();
    ofEnableDepthTest();
    ofEnableSmoothing();
    ofEnableAntiAliasing();

    lightRibbon.setup();
    lightRibbon.enable();
    //    GLfloat color[] = { 0.8, 0.3, 0.9 };
    lightRibbon.setAmbientColor(ofColor(204,76,245));
    
    ofHideCursor();
    
    //initialize the variable so it's off at the beginning
    usecamera       = false;
    reverse         = false;
    boidsSphere     = false;
    newBoid         = false;
    drawAntiboids   = false;
    time            = 0;
    colorChanging   = 100;
    ribbonThickness = 1;
    drawRibbonAuto  = false;
    speed           = 3;
    circleSize      = 10;
    useDOF          = false;
    
    // sphere to align boids
    bSphere.set(200, numVertsGlobal);
    
    
    //---  boids 1 ------
    icoSphereBoids1.setRadius(5);
    boidNum = 100;
    target = ofVec3f(0, 0, 0);
    
    for (int i = 0; i < boidNum; i++)
    {
        SteeredVehicle v(ofRandom(100)+(ofGetWindowWidth()), ofRandom(100)+(ofGetWindowHeight()), ofRandom(100)-50);
        //SteeredVehicle v(ofRandom(100)-50, ofRandom(100)-50, ofRandom(100)-50);
        v.maxForce = 0.5f; //0.5
        v.inSightDist = 10.0f; //80
        
        boids.push_back(v);
    }
    
    
    //---  antiboids  ---
    antiBoidNum = 50;
    icoSphereBoids2.setRadius(10);
    
    for (int i = 0; i < antiBoidNum; i++){
        SteeredVehicle v2(ofRandom(100)+(ofGetWindowWidth()), ofRandom(100)+(ofGetWindowHeight()), ofRandom(100)-50);
        v2.maxForce = 0.5f; //0.5
        v2.inSightDist = 80.0f; //80
        //v2.wanderRange = 100;
        antiBoids.push_back(v2);
    }
    
    //--------- boids 3 --------
    boid3fac = 5;
    boidNum3 = boidNum * boid3fac;
    boxBoids3.set(2);
    
    for (int i = 0; i < boidNum3; i++){
        SteeredVehicle v3(ofRandom(100)+(ofGetWindowWidth()), ofRandom(100)+(ofGetWindowHeight()), ofRandom(100)-50);
        v3.maxForce = 0.2f; //0.5
        v3.wanderRange = 100;
        v3.inSightDist = 180.0f; //80
        
        boids3.push_back(v3);
    }
    
    lightSpectroSph.disable();
}

//--------------------------------------------------------------
void ofApp::updateRibbonBoids(){
    
    // ----------- MIDI -----------------
    if(midiMessage.control == 0){ // fader1
        boidSphereSize = ofMap(midiMessage.value, 0, 127, 0.0, 1000.0);
    }
    if(midiMessage.control == 16){ // rad 1
        boidSphereRotUpdate = ofMap(midiMessage.value, 0, 127, 0, 5);
    }
    if(midiMessage.control == 5){ // fader 6 für speed
        speed = ofMap(midiMessage.value, 0, 127, 0, 30);
    }
    if(midiMessage.control == 21){ // rad 6 für auto ribbon kreis größe
        circleSize = ofMap(midiMessage.value, 0, 127, 1, 500);
    }
    if(midiMessage.control == 6){ // fader 7
        colorChanging = ofMap(midiMessage.value, 0, 127, 0.5, 100);
    }
    if(midiMessage.control == 22){ // rad 7 für ribbon breite
        ribbonThickness = ofMap(midiMessage.value, 0, 127, 0.5, 10);
    }
    // ----------- endMIDI -----------------
    
    
    if(drawRibbonAuto){
        ofVec3f autoDraw(sin(ofGetElapsedTimef())*circleSize+(ofGetWindowWidth()/2), cos(ofGetElapsedTimef())*circleSize+ofGetWindowHeight()/2, 0);
        points.push_back(autoDraw);
    }
    
    //don't move the points if we are using the camera
    if( !reverse && !boidsSphere){
        ofVec3f sumOfAllPoints(0,0,0);
        for(unsigned int i = 0; i < points.size(); i++){
            points[i].z += speed;
            sumOfAllPoints += points[i];
        }
        center = sumOfAllPoints / points.size();
    }else if(reverse && !boidsSphere){
        ofVec3f sumOfAllPoints(0,0,0);
        for(unsigned int i = 0; i < points.size(); i++){
            points[i].z -= speed;
            sumOfAllPoints += points[i];
        }
        center = sumOfAllPoints / points.size();
    }
    

}

//--------------------------------------------------------------
void ofApp::drawRibbonBoids(){
    

    ofSetColor(255);

    // ----- sound
    
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
    
    if(useDOF){
        depthOfField.setFocalDistance(myfocaldistance);
        depthOfField.setFocalRange(myfocalrange);
        depthOfField.begin();
    }
    
    lightRibbon.enable();
    if(usecamera){
        camera.begin();
        camera.getTarget();
    }
    
    
    ofSetColor(50,255,0,100);
    //do the same thing from the first example...
    ofMesh meshRibbon; //ribbon mesh
    meshRibbon.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    int j = 0;
    if (!boidsSphere){
        
        //sound ribbon
        int numOfVerts = numVertsGlobal;
        float * audioData = new float[numOfVerts];
        fftLive.getFftPeakData(audioData, numOfVerts);
        float audioVolume = 0;
        for (int i = 0; i<numOfVerts; i++){
            audioVolume += audioData[i];
        }

        for(unsigned int i = 1; i < points.size(); i++){
            
            //find this point and the next point
            ofVec3f thisPoint = points[i-1];
            ofVec3f nextPoint = points[i];
            
            //get the direction from one to the next.
            //the ribbon should fan out from this direction
            ofVec3f direction = (nextPoint - thisPoint);
            
            //get the distance from one point to the next
            float distance = direction.length();
            
            //get the normalized direction. normalized vectors always have a length of one
            //and are really useful for representing directions as opposed to something with length
            ofVec3f unitDirection = direction.getNormalized();
            
            //find both directions to the left and to the right
            ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
            ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
            
            //use the map function to determine the distance.
            //the longer the distance, the narrower the line.
            //this makes it look a bit like brush strokes
            float thickness = ofMap(distance, 0, 60, 20, 10, true);
            //
            int pSize = points.size();
            if (i < numOfVerts){
                thickness += 40 * audioData[numOfVerts-i];
            }else if(i > pSize-numOfVerts){
                thickness += 40 * audioData[pSize-i];
            }
            
            //calculate the points to the left and to the right
            //by extending the current point in the direction of left/right by the length
            ofVec3f leftPoint = thisPoint+ (toTheLeft * thickness * ribbonThickness);
            ofVec3f rightPoint = thisPoint+ (toTheRight * thickness * ribbonThickness);

            
            //add these points to the triangle strip
            meshRibbon.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
            meshRibbon.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
            

        }
        
        if(bDrawParticles){
            if(points.size() > 0){
                 ofVec3f circlePoint = points[points.size()-1];
                 ofDrawCircle(circlePoint, 20+100*audioVolume);
            }
 
            
        }
        delete[] audioData;
    }

    
    // -------------- create sphere to align boids -------------------------------
    if (boidsSphere){
        
        fftLive.setPeakDecay(0.99);
        fftLive.setMaxDecay(0.99);
        
        //ofSetBackgroundColor(255,255,153);
        if(!usecamera){
            ofTranslate(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 0);
        }
        
        bSphere.setRadius(boidSphereSize );// 200* sin(ofGetElapsedTimef()));
        //bSphere.setPosition(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 0);
        
        // bSphere.setPosition(points[points.size()-1].x, points[points.size()-1].y, points[points.size()-1].z);
        
        boidSphereRot += boidSphereRotUpdate;
        ofRotateZ(boidSphereRot);
        ofRotateX(boidSphereRot/2);
        ofRotateY(boidSphereRot);
        
        //ofLogNotice() << "rotate " << boidSphereRot;
        
        meshSphere = bSphere.getMesh();
        meshSphereWarped = meshSphere;
        vector<ofVec3f> & vertsSphereOrig = meshSphere.getVertices();
        vector<ofVec3f> & vertsSphereWarped = meshSphereWarped.getVertices();
        
        //sound boidsphere
        int numMeshVerts = meshSphere.getNumVertices();
        float * audioData = new float[numMeshVerts];
        fftLive.getFftPeakData(audioData, numMeshVerts);
        float audioVolume = 0;
        
        
        if(drawAntiboids){
            randomPoint.set(meshSphere.getVertex(ofRandom(0,numMeshVerts)));
//            randomPoint.set(ofRandom(boidSphereSize),ofRandom(boidSphereSize),ofRandom(boidSphereSize));
//            ofLog() << "rndP" << randomPoint;
            
            drawAntiboids = false;
//            seekRandomPoint = true;
        }
        
        int j = 0;
        int k = 0;
        int l = 0;
        for (int i = 0; i < numMeshVerts; i++){
            float audioValue = audioData[i];
            ofVec3f & vertOriginal = vertsSphereOrig[i];
            ofVec3f & vertWarped = vertsSphereWarped[i];
            ofVec3f directionSphere = vertOriginal.getNormalized();
            vertWarped = vertOriginal + directionSphere * 300 * audioValue  ;

            
            if (j < boidNum &&  i < (numMeshVerts - (numMeshVerts/4))){ //i > (numMeshVerts/4) &&
                boids[j].maxForce = 0.3;
                boids[j].seek(meshSphereWarped.getVertex(i*4));
                if(!newBoid){
                    boids[j].evade(boids3[l]);
                }
                if(seekRandomPoint){
                    boids[j].seek(randomPoint);
                }
                boids[j].update();
                j++;
            }
            if (k < antiBoids.size() && i < numMeshVerts/20){
                
                antiBoids[k].maxForce = 0.4;
                if(!newBoid){
                    antiBoids[k].evade(boids3[l]);
                }
                if(seekRandomPoint){
                    antiBoids[k].seek(randomPoint);
                }
                else{
                    antiBoids[k].seek(meshSphereWarped.getVertex(i*20));
                }
                antiBoids[k].update();
                k++;
                
                //ofDrawTriangle(meshSphereWarped.getVertex(i), antiBoids[k].position, boids[j].position);
            }
            if (l < boidNum3 && i < numMeshVerts/2){
                for (int m = 0; m < 6; m++ ){
                    boids3[l+m].seek(meshSphereWarped.getVertex(i*2));
                    if(!newBoid){
                        boids3[l+m].evade(antiBoids[k]);
                        boids3[l+m].tooCloseDist = 50;
                    }
                    if(seekRandomPoint){
                        boids3[l+m].seek(randomPoint);
                    }
                    boids3[l+m].update();
                }
                l = l+6;
            }
        }
        delete[] audioData;
    }
    // ------------------------ end boids sphere -------------------


    //boids update
    int pSize = points.size();
    int antiSize = antiBoids.size();
    
    
    
    
    if(!boidsSphere){
        // ----- boids 1 ------
        for (int i = 0; i < boidNum; i++){
            boids[i].update();
            if(points.size()>boidNum*10){
                if(!reverse){
                    boids[i].seek(points[points.size()-1-(i*10)+i]); //ofRandom(0,10)
                    if(newBoid){
                        boids[i].evade(antiBoids[antiSize-1]);
                    }
                }else{
                    boids[i].seek(points[pSize/(i+1)]);
                    boids[i].wander();
                }
            }
        }
        // ----- boids 2 ------
        for (int i = 0; i < antiBoidNum; i++){
            if (antiSize > 1 && pSize > antiBoidNum*8 && newBoid){
                antiBoids[i].flock(antiBoids);
                antiBoids[i].seek(points[points.size()-1-(i*8)+i]);
                if(!newBoid){
                    antiBoids[i].flock(antiBoids);
                    antiBoids[i].flee(points[points.size()-1-(i*8)+i]);
                }
            }
            
            antiBoids[i].update();
        }
        
        // ----- boids 3 ------
        int boid3n = 0;
        for (int i = 0; i < boidNum; i++){
            for(int j = 0; j<boid3fac; j++){
                if(!newBoid){
                    boids3[boid3n].pursue(boids[i]);
                }else{
                    boids3[boid3n].pursue(antiBoids[0]);
                }
                boids3[boid3n].update();
                boid3n++;
            }
        }
        
    }
    
    
    if (boidsSphere){
        for (int i = 0; i < boidNum; i++){
            glPushMatrix();
            glTranslatef(boids[i].position.x, boids[i].position.y, boids[i].position.z);
            ofSetColor(ofColor::fromHsb(i, 200, 200, 200));
            icoSphereBoids1.draw();
            glPopMatrix();
        }
    }
    
    if (boidsSphere || drawAntiboids){
        ofSetColor(ofColor::fromHsb(150, 100, 140, 100));
        for (int i = 0; i< antiBoidNum; i++){
            if (antiSize > 1){
                glPushMatrix();
                glTranslatef(antiBoids[i].position.x, antiBoids[i].position.y, antiBoids[i].position.z);
                icoSphereBoids2.draw();
                glPopMatrix();

            }
        }
    }
    
    if (boidsSphere){
        ofSetColor(ofColor::fromHsb(50, 200, 200, 200));
        
        for (int i = 0; i< boidNum3; i++){
            glPushMatrix();
            glTranslatef(boids3[i].position.x, boids3[i].position.y, boids3[i].position.z);
            boxBoids3.draw();
            
            glPopMatrix();
        }
    }
    

    float colorHue = abs(sin(ofGetElapsedTimef()/colorChanging))*255;
    //ofLogNotice() << "colorHue: " << colorHue;
    float sat = abs(sin(ofGetElapsedTimef()/3))*50;
    float bri = abs(cos(ofGetElapsedTimef()/4))*50;

    
    // material? todo
    ofSetColor(ofColor::fromHsb(colorHue, 200-sat, 200+bri, 190)); // transparent?

    if(!boidsSphere && !bDrawParticles){
        ofVec3f ribbonTarget;
        if(pSize > 0){
            ribbonTarget.set( points[pSize-1]);
        }else{
            ribbonTarget.set(0,0,0);
        }
        if(bAutoCam){
            camDistSpeed = camera.getDistance();
            
            camDistSpeed += camDistSpeedUpdate; //cameraSpeed
            camRotSpeed += camRotSpeedUpdate;
            camLatSpeed += camLatSpeedUpdate;
            camera.orbit(camRotSpeed , camLatSpeed, camDistSpeed);
            
        }
        meshRibbon.draw();
    }
//    else{
//       // meshRibbon.draw();
////        meshSphereWarped.drawWireframe();
//    }
    
    if(usecamera){
        camera.end();
    }
    if(useDOF){
        depthOfField.end();
        depthOfField.getFbo().draw(0,0);
    }
    
    ofSetColor(255);
   
    ofDisableDepthTest();
    ofDisableLighting();
    lightRibbon.disable();
}

//--------------------------------------------------------------
void ofApp::setupDanceCloud(){
//    glDisable(GL_LIGHTING);
//    glDisable(GL_LIGHT0);
    
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetBackgroundAuto(true);

    bDrawFocusAssist = false;
    
    time0 = 0;
    n = nBegin;
    
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }
    
    //Initialize points offsets by random numbers
    for ( int j=0; j < nMax; j++ ) {
        tx[j] = ofRandom( 0, 1000 );
        ty[j] = ofRandom( 0, 1000 );
        tz[j] = ofRandom( 0, 1000 );
    }
    
    lightCloud.setup();
    
    //material settings
    material.setShininess(50);
    material.setSpecularColor(ofColor(30, 200, 30, 15));
    material.setEmissiveColor(ofColor(90, 30, 30, 100));
    material.setDiffuseColor(ofColor(0, 90, 90));
    material.setAmbientColor(ofColor(50, 0, 200));
    

}

//--------------------------------------------------------------
void ofApp::updateDanceCloud(){
    
    //swim the depth of field
    //    depthOfField.setFocalDistance(ofMap(sin(ofGetElapsedTimef()/2),-1,1, 20, 150));
    depthOfField.setFocalDistance(myfocaldistance);
    depthOfField.setFocalRange(myfocalrange);

    
    
    //Get current spectrum with N bands
//    float *val = ofSoundGetSpectrum( N );
    //We should not release memory of val,
    //because it is managed by sound engine
    
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];

    fftLive.getFftPeakData(audioData, numOfVerts);
    
    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<N; i++ ) {
        spectrum[i] *= 0.97;    //Slow decreasing 0.97
        spectrum[i] = max( spectrum[i], audioData[i] );
    }
    
    //Update particles using spectrum values
    
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp( dt, 0.0, 0.1 );
    time0 = time; //Store the current time
    
    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    Rad = ofMap( spectrum[ bandRad ], 1, 3, 100, 200, true ); //1, 3, 400, 800, true );
    Vel = ofMap( spectrum[ bandVel ], 0, 0.1, 0.02, 0.1 ); //0, 0.1, 0.05, 0.5 );
    
    
    // ------------------ MIDI -----------------------

    if(midiMessage.control == 32 && midiMessage.value == 127){ //solo 1 == cc32 drawLine
        dLine = true;
    }
    if(midiMessage.control == 48 && midiMessage.value == 127){ //mute 1 == cc48 drawLine
        dLine = false;
    }

    if(midiMessage.control == 2){ // fader 3    vorocells size
        cellSize  = ofMap(midiMessage.value, 0, 127, 0, 1000);
    }

    
    // ------------------ MIDI end -----------------------
    


    //update particle amount
    if (n < nMax){
        if (time > (timeCount + pointUpdateTime)){
            timeCount = time;
            n++;
        }
    }
    
    //Update particles positions
    for (int j=0; j<nMax; j++) {
        tx[j] += Vel * dt * guiNoise;    //move offset
        ty[j] += Vel * dt * guiNoise;    //move offset
        tz[j] += Vel * dt * guiNoise;
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        p[j].x = ofSignedNoise( tx[j] ) * Rad * danceSize;
        p[j].y = ofSignedNoise( ty[j] ) * Rad * danceSize;
        p[j].z = ofSignedNoise( tz[j] ) * Rad * danceSize;
    }
        
    //update cells
    bool press = ofGetKeyPressed();
    voro::container con(-cellSize,cellSize,
                        -cellSize,cellSize,
                        -cellSize,cellSize,
                        1, 1, 1, 1, 1, 1, 8); //1, 1, 1, press, press, press, 8);
    
    for(int i = 0; i < n ;i++){
        if ( insideContainer( con, p[i])){
            addCellSeed( con, p[i], i);
        }
    }
    
    cellMeshes.clear();
    cellMeshes = getCellsFromContainer(con,0);
    cellMeshWireframes.clear();
    cellMeshWireframes = getCellsFromContainer(con,0,true);
}

//--------------------------------------------------------------
void ofApp::drawDanceCLoud(){
    
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableDepthTest();
    ofEnableSmoothing();
    //Draw cloud
    
    depthOfField.begin();
    
    camera.begin(depthOfField.getDimensions());
    
    if(bAutoCam){
        camDistSpeed = camera.getDistance();
        
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        camLatSpeed += camLatSpeedUpdate;
        camera.orbit(camRotSpeed , camLatSpeed, camDistSpeed);
        
    }
    
    
    ofPushStyle();
    //Draw speheres
    if(!bDrawTriangleCloud){
        for (int i=0; i<n; i++) {
            ofSetColor( 230-i,  70+i, 60,  230 );
            ofFill();
            ofDrawSphere(p[i], (3 + (i*0.02)));
            
        }
    }
    
    //Draw triangles between near points
    if(bDrawTriangleCloud){
        
        int triangleMax = 0;
        ofPoint triangleCenter = ofPoint(0,0,0);
        float dist = 50;    //Threshold parameter of distance 40
        for (int j=0; j<n-1; j++) {

            if (  triangleMax < 60) {
                    triangleMax++;
                    ofSetColor(ofColor::fromHsb(100+(j*5), 200-j, 200-j, 150)); //ofClamp(j,100,130)
                
                    ofDrawTriangle( triangleCenter, p[j], p[j+1]);
                }
        }
    }
    
    //Draw lines between near points
    if(dLine && !bDrawTriangleCloud){
        float dist = 60;    //Threshold parameter of distance 40
        for (int j=0; j<n; j++) {
            for (int k=j+1; k<n; k++) {
                float curDist = ofDist( p[j].x, p[j].y, p[j].z,  p[k].x, p[k].y, p[k].z);
                if(curDist < dist) {
                    ofSetColor(250, 180-(curDist*3));
                    ofSetLineWidth(3);
                    ofDrawLine( p[j], p[k] );
                }
                
            }
        }
    }
    ofPopStyle(); //Restore coordinate system
    
    
    // -- Voro Cells
    if(!bDrawTriangleCloud){
        for(int i = 0; i < cellMeshes.size(); i++){
            ofSetColor(0,200,150, 30);
            cellMeshes[i].drawFaces();
            
            ofPushStyle();
            ofSetLineWidth(3);
            ofSetColor(0,255,255);
            cellMeshWireframes[i].draw();
            ofPopStyle();
        }
    }
    
    camera.end();
    depthOfField.end();
    
    
    if(bDrawFocusAssist){
        depthOfField.drawFocusAssist(0, 0);
    }
    else{
        depthOfField.getFbo().draw(0, 0);
    }
    
    
    if(0){
        // draw the last recieved message contents to the screen
        text << "Received: " << ofxMidiMessage::getStatusString(midiMessage.status);
        ofDrawBitmapString(text.str(), 20, 20);
        text.str(""); // clear
        
        text << "channel: " << midiMessage.channel;
        ofDrawBitmapString(text.str(), 20, 34);
        text.str(""); // clear
        
        text << "pitch: " << midiMessage.pitch;
        ofDrawBitmapString(text.str(), 20, 48);
        text.str(""); // clear
        ofDrawRectangle(20, 58, ofMap(midiMessage.pitch, 0, 127, 0, ofGetWidth()-40), 20);
        
        text << "velocity: " << midiMessage.velocity;
        ofDrawBitmapString(text.str(), 20, 96);
        text.str(""); // clear
        ofDrawRectangle(20, 105, ofMap(midiMessage.velocity, 0, 127, 0, ofGetWidth()-40), 20);
        
        text << "control: " << midiMessage.control;
        ofDrawBitmapString(text.str(), 20, 144);
        text.str(""); // clear
        ofDrawRectangle(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth()-40), 20);
        
        text << "value: " << midiMessage.value;
        ofDrawBitmapString(text.str(), 20, 192);
        text.str(""); // clear
        if(midiMessage.status == MIDI_PITCH_BEND) {
            ofDrawRectangle(20, 202, ofMap(midiMessage.value, 0, MIDI_MAX_BEND, 0, ofGetWidth()-40), 20);
        }
        else {
            ofDrawRectangle(20, 202, ofMap(midiMessage.value, 0, 127, 0, ofGetWidth()-40), 20);
        }
        
        text << "delta: " << midiMessage.deltatime;
        ofDrawBitmapString(text.str(), 20, 240);
        text.str(""); // clear
        
        
        text << "points: " << p[1].x << " " << p[1].y << " " << p[1].z;
        ofDrawBitmapString(text.str(), 20, 300);
        text.str(""); //clear
        
        ofSetColor(255);
    }
}


//--------------------------------------------------------------
void ofApp::setupSpectroSphere(){

    ofSetBackgroundAuto(true);

#ifdef TARGET_OPENGLES
    shader.load("shadersES2/shader");
#else
    if(ofIsGLProgrammableRenderer()){
        shader.load("shadersGL3/shader");
    }else{
        shader.load("shadersGL2/shader");
    }
#endif
    
    ofHideCursor();
    
    img.allocate(80, 60, OF_IMAGE_GRAYSCALE);
    
    box.set(40, 40, 80, 30, 30, 100);
    
    sphere.set(200, resolution);
    sphere.mapTexCoordsFromTexture(img.getTexture());
    
    ofSetBackgroundColor(0);
    
    ofSetSmoothLighting(true);
    
    
    //  ---------- light --------
    center.set(ofGetWidth()*.5, ofGetHeight()*.5, 0);
    
    lightSpectroSph.setup();
    // Point lights emit light in all directions //
    // set the diffuse color, color reflected from the light source //
    lightSpectroSph.setDiffuseColor( ofColor(0.f, 255.f, 0.f));
    
    // specular color, the highlight/shininess color //
    lightSpectroSph.setSpecularColor( ofColor(255.f, 255.f, 255.f));
    lightSpectroSph.setPosition(center.x, center.y, 0);
    
    // shininess is a value between 0 - 128, 128 being the most shiny //
    materialSpectroSph.setShininess( 64 );
    lightColor.setBrightness( 180.f );
    lightColor.setSaturation( 150.f );
    
    colorHue = ofRandom(0, 250);
    
    materialColor.setBrightness(250.f);
    materialColor.setSaturation(200);
    //------------
}

//--------------------------------------------------------------
void ofApp::updateSpectroSphere(){
    float noiseScale = ofMap(mouseX, 0, ofGetWidth(), 0, 0.1);
    float noiseVal = ofGetElapsedTimef()/3;
    
    // -------------- perlin noise ----------------
    ofPixels & pixels = img.getPixels();
    int w = img.getWidth();
    int h = img.getHeight();
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            int i = y * w + x;
            if (x<w/2){
                float noiseValue = ofNoise(x * noiseScale, y * noiseScale, noiseVal);
                pixels[i] = 255 * noiseValue;
            }
            else{
                pixels[i] = pixels[((y+1) * w - x) -1];
            }
        }
    }
    img.update();
    // ------ end perlin noise -------------------
    
//    fftLive.setPeakDecay(0.9); //audioPeakDecay
//    fftLive.setMaxDecay(0.9); //audioMaxDecay
    fftLive.setThreshold(audioThreshold);
    fftLive.setPeakDecay(audioPeakDecay);
    fftLive.setMaxDecay(audioMaxDecay);
  
    // ----------------- sphere sound ----------------------
    sphereOrig = sphere.getMesh();
    sphereWarped = sphereOrig;
    vector<ofVec3f> & vertsOriginal = sphereOrig.getVertices();
    vector<ofVec3f> & vertsWarped = sphereWarped.getVertices();
    
    int numOfVerts =  sphereOrig.getNumVertices();
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    for(int i=0; i<numOfVerts; i++) {
        float audioValue = audioData[i];
        ofVec3f & vertOriginal = vertsOriginal[i];
        ofVec3f & vertWarped = vertsWarped[i];
        ofVec3f direction = vertOriginal.getNormalized();
        vertWarped = vertOriginal + direction * meshDisplacement * audioValue  ;
        
    }
    delete[] audioData;
    
    // --------------- Box sound ---------------------------
    if(0){
        boxOrig = box.getMesh();
        boxWarped = boxOrig;
    
        vector<ofVec3f> & vertsOriginalBox = boxOrig.getVertices();
        vector<ofVec3f> & vertsWarpedBox = boxWarped.getVertices();
        int numOfVertsBox = boxOrig.getNumVertices();
        float * audioDataBox = new float[numOfVertsBox];
        fftLive.getFftPeakData(audioDataBox, numOfVertsBox);
    
        for(int i=0; i<numOfVertsBox; i++) {
            float audioValue = audioDataBox[i];
            ofVec3f & vertOriginalBox = vertsOriginalBox[i];
            ofVec3f & vertWarpedBox = vertsWarpedBox[i];
            ofVec3f direction = vertOriginalBox.getNormalized();
            vertWarpedBox = vertOriginalBox + direction * meshDisplacement * audioValue  ;
        }
        delete[] audioDataBox;
    }
    //--------- light ----------
    colorHue += .1f;
    if(colorHue >= 255) colorHue = 0.f;
    lightSpectroSph.enable();
    lightColor.setHue(colorHue);
    lightSpectroSph.setDiffuseColor(lightColor);
    materialColor.setHue(colorHue);
    // the light highlight of the material //
    materialSpectroSph.setSpecularColor(materialColor);
    
    // polyline
    float angle = 0;
    while (angle < TWO_PI ) {
        currentPolyline.curveTo( ofPoint(100*cos(angle), 100*sin(angle)));
        // currentPolyline.curveTo( ofPoint(300*cos(angle), 300*sin(angle)));
        angle += TWO_PI / 30;
    }
}

//--------------------------------------------------------------
void ofApp::drawSpectroSphere(){
    
    ofSetColor(255);
    
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableDepthTest();
    

    
    // translate plane into center screen.
    float tx = ofGetWidth() / 2;
    float ty = ofGetHeight() / 2;
    
    // the mouse/touch Y position changes the rotation of the plane.
    float percentY = mouseY / (float)ofGetHeight();
    float rotation = ofMap(percentY, 0, 1, -60, 60, true) + 60;
    //ofRotate(rotation, 1, 1, 1);
    
    ofPushMatrix();
    cam.begin();
    
    ofEnableLighting();
    lightSpectroSph.enable();
    materialSpectroSph.begin();
    
    //boxRotation += 0.1;
    
    if(bAutoCam){
        camDistSpeed = cam.getDistance();
        
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        camLatSpeed += camLatSpeedUpdate;
        camera.orbit(camRotSpeed , camLatSpeed, camDistSpeed);
        
    }
    
//    ofRotate(90, 0, 0, 1);
//    ofRotate(boxRotation ,0,0,1);
    ofSetLineWidth(2);
    
    if(0){
        boxWarped.draw();
    }
    
    materialSpectroSph.end();
    ofDisableLighting();


    
    img.getTexture().bind();
    shader.begin();
    ofRotate(-90 ,0,0,1);
    
    // give color to shader
//    float sphereColor[4] = {0, 200 , 0, 100};
    float sphereColor[4] = {80, 0 , 130, 100};
    shader.setUniform4fv("sphereColor", &sphereColor[0]);
    
   // ofTranslate(-100,0,0);
    ofRotate(boxRotation ,1,0,0.2);
    sphereWarped.draw();
    
    shader.end();
    img.getTexture().unbind();
    cam.end();
    
    
    ofPopMatrix();
    
    //ofSetColor(ofColor(255));
    //    img.draw(0, 0);
    ofSetColor(255);
    lightSpectroSph.disable();

}

//--------------------------------------------------------------
void ofApp::closeSpectroSphere(){
    shader.unload();
    ofDisableLighting();
    lightSpectroSph.disable();
    img.clear();
}

//--------------------------------------------------------------
void ofApp::setupFleeingCells(){
    

    makeTissue(nMax, ofGetHeight()*0.8, ofGetHeight()*0.8, 30);
    subtractSeed = 0;
    timeAdd = 0;
    exploding = false;
    
    
    //Initialize points offsets by random numbers
    for ( int j=0; j < cellCentroids.size(); j++ ) {
        tx[j] = cellCentroids[j].x;
        ty[j] = cellCentroids[j].y;
        tz[j] = cellCentroids[j].z;
        p[j] = cellCentroids[j];
    }
    
//    for (int i = 0; i < cellCentroids.size(); i++){
//        p[i] = cellCentroids[i];
//    }
    
    ofEnableLighting();
    cam.setDistance(1000);
    
    lightRibbon.setup();
    
    areaLight.setup();
    areaLight.setAreaLight(1000,1000);
    
}
//--------------------------------------------------------------
void ofApp::makeTissue(int _nCells, int _width, int _height, int _deep){
    
    bDrawFocusAssist = false;
    
    //  Fresh beginning
    //
    cellMeshes.clear();
    cellCentroids.clear();
    cellRadius.clear();
    
    //  Define a container
    //
    voro::container con(-_width*0.5,_width*0.5,
                        -_height*0.5,_height*0.5,
                        -_deep*0.5,_deep*0.5,
                        1,1,1,
                        true,true,true,
                        8);
    
    //  Add walls (un comment one pair if you like to shape the container)
    //
    voro::wall_sphere sph(0, 0, 0, min(_width*0.5, _height*0.5) );
    con.add_wall(sph);
    
    //  Add the cell seed to the container
    //
    for(int i = 0; i < _nCells;i++){
        ofPoint newCell = ofPoint(ofRandom(-_width*0.5,_width*0.5),
                                  ofRandom(-_height*0.5,_height*0.5),
                                  ofRandom(-_deep*0.25,_deep*0.25));
        
        addCellSeed(con, newCell, i, true);
    }
    
    cellMeshes = getCellsFromContainer(con,0.0);
    cellMeshWireframes = getCellsFromContainer(con,0.0,true);
    cellRadius = getCellsRadius(con);
    cellCentroids = getCellsCentroids(con);
}

//--------------------------------------------------------------
void ofApp::updateFleeingCells(){
    
    int freeingSpeed = ofRandom(8,20);  // 10,40 ?

    
    depthOfField.setFocalDistance(myfocaldistance);
    depthOfField.setFocalRange(myfocalrange);
    
    //ofLogNotice() << time << ofGetElapsedTimef();

    freeOne = false;
    if(ofGetElapsedTimef() > timeAdd + freeingSpeed){
        timeAdd = ofGetElapsedTimef();
        freeOne = true;
        if(subtractSeed < cellCentroids.size()){
            subtractSeed++;
        }
    }
    
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp( dt, 0.0, 0.1 );
    time0 = time; //Store the current time
    
    //Update particles positions
    for (int j=0; j<subtractSeed; j++) {
        tx[cellCentroids.size()-j-1] += Vel * dt * guiNoise;    //move offset
        ty[cellCentroids.size()-j-1] += Vel * dt * guiNoise;    //move offset
        tz[cellCentroids.size()-j-1] += Vel * dt * guiNoise;
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        p[cellCentroids.size()-j-1].x = ofSignedNoise( tx[cellCentroids.size()-j-1] ) * Rad * danceSize;
        p[cellCentroids.size()-j-1].y = ofSignedNoise( ty[cellCentroids.size()-j-1] ) * Rad * danceSize;
        p[cellCentroids.size()-j-1].z = ofSignedNoise( tz[cellCentroids.size()-j-1] ) * Rad * danceSize;
    }
    
    if(freeOne || exploding){
        p[cellCentroids.size()-subtractSeed-1]   = cellCentroids[cellCentroids.size()-subtractSeed];
    }
}

//--------------------------------------------------------------
void ofApp::drawFleeingCells(){
    
    ofBackground(0);
    
    ofEnableLighting();
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
//    ofEnableDepthTest();
    ofEnableSmoothing();
    
    
    depthOfField.begin();
    cam.begin(depthOfField.getDimensions());
    areaLight.enable();
    ofEnableLighting();
    glEnable(GL_DEPTH_TEST);
    
    lightRibbon.enable();
    
    
    if(bAutoCam){
        camDistSpeed = cam.getDistance();
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        cam.orbit(camRotSpeed , abs(camRotSpeed/3), camDistSpeed );
    }
    
    // Exploding cell
    if(freeOne){
        timeExplode = ofGetElapsedTimef();
        explodingPoint = cellCentroids[cellCentroids.size()-subtractSeed];
        p[cellCentroids.size()-subtractSeed] = explodingPoint;
    }
    if(freeOne || exploding){
        
        if(explodingSize<30){
            explodingSize += 3;
        }
        ofSetColor(150, 100, 80, 100);
        ofDrawBox(explodingPoint, explodingSize);
        
        if (ofGetElapsedTimef() < timeExplode + 2 ){
            exploding = true;
        }else{
            timeExplode = ofGetElapsedTimef();
            exploding =false;
            explodingSize = 20;

        }

    }
    
//    ofLogNotice() << "exps " << exploding;
//    ofLogNotice() << "expsize " << explodingSize;
    
    
    for (int i = 0; i < cellCentroids.size()-subtractSeed; i++){
        ofSetColor(250, 100, 100);
        ofDrawSphere(cellCentroids[i], cellRadius[i]*0.15);
    }
    for (int i = 0; i < subtractSeed; i++){
        ofSetColor(250, 100, 20);
        ofDrawSphere(p[cellCentroids.size()-i-1], cellRadius[i]*0.15);
    }

    
    for(int i = 0; i < cellMeshes.size()-subtractSeed; i++){
        ofSetColor(30, 250, 40, 240);
        cellMeshes[i].draw();
        
        ofPushStyle();
        ofSetLineWidth(3);
        ofSetColor(0,50,100);
        cellMeshWireframes[i].draw();
        ofPopStyle();
    }
    
    
    glDisable(GL_DEPTH_TEST);
    ofDisableLighting();
    areaLight.disable();
    lightRibbon.disable();
    cam.end();
    depthOfField.end();
    
    if(bDrawFocusAssist){
        depthOfField.drawFocusAssist(0, 0);
    }
    else{
        depthOfField.getFbo().draw(0, 0);
    }
}

//-------------------  Growing Lines : Setup  --------------------
void ofApp::setupGrowingLines(){
    ofBackground(250);
    
    int xstart = 0;
    int ystart = 0;
    int zstart = 0;
    
    xgrow = xstart;
    ygrow = ystart;
    zgrow = zstart;
    
    drawnPoints.clear();
    lines.clear();
    linesMesh.clear();
    camTarget.setPosition(0,0,0);
    
    
    displacementScaleGrowLin = 0.0;
    displacementScaleGrowLin2 = 1.0;
    
    
    bDrawLineMesh = false;
    
    bDrawWireframe = true;
    
    colorChanging   = 100;
    
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }
    
    bAutoCam = true;
}

//------------------  Growing Lines : Update  ------------------------
void ofApp::updateGrowingLines(){
    float growsize = 20;
    
    time = ofGetElapsedTimef();
    
    
    // sound-----
    int numOfVerts = numVertsGlobal;
    float volume = 0;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    
    for ( int i=0; i<numVertsGlobal; i++ ) {
        spectrum[i] *= 0.97;    //Slow decreasing 0.97
        spectrum[i] = max( spectrum[i], audioData[i] );
        volume += spectrum[i];
    }
    //-----
    
    //---------- MIDI ---------------
    
    if(midiMessage.control == 20){ // rad 5
       displacementScaleGrowLin  = ofMap(midiMessage.value, 0, 127, 0.0, 3.0);
    }
    if(midiMessage.control == 4){ // fader 5
        displacementScaleGrowLin2  = ofMap(midiMessage.value, 0, 127, 1.0, 3.0);
    }
    
    if(midiMessage.control == 6){ // fader 7
        colorChanging = ofMap(midiMessage.value, 0, 127, 0.5, 100);
    }
    
    //---------- end MIDI ------------
    
    if(bGrowLine){
        xgrow += ofRandom(-growsize, growsize);
        ygrow += ofRandom(-growsize, growsize);
        zgrow += ofRandom(-growsize, growsize);
//        ofLogNotice() << "xgrow " << xgrow;
//        ofLogNotice() << "ygrow " << ygrow;
//        ofLogNotice() << "zgrow " << zgrow;

        
        xgrow +=  ofRandom(growsize, -growsize);
        ygrow +=  ofRandom(growsize, -growsize);
        zgrow +=  ofRandom(growsize, -growsize);
        
        for (auto point : drawnPoints){
            ofPoint mouse;
            mouse.set(xgrow, ygrow, zgrow);
            float dist = (mouse - point).length();
            if (dist < 30){
                Line lineTemp;
                lineTemp.a = mouse;
                lineTemp.b = point;
                lines.push_back(lineTemp);
            }
        }
        drawnPoints.push_back(ofPoint(xgrow,ygrow, zgrow));
        linesMesh.addVertex(ofPoint(xgrow,ygrow, zgrow));
        offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
        camTarget.setPosition(drawnPoints[drawnPoints.size()-1]);
    }
    
    
    
    for (int i = 0; i < drawnPoints.size(); i++){
        
        ofVec3f vert = linesMesh.getVertex(i);
//        timeScale = 5.0;
//        displacementScale = 0.75;
        //offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
        ofVec3f timeOffsets = offsets[i];
        
        vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScaleGrowLin * volume;
        vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScaleGrowLin * volume;
        vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScaleGrowLin * volume;
        linesMesh.setVertex(i, vert);
        
    }
    
    delete [] audioData;
}

//----------------  Growing Lines : Draw  ----------------------------
void ofApp::drawGrowingLines(){
    ofEnableAlphaBlending();
    
    if(bAutoCam){
        camDistSpeed = cam.getDistance();
        camDistSpeed += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed += camRotSpeedUpdate;
        cam.orbit(camRotSpeed , abs(camRotSpeed/3), camDistSpeed );
        cam.getTarget();

    }
    

    
    cam.begin();
    
    //ofSetColor(230,230,230,30);
    ofSetColor(5,5,5,90);
    
    float degrees = ofGetElapsedTimef();
    cam.rotate(degrees, ofVec3f(0,0,0));
    
    if(!bDrawLineMesh){
        for (auto line : lines) {
            ofSetLineWidth(1);
            ofDrawLine(line.a, line.b);
            
        }
        //ofSetLineWidth(3);
        
//        linesMesh.drawWireframe();
//        linesMesh.draw();

    }
    else{
        float colorHue = abs(sin(ofGetElapsedTimef()/colorChanging))*255;
        //ofLogNotice() << "colorHue: " << colorHue;
        float sat = abs(sin(ofGetElapsedTimef()/3))*50;
        float bri = abs(cos(ofGetElapsedTimef()/4))*50;
        
      
        
        if(!bDrawWireframe){
             // ofSetColor(ofColor::fromHsb(colorHue, 250-sat, 150+bri, 230));
            linesMesh.draw();
        }else{
         //   ofSetColor(ofColor::fromHsb(colorHue + 20, 250-sat + 30, 150+bri +30, 230));
            ofSetLineWidth(2);
            linesMesh.drawWireframe();
        }
        
    }
    
    cam.end();
    ofSetColor(255);

}
//----------------- String Of Pearls : Setup -------------------
void ofApp::setupStringOfPearls(){

    // load the texure
//    ofDisableArbTex();
//    ofLoadImage(glowingTexture, "particleGlow/dot.png");
//    glowingShader.load("particleGLow/particle_glow_shader");

//    ofBackground(245, 245, 220); //beige
    ofBackground(0);
    
    camDistSpeed    = 0;
    camRotSpeed     = 0;
    camera.reset();
    
    bAutoCam        = true;
    
    camera.setFarClip(12000);
    
    if(bDraw3DStripes){
        camera.setDistance(1000);
    }
    
    line3Dreaction  = 0;
    boxRotation     = 0;
    
    ofSetBackgroundAuto(true);
    pearlsXoffset = 0;

}
//----------------- String Of Pearls : update ------------------
void ofApp::updateStringOfPearls(){
    

    // MIDI control - Pearls
    if(midiMessage.control == 4){ // fader 5 für pearls offset & line offset
        pearlsXoffset = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    if(midiMessage.control == 20){ // rad 5 für pearls Size of Field & reaction of lines
        pearlsSize = ofMap(midiMessage.value, 0, 127, 0.1, 1);
    }
    
    if(midiMessage.control == 5){ // fader 6  line offset
        oldLine3Doffset = line3Doffset;
        newLine3Doffset = ofMap(midiMessage.value, 0, 127, 0.0, 400);
        newValue = true;
    }
    if(midiMessage.control == 21){ // rad 6  line offset
        line3Dreaction = ofMap(midiMessage.value, 0, 127, 0.0, 1.0);
    }
    
    // MIDI Value smoothening
    if (newValue){
        if (line3Doffset < newLine3Doffset - 2){
            line3Doffset = oldLine3Doffset + 4;
        }
        if (line3Doffset > (newLine3Doffset + 2) ){
            line3Doffset = oldLine3Doffset - 4;
        }
        else{
            line3Doffset = newLine3Doffset;
            newValue = false;
        }
    }
    
}

//----------------- String Of Pearls : Draw --------------------
void ofApp::drawStringOfPearls(){
    int xMid = ofGetWidth()/2;
    int yMid = ofGetHeight()/2;
    int chessResolution = 1;
    
    bool bBoxes          = bDrawBoxes;
    bool bCircles        = bDrawCircles;
    bool bCosStripes     = bDrawCosStripes;
    bool bBarcodeStripes = bDrawBarcodeStripes;
    bool b3DStripes      = bDraw3DStripes;
    bool bSingleWorm     = bDrawSingleWorm;
    bool bPearlField     = bDrawPearlField;
    bool bWorm2          = bDrawWorm2;
    
    camera.begin();
    if(bAutoCam && !bBarcodeStripes && !bCosStripes){
        camDistSpeed    = camera.getDistance();
        camDistSpeed    += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed     += camRotSpeedUpdate;
        camRotSpeed     = (!b3DStripes) ? camRotSpeed : 0;
        camLatSpeed += camLatSpeedUpdate;
        camLatSpeed     = (!b3DStripes) ? camLatSpeed : 0;
        camera.orbit(camRotSpeed , camLatSpeed, camDistSpeed);
        
    }
    
    if(bCosStripes || bBarcodeStripes){
        camera.reset();
    }
    
    int spacing = 80;
//    ofBeginShape();
//    ofTranslate(-ofGetWidth()/2,  -ofGetHeight()/2, -300);
    ofTranslate(-ofGetWidth()/2,  -ofGetHeight()/2, 0);

    if(bPearlField){
        ofTranslate(-ofGetWidth()/2,  -400, -500);
    }
    
    // offset für entfernung der Linien
    float lineOffset = line3Doffset;
    

    // Sound -----
    float freqMax = 0;
    int numOfVerts = 32;
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    float audioVolume = 0;
    for (int i = 0; i<numOfVerts; i++){
        audioVolume += audioData[i];
        ofSetColor(255);
        
//        //get max frequency
//        if( audioData[i] > audioData[i-1] && i > 0 && i<=numOfVerts/2){
//            freqMax = i;
//        }
        
        //draw spectrum
        if(0){
            ofDrawRectangle(i*10, 0, 10, audioData[i]*100);
        }
        
        // auslenkung der Linien
        float lineReaction = line3Dreaction * audioData[i] * xMid/2 + xMid;
        
        // Boxes -----
        if(bBoxes){
            ofSetBackgroundAuto(true);  // TODO!!! dirty hack...
            
            ofSetColor(255, 0, i*8, i*5 + 50);
            ofNoFill();
            ofSetLineWidth(4);
            
            boxRotation = 6 * pearlsXoffset;
            ofRotate(boxRotation * ofGetElapsedTimef()*0.2, xMid+(ofGetElapsedTimef()*4), yMid-300, -55);

            ofDrawBox(xMid, yMid, 0, 50 + 150*audioData[i]);
        }
        // Circles ----
        if(bCircles){
         //   ofSetBackgroundAuto(true);  // TODO!!! dirty hack...
            for (int j = 1; j<6; j++){
                ofSetColor(110, 30+80*audioData[i], i*8, i*5 + 150 - 20*j);
                ofNoFill();
                ofSetLineWidth(3*audioData[i]+1);
                ofSetCircleResolution(60);
                
//                boxRotation = 0.08 * pearlsXoffset + 0.001;
//                ofRotate(boxRotation * ofGetElapsedTimef()*2, xMid, yMid, -5);
                
                boxRotation += 0.0003 * pearlsXoffset;
                ofRotate(boxRotation , xMid, yMid, -5);
                
                
               // ofSetCircleResolution(5);
                ofDrawCircle(xMid, yMid , audioData[i]*100, 20*i*j + 10*audioData[i]);
            }
        }
        
        // Cosine Stripes -----
        if(bCosStripes){
            ofFill();
            
            float speed = 0.1;
            ofSetColor(255, 10, 10, 255-audioData[i]);
            int distance = yMid/numOfVerts;
            //int yValue = audioData[i] * 30;
            int yValue = 1;
            float stripeWidth = sin(pi * i/numOfVerts * ofGetElapsedTimef() * speed) * ofGetWidth();
            
            yValue = (i < numOfVerts/2) ? -yValue : yValue;
            
            ofDrawRectangle(xMid-stripeWidth/2, yValue + i*distance + yMid, stripeWidth + audioData[i]*10, 14 - audioData[i]*10);
            
            ofDrawRectangle(xMid-stripeWidth/2, -yValue - i*distance + yMid, stripeWidth + audioData[i]*10, 14 -audioData[i]*10);
        }
        
        
        // Barcode Stripes -----
        if(bBarcodeStripes){
             ofFill();
            if(i < numOfVerts/2){
                ofDrawRectangle(-audioData[i]*ofGetWidth()/4+ofGetWidth()/2, 0, audioData[i]*10+1, ofGetHeight());
            }else{
                ofDrawRectangle(audioData[i]*ofGetWidth()/4+ofGetWidth()/2, 0, audioData[i]*10+1, ofGetHeight());
            }
        }
        // Stripes 3D   -------
        if(b3DStripes){
             ofFill();
//            ofRotate(45);
            for (int j=0; j<8; j++){
                // vertical
                if(i < numOfVerts/2){
                    ofDrawRectangle( xMid - lineOffset * j, 0, lineReaction, audioData[i]*10+1, ofGetHeight());
                }else{
                    ofDrawRectangle( xMid + lineOffset * j, 0, lineReaction, audioData[i]*10+1, ofGetHeight());
                }
                //horizontal
                if(i < numOfVerts/2){
                    ofDrawRectangle(0, yMid - lineOffset * j, lineReaction, ofGetWidth(), audioData[i]*10+1);
                }else{
                    ofDrawRectangle(0, yMid + lineOffset * j, lineReaction,  ofGetWidth(), audioData[i]*10+1);
                }
            }
//            ofRotate(-45);
            
        }
        
        // Mountains -----
        
        // ----- TODO : _____
        // Mountains in eigene draw funktion schreiben. ohne rotate, ohne cam, curve kann sich nur zwischen bildrändern bewegen.
        //
        
        if(bDrawMountains){
            camera.getTarget();
            ofSetLineWidth(3);
            ofSetColor(ofColor::fromHsb(28+abs(sin(ofGetElapsedTimef()*0.01)*10)-i, 250, 150-i, 220));

            ofCurveVertex(i + audioData[i]*10  * sin(i*0.001f + ofGetFrameNum() * 0.002f), ofGetHeight()/2 + ofGetHeight() * 0.3f * sin(i*0.01f + ofGetFrameNum() * 0.02f) + audioData[i]*10 );
        }
        
        // or big field -------
        if(bPearlField){
            ofFill();
            camera.getTarget();
            audioVolume /= 30; //normalisieren zu 0..1
            
            spacing = 150;
            int x = i * spacing * pearlsSize;
            
            
            float xOffset = pearlsXoffset+1;
            
            if(i<numOfVerts*0.5){
                for(int z=0; z<ofGetWidth() * pearlsSize *1.2; z += spacing*pearlsSize){
    //                for(int x=0; x<ofGetWidth() * pearlsSize; x+= spacing*pearlsSize){
                        for(int y=0; y<ofGetWidth() * pearlsSize*1.2; y+= spacing*pearlsSize) {
                            float xAxis = yMid + yMid * 0.3f * sin(y*0.01f + ofGetFrameNum() * 0.005f +1) + x; //*audioVolume?
                            float zAxis = yMid + yMid * 0.3f * sin(y*0.01f + ofGetFrameNum() * 0.005f ) + z ;
                            ofSetColor(ofColor::fromHsb(20+i*2+(10*abs(sin(ofGetElapsedTimef()))), 250, 200, 250));
                            //                    xAxis = xMid + x;
                            //                    zAxis = yMid + z;
                            //   ofSetColor(250);
                            ofSetIcoSphereResolution(2);
                           // ofDrawIcoSphere(ofPoint(xAxis, y, zAxis ), 8*sin(ofGetElapsedTimef()*(audioData[i]*0.5+1)+y+z+x*xOffset) + 10 * audioData[i] + 1);
                             ofDrawIcoSphere(ofPoint(xAxis, y, zAxis ), 8*sin(ofGetElapsedTimef()+y+z+x*xOffset) + 10 * audioData[i] + 1);

                        }
    //                }
                }
            }
        }
    }
    audioVolume /= 30; //normalisieren zu 0..1

//    freqMax = (freqMax/(float)numOfVerts)*22050;
    
//    text << "freqMax: " << freqMax;
//    ofDrawBitmapString(text.str(), 250, 20);
//    text.str("");
    
    //single worm
    if(bSingleWorm){
        
        boxRotation += 0.001;
        int lengthOfWorm = ofGetWidth()*4;

        for(int x=0; x < lengthOfWorm; x += spacing/2) {
   
            float yAxis = ofGetHeight()/2 + ofGetHeight() * 0.3f * sin(x*0.008f + ofGetFrameNum() * 0.009f) * audioVolume * 0.2;
            float zAxis = ofGetHeight()/2 + ofGetHeight() * 0.2f * sin(x*0.008f + ofGetFrameNum() * 0.009f)  ;

            ofSetColor(ofColor::fromHsb(100*abs(sin(ofGetElapsedTimef()*0.01)+0.5)+20, 250, 250, 230));
            ofSetIcoSphereResolution(3);
            ofRotate(boxRotation);
            ofDrawIcoSphere(ofPoint(x, yAxis, zAxis ), 50*sin(ofGetElapsedTimef()*0.2f+x));
        }
    }
    
    //Worm 2 - Spiral
    if(bWorm2){
        ofFill();
        boxRotation += pearlsXoffset * 0.008;
        spacing = 2;
        int lengthOfWorm = 2000;
        
        for(int x=0; x < lengthOfWorm; x += spacing/2) {

            float yAxis = yMid + ofGetHeight() * 0.3f * sin(x*0.008f + ofGetFrameNum() * 0.009f  );
            float zAxis = yMid + ofGetHeight() * 0.2f * sin(x*0.008f + ofGetFrameNum() * 0.009f  )  ;
            
            ofSetColor(ofColor::fromHsb(100*abs(sin(ofGetElapsedTimef()*0.01)+0.5)+10+x/80, 200, 250, 230));
            ofSetSphereResolution(3);
            ofRotate(boxRotation);
            ofDrawSphere(ofPoint(x, yAxis, zAxis ), 30*sin(ofGetElapsedTimef()*pearlsSize*0.5+x)* audioVolume + 10);
        }
    }
    
    if(0){
        for(int x=0; x<ofGetWidth(); x+= spacing) {
            float yAxis = ofGetHeight()/2 + ofGetHeight() * 0.3f * sin(x*0.01f + ofGetFrameNum() * 0.01f) * audioVolume ;
            float zAxis = 0  ;
            ofSetColor(ofColor::fromHsb(100*abs(sin(ofGetElapsedTimef())+0.5)+20, 250, 150, 250));
            ofDrawLine(ofPoint(x*audioVolume, yAxis, zAxis ), ofPoint(x*audioVolume, yAxis+200, zAxis ));
        }
    }

    delete[] audioData;
//    ofEndShape(false);
    
    
    

    
    camera.end();

    
    
}

//--------------------------------------------------------------
void ofApp::setupGlitchRect(){
    ofSetBackgroundColor(0);
    
}
//--------------------------------------------------------------
void ofApp::updateGlitchRect(){
    


    if(midiMessage.control == 22){ // rad 7 für alpha values in drawGlitchRect
        colorGlitchAlpha = ofMap(midiMessage.value, 0, 127, 1, 255);
    }
    if(midiMessage.control == 21){ // rad 6 für größe der Rects
        glitchRectSize = ofMap(midiMessage.value, 0, 127, 1, 255);
    }
    if(midiMessage.control == 6){ // fader 7 für größe des draw bereichs
        glitchDrawArea = ofMap(midiMessage.value, 0, 127, 1, ofGetWidth()/3);
    }
    if(midiMessage.control == 5){ // fader 6 für höhe der rects
        glitchRectHeight = ofMap(midiMessage.value, 0, 127, 1, ofGetWidth()/2);
    }
    if(midiMessage.control == 4){ // fader 5 für in Sections getrennte RGB Farben
        glitchSepColor = ofMap(midiMessage.value, 0, 127, 1, 150);
    }
    
}
//--------------------------------------------------------------
void ofApp::drawGlitchRect(){
    
    int     sections = 3;
    float   maxValue = ofGetWidth()/sections;
    
    // Sound -----
    int numOfVerts = numVertsGlobal;
    
    float * audioData = new float[numOfVerts];
    fftLive.getFftPeakData(audioData, numOfVerts);
    volLow = 0;
    volMid = 0;
    volHigh = 0;
    
    
    ofFill();
    
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
        
        if (ofGetKeyPressed('n') || 1 ){
            
            //- - section 1 - -
            ofSetColor(ofRandom(255-glitchSepColor), ofRandom(255-glitchSepColor), ofRandom(255), ofRandom(colorGlitchAlpha*volHigh));
        
            ofDrawRectangle(ofRandom((sections-1)*maxValue+glitchDrawArea-1, sections*maxValue-glitchDrawArea), ofRandom(maxValue-glitchDrawArea, maxValue+glitchDrawArea), ofRandom(glitchRectSize), ofRandom(glitchRectHeight));
            
            //- - section 2 - -
            ofSetColor(ofRandom(255-glitchSepColor), ofRandom(255), ofRandom(255-glitchSepColor), ofRandom(colorGlitchAlpha*volMid));
        
            ofDrawRectangle(ofRandom((sections-2)*maxValue+glitchDrawArea-1, (sections-1)*maxValue-glitchDrawArea+1), ofRandom(maxValue-glitchDrawArea, maxValue+glitchDrawArea), ofRandom(glitchRectSize), ofRandom(glitchRectHeight));
            
            //- - section 3 - -
            ofSetColor(ofRandom(255), ofRandom(255-glitchSepColor), ofRandom(255-glitchSepColor), ofRandom(colorGlitchAlpha*volLow));
        
            ofDrawRectangle(ofRandom((sections-2)*maxValue-glitchDrawArea+1, 0+glitchDrawArea), ofRandom(maxValue-glitchDrawArea, maxValue+glitchDrawArea), ofRandom(glitchRectSize), ofRandom(glitchRectHeight));
       
        }
    }
    
    delete [] audioData;

}

//--------------------------------------------------------------
void ofApp::setupNoiseBlocks(){
    ofSetBackgroundColor(3);
    
    ofEnableDepthTest();
    
    center.set(ofGetWidth()*.5, ofGetHeight()*.5, 0);
    
    // Point lights emit light in all directions //
    // set the diffuse color, color reflected from the light source //
    pointLight.setDiffuseColor( ofColor(255.f, 255.f, 255.f)); //green
    
    // specular color, the highlight/shininess color //
    pointLight.setSpecularColor( ofColor(255.f, 255.f, 255.f));
    pointLight.setPosition(center.x, center.y, 0);
    
    // shininess is a value between 0 - 128, 128 being the most shiny //
    materialNoiseBlocks.setShininess( 128 );
    
    noiseBlocksColorHue = ofRandom(0, 250);
    
    nBlocksLightColor.setBrightness( 180.f );
    nBlocksLightColor.setSaturation( 150.f ); //150
    
    materialNBlocksColor.setBrightness(250.f);
    materialNBlocksColor.setSaturation(200);
    
    centerNode.setPosition(0, 0, 0);
    
    
    noiseValueX = noiseValueY = noiseValueZ = 0.005;
    
    noiseZadding = 0;
    
}

//--------------------------------------------------------------
void ofApp::updateNoiseBlocks(){

    noiseBlocksColorHue += .1f;
    if(noiseBlocksColorHue >= 255) noiseBlocksColorHue = 0.f;
    nBlocksLightColor.setHue(noiseBlocksColorHue);
    pointLight.setDiffuseColor(nBlocksLightColor);
    
    materialNBlocksColor.setHue(noiseBlocksColorHue);
    // the light highlight of the material //
    materialNoiseBlocks.setSpecularColor(materialNBlocksColor);
    

    
    //MIDI
    
    if(midiMessage.control == 21){ // rad 7 noiseValue X
        noiseValueX = ofMap(midiMessage.value, 0, 127, 0.0001, 0.05);
    }
    if(midiMessage.control == 22){ // rad 6 noiseValue Y
        noiseValueY = ofMap(midiMessage.value, 0, 127, 0.0001, 0.05);
    }
    if(midiMessage.control == 6){ // fader 7 für Noise Z, also Geschwindigkeit des Noise
        noiseValueZ = ofMap(midiMessage.value, 0, 127, 0.0, 0.1);
    }
    if(midiMessage.control == 5){ // fader 6 für Audio displacement amount
        audioDisplacement = ofMap(midiMessage.value, 0, 127, 0, 0.5);
    }
//    if(midiMessage.control == 20){ // fader 5 für in Sections getrennte RGB Farben
//        camOrbitSpeed = ofMap(midiMessage.value, 0, 127, -180, 180);
//    }
    
}

//--------------------------------------------------------------
void ofApp::drawNoiseBlocks(){
    int width = 500;
    int height = 500;
    int resolution = 4;
    
//    pointLight.setPosition(width/2, height/2, -200);
    pointLight.setOrientation( ofVec3f( 0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0) );
    pointLight.setPosition( sin(ofGetElapsedTimef()*0.1)*500, sin(ofGetElapsedTimef()*0.5)*300, 200);
    
    
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
    
    if(bAutoCam){
        camDistSpeed    = cam.getDistance();
        camDistSpeed    += camDistSpeedUpdate; //cameraSpeed
        camRotSpeed     += camRotSpeedUpdate;
        camLatSpeed     += camLatSpeedUpdate;
        cam.orbit(camRotSpeed *0.1 , camLatSpeed *0.1, camDistSpeed );
        //cam.setTarget(centerNode);
        
    }
    
    cam.begin();
    
   // pointLight.draw();
    
    ofEnableLighting();
    pointLight.enable();
    materialNoiseBlocks.begin();
    pointLight.rotate(10, 1, 0, 1);
    

    ofTranslate(-width/2, -height/2, 0);
    
    ofFill();
  
   // noiseValueZ += volLow * 0.001 ;
    
    
//    std::string myString(
 //   cout << volLow;
    
    noiseZadding += noiseValueZ;

    for (int i=0; i < width; i+=resolution){
        for (int j=0; j < height; j+=resolution){
            
            ofSetColor(255);
            
            
            
            ofPushMatrix();
            // ofTranslate(i,j, cos(ofGetElapsedTimef()*0.001*i*j)*3);
            
            
            
            auto noise_value = ofNoise(i * noiseValueX, j * noiseValueY,  noiseZadding) ;
            auto rotate_x = 0;
            
           // noise_value *= ofNoise(i* noiseValueX ,0,0);
            
            if (noise_value < 0.25) {
                
                rotate_x = ofMap(noise_value, 0, 0.25, -180, 0);
            }
            else if (noise_value > 0.75) {
                
                rotate_x = ofMap(noise_value, 0.75, 1.0, 0, 180);
            }
            
            ofTranslate(i,j,rotate_x*0.1 * volLow * audioDisplacement);
          //  ofRotateX(rotate_x * 5 * volLow * audioDisplacement);
            
            
            
            // ofRotateX(ofGetElapsedTimef()*50+j+i);
            
            ofFill();
            ofSetColor(10);
            ofDrawBox(0,0 , 0, resolution, resolution, resolution);
          //  ofDrawRectangle(0,0,resolution, resolution);
            
            // ofSetLineWidth(3);
            // ofDrawLine(0, 0, 10, 10);
            
            
            //            ofNoFill();
            //            ofSetColor(250);
            //            ofDrawBox(0,0 , 0, resolution, resolution, resolution);
            ofPopMatrix();
            
            
        }
    }

    cam.end();
    materialNoiseBlocks.end();
    pointLight.disable();
    ofDisableLighting();
        
    delete [] audioData;

}

//--------------------------------------------------------------
void ofApp::exit() {
    
    
    closeFunction();
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
    
    ofSoundStreamClose();
    
    linesBezier.clear();
    linesBezierMirror.clear();
    pointsMountains.clear();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    // make a copy of the latest message
    midiMessage = msg;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    switch(key){
            
        case '1':  // Pearl Field     : Song "Loop Song"
            sceneNumber         = 1;
            sceneName           = "Pearl Field - Bach I ";
            closeFunction();
            setupStringOfPearls();
            bDrawStringOfPearls = true;
            bDrawPearlField     = true;
            break;
            

    
        case '2': // GPU Particles
            sceneNumber         = 2;
            sceneName           = "Particles GPU - Bach II";
            closeFunction();
            bAutoCam = true;
            setupParticlesGPU();
            bDrawParticlesGPU = true;
            break;
            
        case '3':   //  Noise Blocks
            sceneNumber         = 3;
            sceneName           = " Noise Blocks - Bach III";
            closeFunction();
            setupNoiseBlocks();
            bDrawNoiseBlocks    = true;
            break;
            
        
        case '4':   //  CLouds Fbm
            sceneNumber         = 4;
            sceneName           = " Clouds FBM - Bach IV";
            closeFunction();
            setupCloudsFbm();
            bDrawCloudsFbm      = true;
            break;
            
        case '5':  // draw Beziers
            sceneNumber         = 5;
            sceneName           = "Bezier - Bach V + VI";
            closeFunction();
            setupBezierLines();
            bDrawBezierLine     = true;
            break;
          
        case '6':  //Mointains"
            sceneNumber         = 6;
            sceneName           = "My Mountains - Höller?";
            closeFunction();
            ofSetBackgroundAuto(true);
            ofDrawRectangle(0,0,ofGetWidth(), ofGetHeight());
            setupMountains();
            bDrawMountains      = true;
            bDrawBezierLine     = false;
            break;
            
            
        case '7':
            sceneNumber         = 7;
            sceneName           = "Node Lines - Schostakovich I";
            closeFunction();
            setupNodeLines();
            bDrawNodeLines  = true;
            bDrawNet        = false;
            break;
            
        case '8': //grow lines: Song "One"
            sceneNumber         = 8;
            sceneName           = "Growing Lines - Schostakovich II";
            closeFunction();
            setupGrowingLines();
            bGrowLine           = false;
            bDrawGrowingLines   = true;
            break;
            

            
        case '9':
            sceneNumber         = 9;
            sceneName           = "Node Net - Schostakovich III";
            closeFunction();
            setupNodeLines();
            bDrawNodeLines  = true;
            bDrawNet        = true;
            break;

            
        case '0': //NoiseBlocks 2D
            sceneNumber         = 0;
            sceneName           = "NoiseBlocks 2D - Schostakovich IV";
            closeFunction();
            setupNoiseBlocks2D();
            bDrawNoiseBlocks2D  = true;
            bAutoCam            = false;
            break;
            





//
//        case '4': // Worm: Song "LHP"
//            sceneNumber         = 4;
//            sceneName           = "Worm - LHP";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDrawSingleWorm     = true;
//            break;
            
        case 'y':  // Frequency Lines
            sceneNumber         = 7;
            sceneName           = "Frequency Lines";
            closeFunction();
            setupFrequencyLine();
            bDrawFrequencyLine  = true;
            break;
            
        case 'x':   // Frequency Lines 2 Circles
            closeFunction();
            setupFrequencyLine();
            bDrawFrequencyLine  = true;
            bDrawFreqCircles    = true;
            bAutoCam            = true;
            break;

            
//        case '5': // Cosinus Stripes: Song "Outro"
//            sceneNumber         = 5;
//            sceneName           = "Cosinus Stripes - Outro";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDrawCosStripes     = true;
//            break;
//

//
//        case 'y':   // (Barcode &) 3D Line Gitter: Song "Delay Hack"
//            sceneNumber         = 6;
//            sceneName           = "Bar Code - Delay Hack";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDrawBarcodeStripes = true;
//            break;
//
//        case 'x':   // 3D Line Gitter: Song "Delay Hack"
//            sceneNumber         = 6;
//            sceneName           = "Line Gitter - Delay Hack";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDraw3DStripes      = true;
//            break;



//        case '0':   //  Circles   : Song "Glocken"
//            sceneNumber         = 7;
//            sceneName           = " Boxes - Glocken";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDrawBoxes        = true;
//            break;
//
//
//        case 'p':   //  Circles   : Song "Glocken"
//            sceneNumber         = 7;
//            sceneName           = " Circles - Glocken";
//            closeFunction();
//            setupStringOfPearls();
//            bDrawStringOfPearls = true;
//            bDrawCircles        = true;
//            break;
//
//


            



            
        case 'f':
            fullscreen = !fullscreen;
            ofSetFullscreen(fullscreen);
            break;
    
        case 'h':
            bHideCursor = !bHideCursor;
            break;
            
        case 'c':
            usecamera = !usecamera;
            break;
            
        case 'r':
            reverse = !reverse;
            bDrawWireframe = !bDrawWireframe;
            numScreens = 1;
            
            //bezier lines:
            bRotate = !bRotate;
            timeCounter = ofGetElapsedTimef();
            
            break;
            
        case 'n':
            newBoid = !newBoid;
            bDrawNet = !bDrawNet;
            break;
            
        case 'm':
            drawAntiboids = !drawAntiboids;
            bChangeNode     = !bChangeNode;
            break;
        case 'k':
            seekRandomPoint = !seekRandomPoint;
            break;
        case 'q':
            boidsSphere = !boidsSphere;            // boids make a sphere
            usecamera = true;
            break;
        
        case 'w':
            bDrawFocusAssist = !bDrawFocusAssist;
            break;
        case 'a':
            drawRibbonAuto = !drawRibbonAuto;
            break;
//        case 's':
//            useDOF = !useDOF;
//            break;
        case 'g':
            bGrowLine = !bGrowLine;
            break;
        case 't':
            bDrawLineMesh = !bDrawLineMesh;
            numScreens = (int) ofRandom(1, 10);
            
//            if(numScreens < 10){
//                numScreens++;
//            }else if(numScreens > 0){
//                numScreens--;
//            }
            
            
            break;

//            
//        case 's':
//            saveParamsToXML();
//            break;
//            
//        case 'l':
//            loadParamsFromXML();
//            break;
            
        case 'i':
            bInvertColor = !bInvertColor;
            break;
            
    }
    
    
    if (key == 'c'){
        // GPU Particles
        centerForce = 5;
        
        //bezier line
        linesBezier.clear();
        aLine.clear();
        bLine.clear();
        linesBezierMirror.clear();
        aLineMirror.clear();
        bLineMirror.clear();
    }

}

//--------------------------------------------------------------
void ofApp::closeFunction(){
    
    ofSetBackgroundColor(0);
    ofSetBackgroundAuto(true);
    points.clear();
    drawnPoints.clear();
    lines.clear();
    closeSpectroSphere();
    pLine.clear();
    texScreen.clear();
    bDrawDanceCloud     = false;
    bDrawRibbonBoids    = false;
    bDrawSpectroSphere  = false;
    bDrawTriangleCloud  = false;
    bDrawFleeingCells   = false;
    bDrawParticles      = false;
    bDrawGrowingLines   = false;
    bDrawStringOfPearls = false;
    bDrawGlitchRect     = false;
    bDrawCircles        = false;
    bDrawBoxes          = false;
    bDrawCircles        = false;
    bDrawCosStripes     = false;
    bDrawBarcodeStripes = false;
    bDraw3DStripes      = false;
    bDrawSingleWorm     = false;
    bDrawPearlField     = false;
    bDrawWorm2          = false;
    bDrawNoiseBlocks    = false;
    bDrawFrequencyLine  = false;
    bDrawParticlesGPU   = false;
    bDrawFreqCircles    = false;
    bDrawCloudsFbm      = false;
    bDrawBezierLine     = false;
    closeBezierLines();
    bDrawMountains      = false;
    bDrawNodeLines      = false;
    bDrawNoiseBlocks2D  = false;
    bDrawNet            = false;
    
    fadeToBlack         = false;
    
    
    invertShader.unload();
    shaderFbmNoise.unload();
    

}

//--------------------------------------------------------------
void ofApp::saveParamsToXML(){
    
    //----- test for parameter set saving
    ofVec3f position = camera.getPosition();
    ofXml xmlParameter;
    
    if(xml.exists("//CAMPOS")){
        xml.clear();
    }
    xmlParameter.addChild("CAM");
    xmlParameter.setTo("CAM");
    xmlParameter.addValue("CAMPOS", position);
    float xmlCamRotSpeed = (float) camRotSpeedUpdate;
    xmlParameter.addValue("CAMROTSPEED", xmlCamRotSpeed);
    xml.addXml(xmlParameter);
    
    
    xml.save("mySettings.xml");

    
}

//--------------------------------------------------------------
void ofApp::loadParamsFromXML(){
        //if(xml.exists("//CAM")){
        ofVec3f position = xml.getValue<ofVec3f>("//CAMPOS");
        camera.setPosition(position);
        
        camRotSpeedUpdate = xml.getValue<float>("//CAMROTSPEED");
        // }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    switch(key){
            
        case'b':
            closeFunction();
            //bDrawBlack          = true;
            break;
    }
    
    if (key == 'c'){
        centerForce = 0;
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
    if(!bDrawRibbonBoids && !bDrawParticles){
        points.clear();
    }
    
    if(!reverse && !usecamera && bDrawRibbonBoids && !drawRibbonAuto && !bDrawParticles){
        ofVec3f mousePoint(x,y,0);
        points.push_back(mousePoint);
    }
    
//    if(drawRibbonAuto && bDrawRibbonBoids){
//        ofVec3f mousePoint(x,y,0);
//        points.push_back(mousePoint);
//    }

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(bDrawBezierLine){
    
        for (auto point : drawnPointsBezier){
            ofPoint mouse;
            mouse.set(x,y);
            float dist = (mouse - point).length();
            
            
            if (dist < 55){
                Line lineTemp;
                lineTemp.a = mouse;
                lineTemp.b = point;
                linesBezier.push_back(lineTemp);
            }
        }
        if(drawnPointsBezier.size() < 15){
            drawnPointsBezier.push_back(ofPoint(x,y));
        }
        else{
            drawnPointsBezier.clear();
            drawnPointsBezier.push_back(ofPoint(x,y));
        }
        
        // mirror
        ofPoint mirrorPoint;
        mirrorPoint.set(ofGetWidth()-x, ofGetHeight()-y);
        for(auto point : drawnPointsBezierMirror){
            ofPoint mouseMirror;
            mouseMirror.set(mirrorPoint);
            float distMirror = (mouseMirror - point).length();
            
            if (distMirror < 55){
                Line lineTemp;
                lineTemp.a = mouseMirror;
                lineTemp.b = point;
                linesBezierMirror.push_back(lineTemp);
            }
        }
        if(drawnPointsBezierMirror.size() < 15){
            drawnPointsBezierMirror.push_back(mirrorPoint);
        }
        else{
            drawnPointsBezierMirror.clear();
            drawnPointsBezierMirror.push_back(mirrorPoint);
        }
    }
    
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


////--------------------------------------------------------------
//void ofApp::audioIn(float * input, int bufferSize, int nChannels){
//
//    float curVol = 0.0;
//
//    // samples are "interleaved"
//    int numCounted = 0;
//
//    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
//    for (int i = 0; i < bufferSize; i++){
//        left[i]        = input[i*2]*0.5;
//        right[i]    = input[i*2+1]*0.5;
//
//        curVol += left[i] * left[i];
//        curVol += right[i] * right[i];
//        numCounted+=2;
//    }
//
//    //this is how we get the mean of rms :)
//    curVol /= (float)numCounted;
//
//    // this is how we get the root of rms :)
//    curVol = sqrt( curVol );
//
//    smoothedVol *= 0.93;
//    smoothedVol += 0.07 * curVol;
//    
//    bufferCounter++;
//
//}
