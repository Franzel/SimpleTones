#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxDirList.h"
#include "ofxXmlSettings.h"
#include "ofxControlPanel.h"
#include "pointVelocity.h"
#include "button.h"

#define N_COLORS 4

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera. Oherwise, we'll use a movie file
 
#define HOST "localhost"
#define PORT 8000


///-------------tracking point struct---------
typedef struct {
    int hue, sat, val;    
    int r, g, b;
    int hueRange, satRange, valRange;
    int hueDiff;
    
    unsigned char *			grayPixels;
    ofxCvGrayscaleImage		videoGrayscaleCvImage;
    ofxCvContourFinder      contourFinder;
    
    ofVec2f blobPos;
    float blobProportion;
    float   pointDistance, pointDistanceMapped, pointDistanceToCenter;
} trackedColor;


///-------------main class---------
class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void getColorChannels(int _whichPanel, int & _x, int & _y);
        void getDistanceBetweenPoints(ofVec2f pos1, ofVec2f pos2);
        bool amIOver(ofVec2f blobPos, ofVec2f buttonPos, int w, int h);
        ofVec2f calculateCentroid(ofVec2f pos1, ofVec2f pos2, ofVec2f pos3,ofVec2f pos4, int pCounter);
    
    
    ///-------------panel-------------
    string colorNumber;
    ofxControlPanel panel;
    
    bool    showImage;
    bool    fullView;
    bool    showPanel;
    bool    showCtrFinders;
    float   screenWidth, screenHeight;
    
    
    ///-------------computer vision-----
    trackedColor tColor[N_COLORS];
    
    ofxCvColorImage			videoColorCvImage;
    ofxCvColorImage			videoColorHSVCvImage;
    
    ofxCvGrayscaleImage		videoGrayscaleHueImage;
    ofxCvGrayscaleImage		videoGrayscaleSatImage;
    ofxCvGrayscaleImage		videoGrayscaleBriImage;
    unsigned char * colorHsvPixels;
    int                 width, height;

#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		video;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    
    ///-------------points-------------
    vector<pointVelocity>points;
    ofxOscSender sender;
    ofVec2f blobPos;
    bool bMousePressed;
    
    ofVec2f center;
    ofVec2f refPoint;
    
    ofVec2f triangleCentroid;
    int pointCounter;
    float maxRadius;
    
    
    ///-------------buttons-------------
    int nButtons;
    vector<button> buttons;
    float buttonWidth, buttonHeight;
    
    
    ////------------System---------
    ofImage welcome, noScreen;
    float startFade;
    float systemStartTime, limitTime, currentTime, startTime, playTime;
    bool activeSystem;
    
  

};
