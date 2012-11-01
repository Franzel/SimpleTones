//
//  pointVelocity.cpp
//  getVelocityOfPoint
//
//  Created by Francisco Zamorano on 2/13/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "pointVelocity.h"


pointVelocity::pointVelocity(){
    radius = 20;
    lookBack = 10;
    c.set(255, 255, 255, 100);
    measureThisPoint = true;
    bPlaying = false;
    bOverButton = false;
    buttonId = -1;
    prevButtonId = -1;
    prevSliceId = -1;
    currTime = ofGetElapsedTimef();
    limitTime = 5;
    bActivity = true;
    prevPos.set(0,0);
}

void pointVelocity::setup(ofVec2f _pos, float _size, int _num){
    radius = _size;
    blobNum = ofToString(_num);

    
}

//-----------------------------------------------
void pointVelocity::addPos(float x, float y){
    prevPos = currentPos;
    oldPos.push_back(ofVec4f());
    oldPos[oldPos.size()-1].x = pos.x = x;
    oldPos[oldPos.size()-1].y = pos.y = y;
    oldPos[oldPos.size()-1].z = ofGetElapsedTimef();
    oldPos[oldPos.size()-1].w = getSpeed();
    
    if (oldPos.size()>lookBack*2) {
        oldPos.erase(oldPos.begin());
    }
    currentPos.x = pos.x;
    currentPos.y = pos.y;
}

//--------------------------------------------------------------
float pointVelocity::getSpeed(){
    if(oldPos.size()>1){
        float d = ofDist(oldPos[oldPos.size()-1].x, oldPos[oldPos.size()-1].y, oldPos[oldPos.size()-lookBack].x, oldPos[oldPos.size()-lookBack].y);
        float t = oldPos[oldPos.size()-1].z - oldPos[oldPos.size()-lookBack*2].z;
        
        return d/t;
    }
    return 0;
}

//--------------------------------------------------------------
float pointVelocity::smoothSpeed(){
    if(oldPos.size()>1){
        currentMappedSpeed = ofMap(getSpeed(), 0, 1500, 0, 1, true);
        return currentMappedSpeed;
    }
    return getSpeed();
    
}

//--------------------------------------------------------------
float pointVelocity::getDistance(ofVec2f pointPos, float maxDistance){
    if(oldPos.size()>1){
        distanceToPoint = ofDist(currentPos.x, currentPos.y, pointPos.x, pointPos.y);
        distanceToPointMapped = ofMap(distanceToPoint, 0, maxDistance, 1, 0, true);
        return distanceToPointMapped;
    }
    return 0;
}

//--------------------------------------------------------------
float pointVelocity::getAngle(ofVec2f _pivotPos){
        pivotPos = _pivotPos;
        ofVec2f diff = currentPos - pivotPos;
        float distance = diff.length();
        float angle = atan2(diff.y, diff.x);
        float angleDeg = ofRadToDeg(angle);
        currSliceId = (int)ofMap(angleDeg, 180, -180, 0, 6, true);
    
        return angleDeg;
    
}

