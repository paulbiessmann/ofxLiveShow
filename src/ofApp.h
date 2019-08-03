/* Paul Bießmann 30.Nov 2018 -- Made for Cluster Electronic Session*/

#pragma once

#include "ofMain.h"
#include "ofxFFTLive.h"
#include "ofxFFTFile.h"
#include "ofxGui.h"
#include "ofxMidi.h"
#include "ofxDOF.h"
#include "ofxVoro.h"
#include "ofxBoids.h"
#include "ofxGpuParticles.h"

/**   All the switches:
 
 bDrawDanceCloud
 bDrawRibbonBoids
 bDrawSpectroSphere
 bDrawTriangleCloud
 bDrawFleeingCells
 bDrawParticles
 bDrawGrowingLines
 bDrawStringOfPearls -- darin sind diese:
    bDrawBoxes
    bDrawCircles
    bDrawCosStripes
    bDrawBarcodeStripes
    bDraw3DStripes
    bDrawSingleWorm
    bDrawPearlField
    bDrawWorm2
 
 bDrawGlitchRect
 bDrawNoiseBlocks
 bDrawFrequencyLine
 bDrawParticlesGPU
 bDrawFreqCircles
 bDrawCloudsFbm
 bDrawBezierLine
 bDrawMountains
 bDrawNodeLines
 bDrawNoiseBlocks2D
 bDrawNet
 
 
 
 fadeToBlack
 
(bDrawMountains)
 
 */


class Line {
public:
    ofPoint a;
    ofPoint b;
};



class ofApp : public ofBaseApp, public ofxMidiListener{

    public:
    void setup();
    void update();
    void draw();
    void setupGui();
    void drawGui(ofEventArgs & args);
    void exit();

    void closeFunction();

    void drawDanceCLoud();
    void updateDanceCloud();
    void setupDanceCloud();

    void drawRibbonBoids();
    void updateRibbonBoids();
    void setupRibbonBoids();

    void setupSpectroSphere();
    void updateSpectroSphere();
    void drawSpectroSphere();
    void closeSpectroSphere();

    void setupFleeingCells();
    void updateFleeingCells();
    void drawFleeingCells();

    void setupGrowingLines();
    void updateGrowingLines();
    void drawGrowingLines();
    
    void setupStringOfPearls();
    void updateStringOfPearls();
    void drawStringOfPearls();
    
    void setupGlitchRect();
    void updateGlitchRect();
    void drawGlitchRect();
    
    void setupNoiseBlocks();
    void updateNoiseBlocks();
    void drawNoiseBlocks();
    
    void setupFrequencyLine();
    void updateFrequencyLine();
    void drawFrequencyLine();
    
    void setupParticlesGPU();
    void updateParticlesGPU();
    void drawParticlesGPU();
    
    void setupCloudsFbm();
    void updateCloudsFbm();
    void drawCloudsFbm();
    
    void setupBezierLines();
    void updateBezierLines();
    void drawBezierLines();
    void closeBezierLines();
    
    void setupMountains();
    void updateMountains();
    void drawMountains();
    
    void setupNodeLines();
    void updateNodeLines();
    void drawNodeLines();
    
    void setupNoiseBlocks2D();
    void updateNoiseBlocks2D();
    void drawNoiseBlocks2D();
    
    void saveParamsToXML();
    void loadParamsFromXML();
    
//void midiSmoothing(float _oldValue, float _newValue, bool _isNewValue);
    
    
    void makeTissue(int _nCells, int _width, int _height, int _deep);
    
// fhg
    ofSoundPlayer soundPlayer;
    ofxFFTFile fftFile;

    ofSoundStream soundStream;
    ofxFFTLive fftLive;
    float   volLow, volMid, volHigh;


    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void newMidiMessage(ofxMidiMessage& eventArgs);

    
// GUI ----------------------------
    ofxPanel gui;
    ofxSlider<float> guiNoise;
    ofxSlider<float> audioThreshold;
    ofxSlider<float> audioPeakDecay;
    ofxSlider<float> audioMaxDecay;
    ofxSlider<float> danceSize;
    ofxSlider<float> myfocaldistance;
    ofxSlider<float> myfocalrange;
    ofxToggle dLine;
    ofxToggle bAutoCam;
    ofxSlider<float>  cellSize;
    ofxSlider<float> camDistSpeedUpdate;
    ofxSlider<float> camRotSpeedUpdate;
    ofxSlider<float> camLatSpeedUpdate; 
    ofxToggle useSound;
    ofxSlider<int> meshDisplacement;
    
