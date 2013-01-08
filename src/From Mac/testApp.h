#ifndef _TEST_APP
#define _TEST_APP


#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFXOBJLOADER
#define OF_ADDON_USING_OFXDIRLIST
#define OF_ADDON_USING_OFXVECTORMATH
#define OF_ADDON_USING_OFXNETWORK
#define OF_ADDON_USING_OFXVECTORGRAPHICS
#define OF_ADDON_USING_OFXOSC
#define OF_ADDON_USING_OFXTHREAD
#define OF_ADDON_USING_OFXXMLSETTINGS


#include "ofMain.h"
//#include "ofAddons.h"
#include "pixelServer.h"

class testApp : public ofSimpleApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased (int key);

		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

		pixelServer server;
		
        ofImage testImg;
		ofVideoGrabber grabber;
    
        float time_last_packet_sent=0;
};

#endif

