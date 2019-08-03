# ofxLiveShow

My Setup for audioreactive and interactive Live Shows. Paul Bießmann 2019

Still called ClusterDuo, since I started there to build one Framework for all shows.

## Getting Started

Made with Mac OSX High Sierra 10.13.6
Xcode 10.1
openFrameworks v0.9.8_osx_release [get it here](https://openframeworks.cc/download/older/)

### Xcode Build Settings

Change Build Settings:
Since Xcode 10 with High Sierra when first building a solution, go in Xcode to your 
Project -> Build Settings -> OF_CORE_FRAMEWORKS -> Remove "-framework Quicktime"


### Addons

* [ofxDOF](https://github.com/obviousjim/ofxDOF)
* [ofxFFT](https://github.com/julapy/ofxFFT)
* [ofxGpuParticles](https://github.com/neilmendoza/ofxGpuParticles)
* [ofxGui](https://openframeworks.cc/documentation/ofxGui/) should already be in Addons folder
* [ofxMidi](https://github.com/danomatika/ofxMidi)
* [ofxVoro](https://github.com/patriciogonzalezvivo/ofxVoro)


## Controlling

### MIDI
I used a [NanoKontrol 2](https://www.thomann.de/de/korg_nanokontrol_2_black.htm) for all my controls. MIDI adresses are now mapped to this. But maybe one with motor faders would be better. 

Change the MIDI-input device in ofApp.cpp 
```
midiIn.listPorts();
midiIn.openPort(YOURNUMBER);
```
Default it's 1, but let it list your devices and see. 

### Sound

Get an external Soundcard and set your desired device. (Usually it get's automatically the current soundstream, if you comment out the setDeviceID()).
```
soundStream.printDeviceList();
soundStream.setDeviceID(2);
```

## Build your setup

1. Have a look in the *ofKeyreleased()* function. 
    For drawing a scene, do on a 'Key':
```    
case'1':
    bDrawANYTHING = true;
    closeFunction();
    setupANYTHING();
    break;
```

2. In *update()*
```
if(bDrawANYTHING)
{
    updateANYTHING();
}
```

3. in *draw()*
```
if(bDrawANYTHING)
{
    drawANYTHING();
}
```

## All switches to the scenes

Keep updated!
```
bDrawDanceCloud
bDrawRibbonBoids
bDrawSpectroSphere
bDrawTriangleCloud
bDrawFleeingCells
bDrawParticles
bDrawGrowingLines
bDrawStringOfPearls -- goes together with these:
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
```


## Known Issues
03-August 2019

* ofxFFT does sometimes not read in the soundstream of the Interface (RME Babyface Pro)
* ofSetBackgroundAuto() does not work in some functions (drawBoxes and drawCircles). Sometimes works, but unstable.
* maybe the drawing/update routine is too big? Sometimes bDrawANYTHING gets not changed correctly. Think of another strategy to make scenes. 


## Authors

* **Paul Biessmann** 

See also his [website](http://paul.biessmann.de)


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details


## Acknowledgements

* [The Book of Shaders](https://thebookofshaders.com) for the fractal brownian motion scene
* Satoshi Okami for his implementation of the Boids in ofxVoro



