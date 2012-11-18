#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
 

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 800,600, OF_WINDOW);			// <-------- setup the GL context

	// window.setFullscreen(1); 
 
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width an d height too:
	ofRunApp( new testApp());

}
