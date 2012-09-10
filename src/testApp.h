#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxNetwork.h"

#include "ofVec3f.h"
#include "ofxAssimpModelLoader.h"
//
//#include "GUIStuff.h"
#include"ofCamera.h"


//#include"POIS.h"

#include"Calculations.h"
#include"NoteInformation.h"



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
		void UpdateTracking();
		void setupTracker();
		void setupModels();
		void drawModels();

// UDP Stuff ..

		ofxUDPManager udpConnection;
		ofTrueTypeFont  mono;
		ofTrueTypeFont  monosm;

		ofxAssimpModelLoader bb_model,cr_model,lr_model,ls_model,pgn_model,pgy_model,pog_model,rc_model,sr_model,tbox_model;
		ofxAssimpModelLoader squirrelModel;
	
		void drawAxes();
		void drawPlane();
		void UDPReceive();
		void addObjecttoScene(string);
		ofColor Returncolor(string);
		void drawAugmentedPlane(float ,float,ofColor,ofColor,int ,string,string);
};

#endif
