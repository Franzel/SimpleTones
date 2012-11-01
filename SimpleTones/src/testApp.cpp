#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    // open an outgoing connection to HOST:PORT
	sender.setup( HOST, PORT );
    
    ofBackground(50, 50, 50);
    ofEnableSmoothing();
    ofSetCircleResolution(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    
    fullView = false;
    showPanel = true;
    screenWidth = ofGetScreenWidth();
    screenHeight = ofGetScreenHeight();
    
    showCtrFinders = false;
    showImage = false;
    
    welcome.loadImage("welcome.png");
    noScreen.loadImage("noScreen.png");
    noScreen.setImageType(OF_IMAGE_COLOR);
//    systemStartTime = ofGetElapsedTimef();
//    limitTime = 100;
//    activeSystem = false;


    
    ///-----------COMPUTER VISION----------------------
#ifdef _USE_LIVE_VIDEO
    video.initGrabber(640, 480);
    width = video.width; 
    height = video.height;
#else
    vidPlayer.loadMovie("videoTest10.mov");
    vidPlayer.play();
    width = vidPlayer.width; 
    height = vidPlayer.height;
#endif
    
    //allocate textures
	videoColorCvImage.allocate(width, height);
	videoColorHSVCvImage.allocate(width, height);
	
	videoGrayscaleHueImage.allocate(width, height);
	videoGrayscaleSatImage.allocate(width, height);
	videoGrayscaleBriImage.allocate(width, height);
    
    //initialize values for tracked colors
    for (int i=0; i<N_COLORS; i++) {
        tColor[i].hue = 0;
        tColor[i].sat = 0;
        tColor[i].val = 0;
        tColor[i].hueRange = 20;
        tColor[i].satRange = 30;
        tColor[i].valRange = 25;
    }
    
    //setup tracked colors
    for (int i=0; i<N_COLORS; i++) {
        tColor[i].grayPixels = new unsigned char [width * height];
        tColor[i].videoGrayscaleCvImage.allocate(width, height); 
    }
    
    
    ///-----------PANEL--------------------------------
    panel.setup("cv settings", 720, 0, 300, 300);
    // set panel numbers
    for (int n=0; n<N_COLORS; n++) {
        string panelNumber = ofToString(n);
        panel.addPanel("color" + panelNumber, 1, false);
        panel.setWhichPanel("color" + panelNumber);
        panel.addSlider("hue range"+panelNumber, "HUERANGE"+panelNumber, 20, 0, 255, true);
        panel.addSlider("sat range"+panelNumber, "SATRANGE"+panelNumber, 30, 0, 255, true);
        panel.addSlider("val range"+panelNumber, "VALRANGE"+panelNumber, 25, 0, 255, true);
        
    }
    
    panel.addPanel("area", 1, false);
    panel.setWhichPanel("area");
    panel.addSlider("maxRadius", "MAXRADIUS", 240, 100, 400, true);
//    panel.addSlider("timer", "TIMER", 100, 100, 10000, true);

    panel.loadSettings("cvSettings.xml");
    
    ///-----------POINTS--------------------------------
    for (int i=0; i<points.size(); i++) {
        tColor[i].blobPos.x = 0;
        tColor[i].blobPos.y = 0;
    }
    
    for (int i=0; i<N_COLORS; i++) {
        pointVelocity newPoint;
        newPoint.setup(tColor[i].blobPos, 200, i);
        points.push_back(newPoint);
        
    }
    
    //set center of screen
    center.x = width/2;
    center.y = height/2;
    
    refPoint.set(width/2, height/2);
    maxRadius = 240;
    
    
    ///-----------BUTTONS--------------------------------
    // button size, dimensions and positions
    int nButtons = 4;
    ofVec2f tempPos;
    tempPos.set(80,0);
    float topBleed = 30;
    float buttonWidth = 200;
    float buttonHeight = 40;
    
    // create buttons
    for (int n=0; n<2; n++) {
        float secondSetY = buttonHeight*nButtons*n; // do it twice
        
        for (int i=0; i<nButtons; i++) {
            button newButton;
            tempPos.y =  topBleed + buttonHeight*i + secondSetY;
            newButton.setup(tempPos, buttonWidth, buttonHeight, i);
            buttons.push_back(newButton);
        }
    }
       
}

