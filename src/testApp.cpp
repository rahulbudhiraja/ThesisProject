#include "testApp.h"

// Including all the header files for Vuzix Tracker  ..

#include "stdafx.h"
#include "IWRsdk.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <iostream>


// Function return defines.
#define IWR_OK					0 // Ok 2 go with tracker driver.
#define IWR_NO_TRACKER			1 // Tracker Driver is NOT installed.
#define IWR_OFFLINE				2 // Tracker driver installed, yet appears to be offline or not responding.
#define IWR_TRACKER_CORRUPT		3 // Tracker driver installed, and missing exports.
#define IWR_NOTRACKER_INSTANCE	4 // Tracker driver did not open properly.
#define IWR_NO_STEREO			5 // Stereo driver not loaded.
#define IWR_STEREO_CORRUPT		6 // Stereo driver exports incorrect.



// Provide for a filtering mechanism on the VR920 tracker reading.
extern void	IWRFilterTracking( long *yaw, long *pitch, long *roll );


bool g_tracking	= false;		// True = enable head tracking
float g_fYaw = 0.0f,  g_fPitch = 0.0f, g_fRoll = 0.0f;
long iwr_status;

int	g_Filtering	= 0;			// Provide very primitive filtering process.
enum { NO_FILTER=0, APPLICATION_FILTER, INTERNAL_FILTER };

int	Pid	= 0;
using namespace std;
HMODULE hMod;

//GUIStuff gui;
ofCamera cam;

/// -- Processing ...

float yaw=0,roll=0;

//vector <POIs> scenes();
vector<POIs> scenes;
vector <NoteInformation> Notes;

float rotX, rotY;

float rotAngle=0;

// Crosshair ..
ofImage crosshair;

float initialyaw=0;
///////

int i;
Calculations calc;

vector <string>result;



ofColor white =ofColor::fromHex(0xffffff); 
ofColor green= ofColor::fromHex(0x00ff00);
ofColor blue=ofColor::fromHex(0x0000ff);
ofColor yellow=ofColor::fromHex(0xffff00);
ofColor red=ofColor::fromHex(0xff0000);
ofColor black=ofColor::fromHex(0x000000);

ofTrueTypeFont Serif_15;
ofTrueTypeFont Serif_10;

