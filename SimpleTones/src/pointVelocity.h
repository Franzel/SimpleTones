//
//  pointVelocity.h
//  getVelocityOfPoint
//
//  Created by Francisco Zamorano on 2/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef getVelocityOfPoint_pointVelocity_h
#define getVelocityOfPoint_pointVelocity_h

#include "ofMain.h"
#include "ofxOsc.h"


#define HOST "localhost"
#define PORT 8000



class pointVelocity{ 

public:
    
    pointVelocity();
    
    void    setup(ofVec2f _pos, float _size, int _num);
    void    addPos(float x, float y);
    float   getSpeed();
    float   getDistance(ofVec2f pointPos, float maxDistance);
    float   smoothSpeed();
    void    draw(ofVec2f _pos);
    float   getAngle(ofVec2f _pivotPos);
    
    void    play(ofxOscSender * sender, bool bBlob);
    void    stop(ofxOscSender * sender);
    
    void    playOverButton(ofxOscSender * sender , int buttonNum);
    void    playSlice(ofxOscSender * sender, int sliceId, int blobId);
    


    
    //properties
    ofColor c;
    ofVec2f pos;
    float   radius;

    
    //smoothing
    vector<ofVec4f> oldPos;
    float   currentMappedSpeed;
    int     lookBack;
    
    //distance
    float distanceToPoint, distanceToPointMapped;
    bool measureThisPoint;
    ofVec2f prevPos;
    ofVec2f currentPos;
    
    //angle
    float angle;
    int angleSlices;
    ofVec2f pivotPos;
    int prevSliceId;
    int currSliceId;
    
    
    
    //messages
    string blobNum;
    bool bPlaying;
    bool bOverButton;
    int buttonId;
    int prevButtonId;
    
    //timeCounter
    float startTime;
    float currTime;
    float limitTime;
    bool bActivity;



};
#endif
