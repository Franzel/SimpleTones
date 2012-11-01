//
//  button.h
//  emptyExample
//
//  Created by Francisco Zamorano on 2/15/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef emptyExample_button_h
#define emptyExample_button_h

#include "ofMain.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 8000


class button{
public:
    
    void setup(ofVec2f _pos, float _w, float _h, int _num);
    void draw();
    
    
    ofColor c;
    ofVec2f pos;
    int w, h;
    string buttonNum;
    bool bPlaying;

    int blobId;
    
};



#endif