//--------------------------------------------------------------
void testApp::setup()
{

	Serif_10.loadFont("Serif.ttf",10);
	Serif_15.loadFont("Serif.ttf",15);

	setupTracker();
	setupModels();


	//some model / light stuff
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);


	glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable (GL_COLOR_MATERIAL);
	//we run at 60 fps!

	ofSetFrameRate(90);

	//load our type
	mono.loadFont("type/mono.ttf", 9);
	monosm.loadFont("type/mono.ttf", 8);

	//create the socket and bind to port 11999
	udpConnection.Create();
	udpConnection.Bind(12001);
	udpConnection.SetNonBlocking(true);

	//load the squirrel model - the 3ds and the texture file need to be in the same folder

	crosshair.loadImage("crosshair.png");
	crosshair.saveImage("asdasdsa.png");

	crosshair.resize(crosshair.width*1.5,crosshair.height*1.5);
	UpdateTracking();
	initialyaw=g_fYaw;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw()
{


	ofBackground(0,0,0);
	cam.setPosition(0,0,10);
	cam.lookAt(ofVec3f(0,100,10),ofVec3f(0,0,1.0)); // have changed this a bit ..
	//BlackbayModels.ModelsDraw();

	UpdateTracking();


	cam.pan(g_fYaw);
	cam.begin();

	drawAxes();
	drawPlane();
	drawModels();

	ofSetColor(255, 255, 255);
	ofFill();
	
	 for(i=0;i<scenes.size();i++)
    {
      
        ofPushMatrix();
//        println("Xposition\t"+x_temp_poi); println("\n\nYposition\t"+y_temp_poi);  
        ofTranslate(scenes[i].x,scenes[i].y);
        
		drawAugmentedPlane(scenes[i].x,scenes[i].y,Notes[i].text_color,Notes[i].bg_color,i,Notes[i].note_heading,Notes[i].note_text);
      
        ofPopMatrix();
    }
	

	cam.end();

	UDPReceive();
	calc.check_intersection(calc.convertDegreestoRadians(g_fYaw),scenes);

	//ofFill();
	//	gui.drawAxes();
	//cout<<"\n\n "<<g_fPitch<<"\t "<<g_fRoll<<"\t"<<g_fYaw;

	ofEnableAlphaBlending();
	crosshair.draw(ofGetWidth()/2-crosshair.width/2,ofGetHeight()/2-crosshair.height/2 );
	 ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

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
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::setupTracker()
{

	if( IWRGetProductID ) 
		Pid = IWRGetProductID();

	//hMod = LoadLibrary("opengl32.dll");
	iwr_status = IWRLoadDll();

	if( iwr_status != IWR_OK ) {
		cout<<"NO VR920 iwrdriver support", "VR920 Driver ERROR";
		IWRFreeDll();

	}

}

void testApp::UpdateTracking(){
	// Poll input devices.
	long	Roll=0,Yaw=0, Pitch=0;
	iwr_status = IWRGetTracking( &Yaw, &Pitch, &Roll );
	if(	iwr_status != IWR_OK ){
		// iWear tracker could be OFFLine: just inform user, continue to poll until its plugged in...
		g_tracking = false;
		Yaw = Pitch = Roll = 0;
		IWROpenTracker();
	}


	// Always provide for a means to disable filtering;
	if( g_Filtering == APPLICATION_FILTER){ 
		IWRFilterTracking( &Yaw, &Pitch, &Roll );
	}
	// Convert the tracker's inputs to angles
	g_fPitch =  (float)Pitch * IWR_RAWTODEG; 
	g_fYaw   =  (float)Yaw * IWR_RAWTODEG;
	g_fRoll  =  (float)Roll * IWR_RAWTODEG;

}

void testApp::setupModels()
{

	cout<<"\nLoading 3D Models \n";


	// Setting the default parameters of the models ..
	bb_model.loadModel("3Dmodels/bb/model.obj",1);  
	bb_model.setScale(0.25,0.25,0.25);

	ofPoint Scene_Center=bb_model.getSceneCenter();
	bb_model.setPosition(-Scene_Center.x,-Scene_Center.y,-Scene_Center.z);

	cr_model.loadModel("3Dmodels/cr/model.dae", 1);  
	cr_model.setScale(0.1,0.1,0.1);
	cr_model.setRotation(0,90,1,0,0);

	Scene_Center=cr_model.getSceneCenter();
	cr_model.setPosition(-Scene_Center.x,-Scene_Center.y,-Scene_Center.z);

	lr_model.loadModel("3Dmodels/lr/model.dae", 1); 
	lr_model.setScale(0.2,0.2,0.2);
	lr_model.setRotation(0,90,1,0,0);

	cout<<"\n"<<lr_model.getSceneCenter();


	Scene_Center=lr_model.getSceneCenter();
	sr_model.setPosition(-Scene_Center.x,-Scene_Center.y,-Scene_Center.z);

	sr_model.loadModel("3Dmodels/rc/model.dae", 1);  
	sr_model.setScale(0.25,0.25,0.25);
	sr_model.setRotation(0,90,1,0,0);


	Scene_Center=sr_model.getSceneCenter();
	sr_model.setPosition(-Scene_Center.x,-Scene_Center.y,-Scene_Center.z);
	cout<<"\n"<<sr_model.getSceneCenter();

	cout<<"\n Models loaded successfully";


}


void testApp::drawModels()
{
	ofNoFill();

	ofSetColor(255,255,255);

	ofPushMatrix();
	ofTranslate(116.747,-163.28,1);
	ofRotateZ(rotAngle);



	bb_model.drawFaces();


	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(280.72705,244.184);

	ofRotateZ(rotAngle);

	cr_model.drawFaces();


	ofPopMatrix();

	ofPushMatrix();

	ofTranslate(-250,-300);
	ofRotateZ(rotAngle);

	lr_model.enableColors();
	lr_model.drawFaces();

	ofPopMatrix();

	ofPushMatrix();

	ofTranslate(-100.23,129.49);

	ofRotateZ(rotAngle);
	sr_model.enableColors();
	sr_model.drawFaces();

	ofPopMatrix();

	rotAngle+=0.1;



}

void testApp::drawAxes()
{

	ofSetColor(255, 0,0);
	ofLine(100,0,0,0); 

	ofSetColor(0,255,0);
	ofLine(0,100,0,0); 

	ofSetColor(0,0,255);
	ofLine(0,0,0,0,0,100); 

}

void testApp::drawPlane()
{

	ofSetColor(255,255,255);
	//  translate(0,0,-10);
	for(int i=-200;i<200;i+=20)
	{
		ofLine(-200,i,200,i);
		ofLine(i,-200,i,200);
	}
	//    translate(0,0,10);
}

void testApp::UDPReceive()
{
	char udpMessage[100000];
	udpConnection.Receive(udpMessage,100000);
	cout<<udpMessage;

	if(udpMessage!="")
	{
		result=ofSplitString(ofToString(udpMessage),",");

		if(result[0].compare("note")==0)
			addObjecttoScene(ofToString(udpMessage));

		else if(result[0].compare("color")==0)
		{
			if(result[1].length()>2)
				Notes.back().text_color=Returncolor(result[1]);
			if(result[2].length()>2)
				Notes.back().bg_color=Returncolor(result[2]);
     
  }
    }

}

void testApp::addObjecttoScene(string message)
{
 vector <string>messageParts;
 messageParts=ofSplitString(message,",");
 
 NoteInformation temp_note=NoteInformation(messageParts[3],messageParts[4],white,black);

 if(messageParts[0].compare("note")==0)
 {
   cout<<"\n\n"<<messageParts[1]<<"\t\t"<<messageParts[2]; 
   Notes.push_back(temp_note); 
   
   cout<<messageParts[3]<<"\t\t"<<messageParts[4];

   calc.Calculate2dCoordinates(calc.lat1,calc.long1,ofToFloat(messageParts[1]),ofToFloat(messageParts[2])); 
  
 }
 
  cout<<"Xposition\t"<<calc.x_temp_poi;  cout<<"\n\nYposition\t"<<calc.y_temp_poi;  

  POIs temp_POI=POIs(calc.x_temp_poi,calc.y_temp_poi);
  scenes.push_back(temp_POI);

}

ofColor testApp::Returncolor(string colorstring)
{
	 
  if(colorstring.compare("white")==0)
  return white;
  
  if(colorstring.compare("green")==0)
  return green;
  
  if(colorstring.compare("red")==0)
  return red;
  
  if(colorstring.compare("black")==0)
  return black;
  
  if(colorstring.compare("yellow")==0)
  return yellow;
  
  if(colorstring.compare("blue")==0)
  return blue;
  
  else return black;
  
}

	void testApp::drawAugmentedPlane(float xPosition,float yPosition,ofColor textColor,ofColor bgColor,int i,string note_heading,string note_text)
	{

		

		//  rotateZ( P(bearing)   );

		//  println("Bearing"+bearing);

		//  Notes.lastElement().

		ofTranslate(xPosition,yPosition,0); 

		ofRotateZ(180-calc.convertRadianstoDegrees(PI/2+acos((xPosition/sqrt(xPosition*xPosition+yPosition*yPosition))) ) ); // Moving the plane so that it faces the user ...

		//println(PI/2-acos((xPosition/sqrt(xPosition*xPosition+yPosition*yPosition)) ) );
		ofPushMatrix();

		ofRotateZ(PI/2);

		ofSetColor(bgColor);

		ofBeginShape();

		ofVertex(0,-50,0);
		ofVertex(0,50,0);
		ofVertex(0,50,50);
		ofVertex(0,-50,50);


		ofEndShape();

		ofSetColor(textColor);

		ofRotateX(-90);
		ofRotateY(+90);

		ofTranslate(0,0,-1);
		ofDrawBitmapString(note_heading,-50,-40);
		ofDrawBitmapString(note_text,-50,-30);

		ofPushMatrix();
		

		

		ofPopMatrix();

		//ofSetColor(textColor);

		//ofPushMatrix();
		//ofRotateX(-PI/2); 

  //		Serif_15.drawString(note_heading,-50,-40); // May require Changes ..
		//Serif_10.drawString(note_text,-50,-30);


		ofPopMatrix();


	}