    int     sceneNumber;
    string  sceneName;
    
    
// DanceCLoud-------------
    bool    bDrawDanceCloud = false;
    float   timeCount;
    ofEasyCam cam;
    float   camDistSpeed;
    float   camRotSpeed;
    float   camLatSpeed;
    float   oldDanceSize, newDanceSize;
    bool    newValue = false;
    // cells voros
    vector<ofVboMesh>  cellMeshes;
    vector<ofVboMesh>  cellMeshWireframes;
    int             containerSize;
    ofLight lightCloud;
    bool bDrawFocusAssist;
    
    
// -------------------------
    
// Ribbon Boids ----------------------------------
    bool bDrawRibbonBoids = false;
    //this holds all of our points
    vector<ofVec3f> points;
    //this keeps track of the center of all the points
    ofVec3f center;

    ofEasyCam camera;
    bool usecamera;
    bool reverse;
    ofLight lightRibbon;
    
    int boidNum;
    ofVec3f target;
    vector<SteeredVehicle> boids;
    vector<SteeredVehicle> antiBoids;
    vector<SteeredVehicle> boids3;
    
    ofIcoSpherePrimitive icoSphereBoids1;
    ofSpherePrimitive icoSphereBoids2;
    ofBoxPrimitive boxBoids3;
    
    ofVec3f oldPoint;
    float time;
    bool newBoid;
    int antiBoidNum;
    int boidNum3;
    int boid3fac;
    bool boidsSphere;
    const int numVertsGlobal = 256;
    int boidSphereSize;
    float boidSphereRot, boidSphereRotUpdate;
    bool drawAntiboids;
    bool    seekRandomPoint;
    ofVec3f randomPoint;
    
    ofSpherePrimitive bSphere;
    ofMesh meshSphereWarped;
    ofMesh meshSphere;
    
    float   colorChanging;
    float   ribbonThickness;
    bool    drawRibbonAuto;
    float   circleSize;
    float   speed ;
    bool    useDOF;
    
    bool    bDrawParticles;

//----------------------------------------------
    
// SpectroSphere ------------
    bool bDrawSpectroSphere = false;
    ofShader shader;
    ofPlanePrimitive plane;
    ofSpherePrimitive sphere;
    ofMesh sphereWarped;
    ofMesh sphereOrig;
    ofImage img;
    
    ofMaterial materialSpectroSph;
    
    ofBoxPrimitive box;
    ofMesh boxOrig;
    ofMesh boxWarped;
    
    float boxRotation;
    
    ofLight lightSpectroSph;
    ofColor lightColor;
    ofColor materialColor;
    float colorHue;
    
    float resolution = 50;
    //float meshDisplacement = 200;
    ofPolyline currentPolyline;
//--------------------------
    
// Triangles Cloud---------------
    bool bDrawTriangleCloud = false;
    
//--------------------------
    
    // fleeing Cells ------------
    bool        bDrawFleeingCells = false;
    vector<ofPoint> cellCentroids;
    vector<float>   cellRadius;
    float       time0, timeAdd, timeExplode;
    bool        freeOne;
    int         subtractSeed;
    bool        exploding;
    int         explodingSize = 20;
    ofPoint     explodingPoint;
//--------------------------
    
// Growing Lines ------------
    bool    bDrawGrowingLines   = false;
    vector <ofPoint> drawnPoints;
    vector <Line> lines;
    int     xgrow, ygrow, zgrow;
    bool    bGrowLine = false;
    vector<ofVec3f> offsets;
    ofMesh  linesMesh;
    float   timeScale = 5.0; //5.0
    float   displacementScaleGrowLin = 0.75; //0.75
    float   displacementScaleGrowLin2 = 1.0;
    ofNode  camTarget;
    bool    bDrawLineMesh   = false;
    bool    bDrawWireframe  = false;
//--------------------------
    
// String Of Pearls --------
    
    bool    bDrawStringOfPearls = false;   //whole Draw Function without any Presentation
    
    // StringOfPearl Presentations are the following:
    bool    bDrawBoxes          = false;
    bool    bDrawCircles        = false;
    bool    bDrawCosStripes     = false;
    bool    bDrawBarcodeStripes = false;
    bool    bDraw3DStripes      = false;
    bool    bDrawSingleWorm     = false;
    bool    bDrawPearlField     = false;
    bool    bDrawWorm2          = false;
    
    vector<ofPoint> circlePos;
    
    float   pearlsXoffset   = 1.0;
    float   pearlsSize      = 1.0;
    float   line3Doffset    = 1.0;
    float   line3Dreaction  = 1.0;
    float   newLine3Doffset = 1.0;
    float   oldLine3Doffset = 1.0; //to smooth MIDI values
    
    bool    addResolution   = true;
    
    ofTexture   glowingTexture;
    ofShader    glowingShader;
    ofVbo       vbo;
//--------------------------
    
// Glitch Rectangles -------
    bool    bDrawGlitchRect = false;
    float   colorGlitchAlpha;
    float   glitchDrawArea;
    float   glitchRectSize;
    float   glitchRectHeight;
    float   glitchSepColor;
//--------------------------
    