//--------------------------------------------------------------
void testApp::update(){
//    ///-----------SYSTEM--------------------------------
//    systemStartTime = ofGetElapsedTimef();
    

    
    ///-----------PANEL--------------------------------
    panel.update();
    
    ///-----------COMPUTER VISION----------------------
    bool bNewFrame = false;
#ifdef _USE_LIVE_VIDEO
    video.grabFrame();
    bNewFrame = video.isFrameNew();
#else
    vidPlayer.idleMovie();
    bNewFrame = vidPlayer.isFrameNew();
#endif
    
    if (bNewFrame){
#ifdef _USE_LIVE_VIDEO
        video.update();
        
        videoColorCvImage.setFromPixels(video.getPixels(), width, height);
#else
        videoColorCvImage.setFromPixels(vidPlayer.getPixels(), width, height);
#endif
        
        ///get video 
        videoColorCvImage.mirror(false, false);
		videoColorHSVCvImage = videoColorCvImage;
		videoColorHSVCvImage.convertRgbToHsv();
		videoColorHSVCvImage.convertToGrayscalePlanarImages(videoGrayscaleHueImage, videoGrayscaleSatImage, videoGrayscaleBriImage);
		colorHsvPixels = videoColorHSVCvImage.getPixels();
        
        
        /// get range values frmo panel
        for (int n = 0; n < N_COLORS; n++){
            string channelNum = ofToString(n);
            tColor[n].hueRange = panel.getValueI("HUERANGE" + channelNum);
            tColor[n].satRange = panel.getValueI("SATRANGE" + channelNum);
            tColor[n].valRange = panel.getValueI("VALRANGE" + channelNum);
        }
        
        maxRadius = panel.getValueF("MAXRADIUS");
        limitTime = panel.getValueF("TIMER");

        
        /// convert to HSV
        for (int n = 0; n < N_COLORS; n++){
            for (int i = 0; i < width*height; i++){
                // since hue is cyclical:
                tColor[n].hueDiff = colorHsvPixels[i*3] - tColor[n].hue;
                if (tColor[n].hueDiff < -127) tColor[n].hueDiff += 255;
                if (tColor[n].hueDiff > 127) tColor[n].hueDiff -= 255;
                
                if ( (fabs(tColor[n].hueDiff) < tColor[n].hueRange) &&
                    (colorHsvPixels[i*3+1] > (tColor[n].sat - tColor[n].satRange) && colorHsvPixels[i*3+1] < (tColor[n].sat + tColor[n].satRange)) &&
                    (colorHsvPixels[i*3+2] > (tColor[n].val - tColor[n].valRange) && colorHsvPixels[i*3+2] < (tColor[n].val + tColor[n].valRange))){
                    tColor[n].grayPixels[i] = 255;
                } else { 
                    tColor[n].grayPixels[i] = 0;
                }
            }
            tColor[n].videoGrayscaleCvImage.setFromPixels(tColor[n].grayPixels, width, height);
            tColor[n].contourFinder.findContours(tColor[n].videoGrayscaleCvImage, 20, 4000, 1, false); //get blobs
        }
        
        
        ///point positions and messages
        pointCounter = 0;
        
        for (int i=0; i<points.size(); i++) {
            if (tColor[i].contourFinder.nBlobs>0) {
                
                tColor[i].blobPos.x = tColor[i].contourFinder.blobs[0].centroid.x; //get blob xPos
                tColor[i].blobPos.y = tColor[i].contourFinder.blobs[0].centroid.y; //get blob yPos

                tColor[i].blobProportion =  tColor[i].contourFinder.blobs[0].boundingRect.width / tColor[i].contourFinder.blobs[0].boundingRect.height; // get proportion of blob
                
                ///Threshold by proportion of blob
                if (tColor[i].blobProportion > 0.8  && tColor[i].blobProportion < 1.2) {

                    points[i].addPos(tColor[i].blobPos.x, tColor[i].blobPos.y);


                    ///do not read anything outside max radius
                    tColor[i].pointDistanceToCenter = ofDist(tColor[i].blobPos.x, tColor[i].blobPos.y, points[3].pos.x, points[3].pos.y);
                    if(tColor[i].pointDistanceToCenter > maxRadius){
                        points[i].currentPos = points[i].prevPos;
                    }
                    

                    ///get distances
                    points[i].getDistance(center, 200); //get distance to a specific point
                    points[i].getAngle(center); // get angle from pivot
                    points[3].getDistance(refPoint, 200); //get distance to refPoint
                    
                    points[i].play(&sender, true);
                    
                    
                }
            }
            else{
                points[i].play(&sender, false);
                
            }
        }
        
        /// set center to a specific point
        center = points[3].pos; //the last one
        
        
        /// SLICES
        ///play Slices
        if (fullView){
            for (int i=0; i<points.size(); i++) {
                if(points[i].prevSliceId != points[i].currSliceId){
                    points[i].playSlice(&sender, points[i].currSliceId, i);
                }
            }
        }
        

        
//        ///BUTTON TRIGGERS
//        for (int i=0; i<points.size(); i++) {
//            points[i].buttonId = -1;
//        }
//        
//        for (int i=0; i<points.size(); i++) {
//            points[i].bOverButton = false;
//            for (int n=0; n<buttons.size(); n++) {              
//                if (amIOver(points[i].pos, buttons[n].pos, buttons[n].w, buttons[n].h)){
//                    points[i].bOverButton = true;
//                } 
//            }
//        }
//        
//        for (int n=0; n<buttons.size(); n++) {
//            buttons[n].bPlaying = false;
//            for (int i=0; i<points.size(); i++) {
//                if(amIOver(points[i].pos, buttons[n].pos, buttons[n].w, buttons[n].h)){
//                    buttons[n].bPlaying = true;
//                    points[i].buttonId = n;
//                    if(points[i].prevButtonId != points[i].buttonId){
//                        points[i].playOverButton(&sender, points[i].buttonId);
//                    }
//                }
//            } 
//        }
        
        ///calculate centroid between points
        calculateCentroid(points[0].pos, points[1].pos, points[2].pos, points[3].pos, pointCounter);

    }
    
    
}


