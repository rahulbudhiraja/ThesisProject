#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(20,20,20);
	server.setup(44999);
	grabber.initGrabber(320, 240, true);
    ofLog(OF_LOG_VERBOSE);
    grabber.listDevices();
    
    
    testImg.loadImage("test.jpeg");
    testImg.resize(320, 240);
    time_last_packet_sent=0;
}

//--------------------------------------------------------------
void testApp::update(){
	server.update(2048);
	grabber.grabFrame();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	string statusStr =  "status: " + server.getStateStr();
	statusStr += " -- sent "+ofToString(server.getPctSent(), 2)+"%";
	
	ofSetColor(255, 0, 255);
	ofDrawBitmapString(statusStr, 10, 20);
	ofDrawBitmapString("server - launch client than hit s key to send current frame", 10, ofGetHeight()-20);

	ofSetColor(255, 255, 255);
	grabber.draw(0, 25);
    
    unsigned char * pix = grabber.getPixels();
    if(server.getStateStr()!="SENDING"&&ofGetElapsedTimeMillis()/1000-time_last_packet_sent>4)
    {
    server.sendPixels(pix);
  time_last_packet_sent=ofGetElapsedTimeMillis()/1000;    }
    
    
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if( key == 's'){
		unsigned char * pix = grabber.getPixels();
		server.sendPixels(pix);
	}
    
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
   
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(){

}