//-----------------------------------------------
void pointVelocity::draw(ofVec2f _pos){
    
    if(oldPos.size()>0){
        ofNoFill();
        ofSetColor(c.r, c.g, c.b, 200+55*smoothSpeed());
        ofBeginShape();
        for(int i=0;i<oldPos.size();i++){
            ofVertex(oldPos[i].x, oldPos[i].y);
        }
        ofEndShape();
        
        ofFill();
        ofSetColor(c.r, c.g, c.b, 100+150*smoothSpeed());
        
        ofCircle(currentPos.x,currentPos.y, 10 + radius*smoothSpeed());
//        ofRect(10, 100, 10, -100*smoothSpeed()); // show speed indicator
    }
    
    ofSetColor(255);
//    if (bOverButton) ofCircle(oldPos[oldPos.size()-1].x,oldPos[oldPos.size()-1].y, 5);
    
    
    /// draw pivot
    ofCircle(pivotPos.x, pivotPos.y, 1);
    
    ///draw line to Pivot
    ofLine(pivotPos.x, pivotPos.y, currentPos.x, currentPos.y);
    
    string id;
	id = "b " + ofToString ( buttonId );
    
    string ang, slice, pSlice, sT, cT;
    ang = "a " + ofToString ( getAngle(pivotPos) );
    slice = "s " + ofToString ( currSliceId );
    pSlice = "ps " + ofToString ( prevSliceId );
    
    
    sT = "sT " + ofToString ( startTime );
    cT = "cT " + ofToString ( currTime );

    
//    ofDrawBitmapString(id    , pos.x-10, pos.y+20 );
//    ofDrawBitmapString(ang   , pos.x-10, pos.y+30 );
//    ofDrawBitmapString(pSlice, pos.x-10, pos.y+40 );
//    ofDrawBitmapString(sT , pos.x-10, pos.y+20 );
//    ofDrawBitmapString(cT , pos.x-10, pos.y+30 );
//    ofDrawBitmapString(slice , currentPos.x-10, currentPos.y+10 );


    ///check previous button Id
    if (prevButtonId != buttonId) {
            ofSetColor(255,255,255, 255);
            ofCircle(currentPos.x,currentPos.y, 5);
    }
    
    prevButtonId = buttonId;     ///update buttonId
    
    ///timer for welcome screen
    if (smoothSpeed()<0.01){
        currTime = ofGetElapsedTimef();
        if (currTime - startTime > limitTime) {
            bActivity = false;
        }
    }

    if (smoothSpeed()>0.01){
        currTime = 0;
        startTime = ofGetElapsedTimef();
        bActivity = true;
    }
    
//    if (!bActivity){
//        ofRect(pos.x-20, pos.y-10, 20, 20);
//    }
    
    ///draw circle if slice is new
    if (prevSliceId != currSliceId) {
        ofCircle (currentPos.x, currentPos.y, 5);
        ofNoFill();
        ofSetLineWidth(3);
        ofCircle (currentPos.x, currentPos.y, 12 + radius*smoothSpeed());
        ofFill();
        ofSetLineWidth(1);
    }
    
    prevSliceId = currSliceId; /// update slice Id
    
    
	
        
}
//--------------------------------------------------------------
void pointVelocity::play(ofxOscSender * sender, bool bBlob){
    
    if (bBlob){
        ofxOscMessage s;
        s.setAddress("blob" + blobNum + "/speed");
        s.addFloatArg(smoothSpeed()*10);
        sender->sendMessage( s );
        
        ofxOscMessage d;
        d.setAddress("blob" + blobNum + "/dist");
        d.addFloatArg(distanceToPointMapped);
        sender->sendMessage( d );

        if (!bPlaying) {
            ofxOscMessage m;
            m.setAddress("blob" + blobNum + "/note");
            m.addFloatArg(1);
            sender->sendMessage( m );
            bPlaying=true;
        }
        
    }else{
        
        if (bPlaying) {
            ofxOscMessage m;
            m.setAddress("blob" + blobNum + "/note");
            m.addFloatArg(0);
            sender->sendMessage( m );
            bPlaying=false;
        }
    }

}

//--------------------------------------------------------------
void pointVelocity::stop(ofxOscSender * sender){
    ofxOscMessage m;
    m.setAddress("blob" + blobNum);
    m.addFloatArg(smoothSpeed()*10);
//    if (measureThisPoint)  m.addFloatArg(distanceToPointMapped);
    m.addFloatArg(0);
	sender->sendMessage( m );
}

////--------------------------------------------------------------
void pointVelocity::playOverButton(ofxOscSender * sender, int buttonNum){
    ofxOscMessage m;
    m.setAddress("button" + ofToString(buttonNum));
    m.addFloatArg( 1 );
	sender->sendMessage( m );
    
}

////--------------------------------------------------------------
void pointVelocity::playSlice(ofxOscSender * sender, int sliceId, int blobId){
    ofxOscMessage m;
    m.setAddress("b"+ ofToString(blobId) + "/slice" + ofToString(sliceId));
    m.addFloatArg( 1 );
	sender->sendMessage( m );
    
}