//--------------------------------------------------------------
void testApp::draw(){
    
    
    ///-----------COMPUTER VISION----------------------
    ////fullscreen mode
    if (fullView){
        
        ///-----------SYSTEM----------------------
        ///background
        ofBackground(0);
        ofSetColor(10,10,10);
        ofPushMatrix();
        ofScale(screenHeight/height, screenHeight/height);
        ofTranslate((width-height)/2 + 10,0);
        ofRect(10,10,width-20,height-20);
        
        ///draw boundary rectangle
        ofSetColor(255, 255, 255, 100);
        ofNoFill();
        ofSetLineWidth(3);
        ofRect(10, 10, width-20, height-20);
        ofSetLineWidth(1);
        ofFill();
        
        ofPopMatrix();
        

        
        ///panel and video
        if(showPanel) panel.draw(); //toggle panel draw

        ///toggle contourfinders overlay
        if (showCtrFinders){
            for (int n=0; n<N_COLORS; n++) {
                tColor[n].contourFinder.draw(0,(height/4)*n,width/4,height/4);
                ofNoFill();
                ofSetColor(255);
                ofRect(0,(height/4)*n,width/4,height/4);
                ofFill();
            }
        }
        
        ///scale everything
        ofPushMatrix();
        ofScale(screenHeight/height, screenHeight/height);
        ofTranslate((width-height)/2,0);
        ofSetFullscreen(true);
        
        ///toggle image overlay
        if (showImage){
            ofSetColor(255, 255, 255,100);
            videoColorCvImage.draw(0,0);
        }else{
            ofSetColor(255, 255, 255);
        }
        
        
        
        ///-----------GRAPHICS----------------------
        
        ///draw rhythmPoint 1
        for (int i=0;i<100 ; i++) {
            ofSetColor(255, 255, 255, 1);
            ofCircle(refPoint.x, refPoint.y, 1*i);
            
        }
        
        ///draw points
        for (int i=0; i<points.size(); i++) {
            if (tColor[i].contourFinder.nBlobs>0) {
                points[i].c.set(tColor[i].r, tColor[i].g, tColor[i].b); ///set circle color to be the same as blob
                points[i].draw(tColor[0].blobPos);
            }            
        }
        
//        ///draw buttons
//        for (int i=0; i<buttons.size(); i++) {
//            buttons[i].draw();
//        }
        


        
//        /// draw columns
//        ofNoFill();
//        ofSetColor(255);
//        ofCircle(20, 210, 40);
//        ofCircle(600, 210, 40);
//        ofFill();
        
        
        ///draw slices
        float nDivisions = 6;
        float radius = maxRadius;
        for (int i=0;i<nDivisions ; i++) {
            float angle = TWO_PI/nDivisions;
            float x = cos(angle * i) * radius + center.x;
            float y = sin(angle * i) * radius + center.y;
            ofSetColor(255,0,0,100);
            ofSetLineWidth(2);
            ofLine(center.x, center.y, x, y);
        }
        ofNoFill();
        ofSetColor(255,0,0,50);
        ofCircle(center.x, center.y, maxRadius); ///draw maximum Radius
        ofSetColor(255);
        ofFill();
        ofSetLineWidth(1);
        
        
        ///draw center pole point
        ofFill();
        ofSetColor(tColor[3].r, tColor[3].g, tColor[3].b);
        ofCircle(center.x, center.y, 13);
        ofNoFill();

        
        /*
         ///draw centroid circle
         ofCircle(triangleCentroid.x, triangleCentroid.y, 1);
         for (int i=0; i<points.size(); i++) {
         ofLine(points[i].pos.x, points[i].pos.y, triangleCentroid.x, triangleCentroid.y);
         }
         */
        
        ofPopMatrix(); ///end screen scaling
        

        
//        if (points[0].bActivity == true || 
//            points[1].bActivity == true ||
//            points[2].bActivity == true ){
//
//            startTime = systemStartTime;
//            playTime ++;
//            
//            if (playTime > limitTime) {
//                startFade ++;
//                if (startFade>255) startFade=255; 
//                ofFill();
//                ofSetColor(255, 255, 255, startFade);
//                noScreen.draw(0,0,screenWidth, screenHeight);
//            }
//            
//        }
        
        ///Welcome screen if inactive
        if (points[0].bActivity == false &&
            points[1].bActivity == false &&
            points[2].bActivity == false ){
            
            ofFill();
            ofSetColor(255,255,255);
            ofRect(0, 100, 30, 30);
//            startTime = 0;
//            playTime = 0;
//            startFade = 0;
            
            welcome.draw(0,0,screenWidth, screenHeight);
            activeSystem = false;
            
//            ofSetColor(255, 255, 255, 0);
//            noScreen.draw(0,0,screenWidth, screenHeight);
            
        } 
        
        

        

        
        
//        //show info
//        string systemT, startT, playT, currT, fadeAmount;
//        
//        playT  = "timePlayin = " + ofToString ( playTime );
//        startT = "startTime = " + ofToString ( startTime );
//        currT = "currentTime = " + ofToString ( currentTime );
//        systemT = "systemRunning = " + ofToString ( systemStartTime );
//        fadeAmount = "fadeAmount = " + ofToString ( startFade );
        
        
//        ofSetColor(255);
//        ofDrawBitmapString(playT, 10, 10);
//        ofDrawBitmapString(startT, 10, 20);
//        ofDrawBitmapString(currT, 10, 30);
//        ofDrawBitmapString(systemT, 10, 40);
//        ofDrawBitmapString(fadeAmount, 10, 50);

        
        
        
        
        
        
   
    } else{
        ////calibration mode
        ofSetFullscreen(false);
        
        /// Draw images (calibration screen)
        ofSetColor(255, 255, 255);
        videoColorCvImage.draw(0,0, width,height);
        //        videoColorHSVCvImage.draw(width, 0, width, height);
        
        if(showPanel) panel.draw(); ///toggle panel draw
        
        
        ///contourfinders
        for (int n=0; n<N_COLORS; n++) {
            ofSetColor(255, 255, 255,100);
            tColor[n].videoGrayscaleCvImage.draw(n*(width/2),height+height/3,width/2,height/2);
            ofSetColor(255, 255, 255);
            tColor[n].contourFinder.draw(n*(width/2),height+height/3,width/2,height/2);
        }
        ///draw color rectangles
        for (int n=0; n<N_COLORS; n++) {
            ofFill();
            ofSetColor(tColor[n].r, tColor[n].g, tColor[n].b);
            ofRect(n*(height/3), height, height/3, height/3);
        }
        
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    switch (key) {
            
        case 'f':
            if (fullView){
                fullView = false;
            } else {
                fullView = true;
            }
            break;
        case 'i':
            if (showImage){
                showImage = false;
            } else {
                showImage = true;
            }
            break;
        case 'p':
            if (showPanel){
                showPanel = false;
            } else {
                showPanel = true;
            }
            break;
        case 'c':
            if (showCtrFinders){
                showCtrFinders = false;
            } else {
                showCtrFinders = true;
            }
            break;
        case '1':
            panel.selectedPanel = 0;            
            break;
        case '2':
            panel.selectedPanel = 1;
            break;
        case '3':
            panel.selectedPanel = 2;            
            break;
        case '4':
            panel.selectedPanel = 3;
            break;
        case '5':
            panel.selectedPanel = 4;
            break;
            
            
            
#ifndef _USE_LIVE_VIDEO   
        case ' ':
            vidPlayer.setFrame(1);
            break;
#else
        case 's':
            video.videoSettings();
            break;
#endif
            
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    panel.mouseDragged(x, y, button);
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    panel.mousePressed(x, y, button);
    
    for (int i=0; i<N_COLORS; i++) {
        if(panel.selectedPanel==i) getColorChannels(panel.selectedPanel,x,y);
    }
    bMousePressed = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    panel.mouseReleased();
    bMousePressed = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}

//--------------------------------------------------------------
void testApp::getColorChannels(int _whichPanel, int & _x, int & _y){
    
    if (_x >= 0 && _x < width && _y >= 0 && _y < height){
        int pixel = _y * width + _x;
        
        for (int i=0; i<N_COLORS; i++) {
            tColor[_whichPanel].hue = videoGrayscaleHueImage.getPixels()[pixel];;
            tColor[_whichPanel].sat = videoGrayscaleSatImage.getPixels()[pixel];
            tColor[_whichPanel].val = videoGrayscaleBriImage.getPixels()[pixel];
            
            tColor[_whichPanel].r = videoColorCvImage.getPixels()[pixel*3];
            tColor[_whichPanel].g = videoColorCvImage.getPixels()[pixel*3+1];
            tColor[_whichPanel].b = videoColorCvImage.getPixels()[pixel*3+2];
        }
    }
    
}

//--------------------------------------------------------------
bool testApp::amIOver(ofVec2f blobPos, ofVec2f buttonPos, int w, int h){
    
    if (blobPos.x>buttonPos.x && blobPos.x<buttonPos.x+w &&
        blobPos.y>buttonPos.y && blobPos.y<buttonPos.y+h) {
        return true;
    } else {
        return false;
    }
    
    
}

//--------------------------------------------------------------
ofVec2f testApp::calculateCentroid(ofVec2f pos1, ofVec2f pos2, ofVec2f pos3, ofVec2f pos4, int pCounter){
    if (pCounter>1){
        float centroidX = (pos1.x + pos2.x + pos3.x + pos4.x) / pCounter;
        float centroidY = (pos1.y + pos2.y + pos3.y + pos4.y) / pCounter;
        
        triangleCentroid.set(centroidX, centroidY);
        return triangleCentroid;
    }else{
        return 0;
    }
}






