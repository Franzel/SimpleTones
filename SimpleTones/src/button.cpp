//
//  button.cpp
//  emptyExample
//
//  Created by Francisco Zamorano on 2/15/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "button.h"

//--------------------------------------------------------------
void button::setup(ofVec2f _pos, float _w, float _h, int _num){
    pos = _pos;
    pos.x = _pos.x;
    pos.y = _pos.y;
    w = _w;
    h = _h;
    buttonNum = ofToString(_num);
    
    c.set(255, 255, 255, 255);
    bPlaying = false;
    blobId = -1;
    
}


//--------------------------------------------------------------
void button::draw(){
    if (bPlaying) {
        ofSetColor(c.r, c.g, c.b, 100);
    } else{
        ofSetColor(c.r, c.g, c.b, 20);
    }
    
    ofFill();
    ofRect(pos.x, pos.y, w, h);
    
    
    ofSetLineWidth(1);
    ofNoFill();
    ofSetColor(150);
    ofRect(pos.x, pos.y, w, h);

    
}