    //shader
    ofxDOF depthOfField;
    
    // material
    ofMaterial material;
    ofLight areaLight;

    
    //MIDI
    stringstream text;
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    bool bHideCursor;
    bool fullscreen;
    bool fadeToBlack    = false;
    bool bDrawBlack     = false;
    
    
    ofFbo fbo;
    ofFbo fboEff;
    
    int fadeAmount;
    
//-------- Noise Blocks ------------
    
    ofLight pointLight;
    
    ofColor nBlocksLightColor;
    float noiseBlocksColorHue;
    ofColor materialNBlocksColor;
    
    ofMaterial materialNoiseBlocks;
    
//    ofVec3f center;
    
    ofNode centerNode;
    
    float noiseValueX;
    float noiseValueY;
    float noiseValueZ;
    float noiseZadding;
    float audioDisplacement;
    float camOrbitSpeed;
    
    bool bDrawNoiseBlocks = false;
//----------------------------------
    


    
    
    //------------ Frequency Line -----------
//    void audioIn(float * input, int bufferSize, int nChannels);
    void drawSingle(int numShapes);
    void addPoint(float x, float y, float z);
    void drawShape(int numOfShape);
    void drawFreqCircles(int numOfShape);
    
    float noiseMax;
    float phase;
    
    vector <ofVec3f> freqPoints;
    vector <ofVec3f> sizes;
    
    ofPolyline  pLine;
    ofTexture   texScreen;
    ofTexture   texScreenInverted;
    ofPixels    screenInverted;
    
    bool    bDrawFrequencyLine = false;
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int     bufferCounter;
    int     drawCounter;
    
    float   smoothedVol;
    float   scaledVol;
    
    int     numScreens = 1;
    
    ofShader    invertShader;
    
    bool    bInvertColor = false;
    
    
    bool    bDrawFreqCircles = false;
    
    //----------------------------------
    
    
//----- particles GPU ------------
    // set any update uniforms in this function
    void    onParticlesUpdate(ofShader& shader);
    
    ofxGpuParticles particles;
    
    bool    bCenterForce        = false;
    float   centerForce         = 0;
    bool    bDrawParticlesGPU   = false;
    
    ofShader    shaderParticles;
    
    float   timeSpeed           = 1.0;
    
//----------------------------------
    
    
//----------- Clouds Fractal Brownian Motion ---------
    ofShader    shaderFbmNoise;
    float       lacunarityFbm;
    float       glitchFbm;
    float       uniVFbm;
    float       gainFbm;
    
    float       cloudTimeX;
    float       cloudTimeY;
    float       timeSpeedX;
    float       timeSpeedY;
    float       audioReaction = 1.0;
    
    float       oldAudioEnergy = 0.0;
    float       smoothAudioEnergy = 0.0;
    
    bool        bDrawCloudsFbm  = false;
//-----------------------------------------------------
    
    
//----------- Bezier Drawing -----------------
    vector < ofPoint > drawnPointsBezier;
    vector < Line > linesBezier;
    
    vector < ofPoint > drawnPointsBezierMirror;
    vector < Line > linesBezierMirror;
    
    bool            bRotate = false;
    float           timeCounter = 0;
    
    ofPolyline      aLine;
    ofPolyline      bLine;
    
    ofPolyline      aLineMirror;
    ofPolyline      bLineMirror;
    
    bool            bDrawBezierLine = false;
//--------------------------------------------
    
    
//----------- My Mountains -----------------
    void addPointMountains(float x, float y, float z);

    float noiseMaxMountains;
    float phaseMountains;
    
    vector <ofVec3f> pointsMountains;
    vector <ofVec3f> sizesMountains;

    
    ofPolyline pMountains;
    
    float mountainMoveX;
    float mountainMoveY;
    
    float mountainX;
    float mountainY;
    
    bool  bDrawMountains = false;
//--------------------------------------------
    
//------------ nodeLine ----------------------
    ofNode baseNode;
    ofNode childNode;
    ofNode grandChildNode;
    
    ofPolyline line;
    ofPolyline line2;
    
    ofPoint point1, point2;
    
    float   noiseMax_NodeLine;
    
    bool    bChangeNode     = false;
    bool    bDrawNet        = false;
    
    bool    bDrawNodeLines  = false;
    
//--------------------------------------------
    
//------------ noiseBlocks2D -----------------
    
    bool    bDrawNoiseBlocks2D  = false;

//--------------------------------------------

    
//-------- save parameter sets to xml  ---------------------
    ofXml   xml;
    string  xmlStructure;
    bool    loadXml    = false;
		
};


