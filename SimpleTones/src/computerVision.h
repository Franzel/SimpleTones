//
//  computerVision.h
//  system_1.0
//
//  Created by Francisco Zamorano on 2/19/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef system_1_0_computerVision_h
#define system_1_0_computerVision_h

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
//// otherwise, we'll use a movie file


#include "ofxOpenCv.h"




class computerVision{
public:
    
    void setup();
    void update();
    void draw();
    
#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		video;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    
    
    
};


#endif
