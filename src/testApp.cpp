////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file src\testApp.cpp
///
/// \brief Implements the test application class.
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "testApp.h"
// Including all the header files for Vuzix Tracker  ..
#include "stdafx.h"
#include "IWRsdk.h"
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <iostream>
#define IWR_OK					0 // Ok 2 go with tracker driver.
#define IWR_NO_TRACKER			1 // Tracker Driver is NOT installed.
#define IWR_OFFLINE				2 /// Tracker driver installed, yet appears to be offline or not responding.
#define IWR_TRACKER_CORRUPT		3 /// Tracker driver installed, and missing exports.
#define IWR_NOTRACKER_INSTANCE	4 // Tracker driver did not open properly.
#define IWR_NO_STEREO			5 // Stereo driver not loaded.
#define IWR_STEREO_CORRUPT		6 // Stereo driver exports incorrect.

//#define USEARMARKER
//#define NEWTRACKBLOBS

extern void	IWRFilterTracking( long *yaw, long *pitch, long *roll );

bool g_tracking	= false;		// True = enable head tracking
/// \brief The roll.
float g_fYaw = 0.0f,  g_fPitch = 0.0f, g_fRoll = 0.0f;
/// \brief The iwr status.
long iwr_status;
/// \brief The filtering.
int	g_Filtering	= 0;			// Provide very primitive filtering process.

enum { NO_FILTER=0, APPLICATION_FILTER, INTERNAL_FILTER };


int	Pid	= 0;
using namespace std;
HMODULE hMod;

ofCamera cam,dummy,radarCam;


/// \brief The roll.
float yaw=0,roll=0;

/// \brief The scenes.
///
/// This Vector stores the Scene Information
vector<POIs> scenes;
vector <NoteInformation> Notes;
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \property float rotX, rotY
///
/// \brief Gets the rot y coordinate.
///
/// \return The rot y coordinate.
////////////////////////////////////////////////////////////////////////////////////////////////////
float rotX, rotY;
/// \brief The rot angle.
float rotAngle=0;

/// \brief The crosshair.
ofImage crosshair;
/// \brief The initialyaw.
float initialyaw=0;
///////

int i;
Calculations calc;
/// \brief .
vector <string>result;

/// \brief The white.
ofColor white =ofColor::fromHex(0xffffff); 
ofColor green= ofColor::fromHex(0x00ff00);
/// \brief The blue.
ofColor blue=ofColor::fromHex(0x0000ff);
ofColor yellow=ofColor::fromHex(0xffff00);
/// \brief The red.
ofColor red=ofColor::fromHex(0xff0000);
ofColor black=ofColor::fromHex(0x000000);
/// \brief The fifth serif 1.
ofTrueTypeFont Serif_15;
ofTrueTypeFont Serif_10;
bool sent=false;
string dirn="up";
int bb_model_touch_trans_x=0;
//Storing the last touch-positions
float last_single_touch[2];
long last_time_select=0;
float midPoint_x=0,midPoint_y=0;
bool beginCrossDimSelection=false,keepCrossDimSelected=true;
int crossDimObjSelected=0;
long time_begin_crossDimSelection=0;


int guiWidth=300,guiHeight=125;

//# define TRACKBLOBS

# define USERSTUDY


////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void testApp::setup()
///
/// \brief OpenFrameworks First method that is called at the beginning of the program.Initialize all Variables Here.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void testApp::setup()
{
	last_gesture_selected=gestureType=0;
	 
	setupTracker();
	setupFonts();
	setupUDPConnections();
	setupCrosshair();
	beginSelection=false;
	//some model / light stuff
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);

	glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable (GL_COLOR_MATERIAL);
	ofSetFrameRate(90);
	UpdateTracking();
	initialyaw=g_fYaw;
	last_time_select=0;
	crosshair_selected=0;


	loadModelsfromXML();
	
	AndroidPhoneResWidth=800;AndroidPhoneResHeight=480; /// 

#ifdef ARMARKER
	SetupImageMatrices();
	setupARToolkitStuff();
	iphone5Model.loadModel("iphone/iPhone5.dae",false);
	iphone5Model.setScaleNomalization(false);
	iphone5Model.setScale(0.03,0.03,0.03);
	iphone5Model.setRotation(0,90,0,0,1);
#endif 

#ifdef TRACKBLOBS

	setupImgMatrices();

#endif


#ifdef NEWTRACKBLOBS

	setupCameraforBlobs();
		ScreenGrabbedImage.allocate(ofGetScreenWidth(),ofGetScreenHeight(),OF_IMAGE_COLOR);
#endif


	viewport_x=viewport_y=0;
	
	server.setup(44999);

	

	showText=true;

#ifdef USERSTUDY

	TargetPosition.x=TargetPosition.y=TargetPosition.z=0;
	CursorPosition.x=ofGetWidth()/2;CursorPosition.y=ofGetHeight()/2;
	RandomizeTargetPosition();
	bullsEye.loadImage("bullseye.png");
	bullsEye.resize(100,100);

#endif

	 startTime=ofGetElapsedTimeMillis();
	 motionSensitivity=20;
	//SaveAndRecordTimes();
}
//--------------------------------------------------------------
void testApp::update(){

	#ifdef ARMARKER

	GrabCameraFrameandConvertMatrices();

	#endif

#ifdef TRACKBLOBS

	GrabCameraFrameandSetThreshold();

#endif

#ifdef NEWTRACKBLOBS
	GrabFrameandFindContours();
	last_camerapos_sent=0;
#endif

	server.update(2048);
	;//ScreenGrabbedImage.grabScreen(0,0,320,240);
}
//--------------------------------------------------------------
void testApp::draw()
{
	windowWidth=ofGetWidth();
	windowHeight=ofGetHeight();
	ofBackground(0,0,0);
	//UpdateTracking();

	
	if(g_fPitch>-25)
	{
		if(dirn.compare("down")==0)
		{
			
			if(ofGetSystemTime()-last_camerapos_sent>50)
				{last_camerapos_sent=ofGetSystemTime();
			int sent = udpSendConnection.Send("Change",6);//// Send a Change message and change to dirn.compare==up;
			}dirn="up";
		}

		// Viewport Experiments 
		//ofViewport(0,0,ofGetWidth()+10*viewport_x,ofGetHeight()+10*viewport_y);
		

		cam.setPosition(0,0,10);
		cam.lookAt(ofVec3f(0,100,10),ofVec3f(0,0,1.0)); // have changed this a bit ..
		
		cam.pan(g_fYaw);
		cam.tilt(g_fPitch);

		/*Temp_viewport=ofGetCurrentViewport();
		Temp_viewport.x=viewport_x;
		Temp_viewport.y=viewport_y;*/
	
		
		cam.begin();

		//cam.enableOrtho();
			
		drawAxes();
		drawPlane();
		//drawModels();

#ifndef USERSTUDY

		drawModelsXML();
#endif

		
		//texScreen.loadScreenData(100,100,300,300);
	
		
		ofSetColor(255, 255, 255);
		ofFill();

		
#ifndef USERSTUDY
	
		for(i=0;i<scenes.size();i++)
		{
			ofPushMatrix();
			ofTranslate(scenes[i].x,scenes[i].y);
			drawAugmentedPlane(scenes[i].x,scenes[i].y,Notes[i].text_color,Notes[i].bg_color,i,Notes[i].note_heading,Notes[i].note_text);
			ofPopMatrix();
		}
#endif

#ifdef ARMARKER
		if(artk.getNumDetectedMarkers()!=0)
		{	
			
			//iphone5Model.setPosition(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()-artk.getCameraPosition(0).y/10);
			//ofBox(ofVec3f(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()+artk.getCameraPosition(0).y/10),20);
					
			ofPushMatrix();
			ofRotateZ(g_fYaw);
			ofTranslate(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()-artk.getCameraPosition(0).y/10);
			ofRotateX(90);
			ofSetColor(255,0,0);
			ofRect(-136.6/20,0,-70.6/20,136.6/10,70.6/10);
			//iphone5Model.drawFaces();
			//ofBox(ofVec3f(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()+artk.getCameraPosition(0).y/10),20);
			ofPopMatrix();


			//cout<<"\n\n Marker Detected"<<artk.getNumDetectedMarkers()<<"     "<<ofGetSystemTime();
			string CameraPosition="CameraPos,"+ofToString(-artk.getCameraPosition(0).x/10)+","+ ofToString(artk.getCameraPosition(0).z/10)+","+ofToString(-artk.getCameraPosition(0).y/10+10)+",end";
			int sent = udpSendCameraPosition.Send(CameraPosition.c_str(),CameraPosition.length());
			cout<<"\n\n Camera Position  "<<artk.getCameraPosition(0).x/10 <<"\t\t"<<artk.getCameraPosition(0).y/10<<"\t\t"<<artk.getCameraPosition(0).z/10;
		}
#endif
		
	
		cam.end();
		

#ifdef TRACKBLOBS

		// We want to draw a line only if its a valid position
		if(yellowBlobCoordinates[0]!=0&&blueBlobCoordinates[0]!=0&&greenBlobCoordinates[0]!=0&&redBlobCoordinates[0]!=0)
		{
			// Draw a yellow line from the previous point to the current point
		/*	cvLine(imgScribble, cvPoint(yellowBlobCoordinates[0],yellowBlobCoordinates[1]), cvPoint(greenBlobCoordinates[0],greenBlobCoordinates[1]), cvScalar(0,255,255), 15);
			cvLine(imgScribble, cvPoint(redBlobCoordinates[0],redBlobCoordinates[1]), cvPoint(yellowBlobCoordinates[0],yellowBlobCoordinates[1]), cvScalar(0,255,255), 15);
			cvLine(imgScribble, cvPoint(redBlobCoordinates[0],redBlobCoordinates[1]), cvPoint(blueBlobCoordinates[0],blueBlobCoordinates[1]), cvScalar(0,255,255), 15);
			cvLine(imgScribble, cvPoint(blueBlobCoordinates[0],blueBlobCoordinates[1]), cvPoint(greenBlobCoordinates[0],greenBlobCoordinates[1]), cvScalar(0,255,255), 15);*/

			cout<<"Passing"<<endl;

			ofEnableAlphaBlending();

			ofSetColor(0,0,255);
			ofCircle(ofPoint(blueBlobCoordinates[0],blueBlobCoordinates[1]),20);
			ofSetColor(255,0,0);
			ofCircle(ofPoint(redBlobCoordinates[0],redBlobCoordinates[1]),20);
			ofSetColor(0,255,0);
			ofCircle(ofPoint(greenBlobCoordinates[0],greenBlobCoordinates[1]),20);
			ofSetColor(0,255,255);
			ofCircle(ofPoint(yellowBlobCoordinates[0],yellowBlobCoordinates[1]),20);

			ofSetColor(0);

			ofBeginShape();
			ofVertex(blueBlobCoordinates[0],blueBlobCoordinates[1]);
			ofVertex(redBlobCoordinates[0],redBlobCoordinates[1]);
			ofVertex(yellowBlobCoordinates[0],yellowBlobCoordinates[1]);
			ofVertex(greenBlobCoordinates[0],greenBlobCoordinates[1]);
			ofEndShape();
				


			//ofLine()

			ofLine(yellowBlobCoordinates[0],yellowBlobCoordinates[1], greenBlobCoordinates[0],greenBlobCoordinates[1]);
			ofLine(redBlobCoordinates[0],redBlobCoordinates[1], greenBlobCoordinates[0],greenBlobCoordinates[1]);
			ofLine(redBlobCoordinates[0],redBlobCoordinates[1], blueBlobCoordinates[0],blueBlobCoordinates[1]);
			ofLine(blueBlobCoordinates[0],blueBlobCoordinates[1], yellowBlobCoordinates[0],yellowBlobCoordinates[1]);




			ofDisableAlphaBlending();
		}

		// Add the scribbling image and the frame...
		//cvShowImage("thresh", imgYellowThresh);

#endif
		;//texScreen.draw(100,100);

#ifdef NEWTRACKBLOBS
		ofSetColor(255,255,0);
		for (int i=0; i<contours.nBlobs; i++) 
			ofCircle(contours.blobs[i].centroid.x*ofGetWindowWidth()/cameraWidth, contours.blobs[i].centroid.y*ofGetWindowHeight()/cameraHeight, 20);
		
		ofSetColor(0,255,0);
		for (int i=0; i<contours2.nBlobs; i++) 
			ofCircle(contours2.blobs[i].centroid.x*ofGetWindowWidth()/cameraWidth, contours2.blobs[i].centroid.y*ofGetWindowHeight()/cameraHeight, 20);
		
		ofSetColor(0);

		

		if(contours.nBlobs>0&&contours2.nBlobs>0)
		    {
				
				ScreenGrabbedImage.resize((abs(contours.blobs[0].centroid.x-contours2.blobs[0].centroid.x)*ofGetWindowWidth())/cameraWidth,(abs(contours.blobs[0].centroid.y-contours2.blobs[0].centroid.y)*ofGetWindowHeight())/cameraHeight);
				ScreenGrabbedImage.grabScreen(min(contours.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth, contours2.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth),min(contours.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight, contours2.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight),(abs(contours.blobs[0].centroid.x-contours2.blobs[0].centroid.x)*ofGetWindowWidth())/cameraWidth,(abs(contours.blobs[0].centroid.y-contours2.blobs[0].centroid.y)*ofGetWindowHeight())/cameraHeight);
				ScreenGrabbedImage.resize(320,240);
				ofRect(min(contours.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth, contours2.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth),min(contours.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight, contours2.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight),(abs(contours.blobs[0].centroid.x-contours2.blobs[0].centroid.x)*ofGetWindowWidth())/cameraWidth,(abs(contours.blobs[0].centroid.y-contours2.blobs[0].centroid.y)*ofGetWindowHeight())/cameraHeight);
		}
		if(contours.nBlobs>0&&contours2.nBlobs>0&&(ofGetSystemTime()/1000)-last_camerapos_sent>6)
		{string CameraPosition="Yaw,"+ofToString(g_fYaw)+",X,"+ ofToString(min(contours.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth, contours2.blobs[0].centroid.x*ofGetWindowWidth()/cameraWidth))+",Y,"+ofToString(min(contours.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight, contours2.blobs[0].centroid.y*ofGetWindowHeight()/cameraHeight))+",width,"+ofToString(abs(contours.blobs[0].centroid.x-contours2.blobs[0].centroid.x)*ofGetWindowWidth()/cameraWidth)+",height,"+ofToString(abs(contours.blobs[0].centroid.y-contours2.blobs[0].centroid.y)*ofGetWindowHeight()/cameraHeight)+",tilt,"+ofToString(g_fPitch)+",end,";
		;//int sent = udpSendCameraPosition.Send(CameraPosition.c_str(),CameraPosition.length());
		//cout<<"\n\n Camera Position  "<<artk.getCameraPosition(0).x/10 <<"\t\t"<<artk.getCameraPosition(0).y/10<<"\t\t"<<artk.getCameraPosition(0).z/10;
		last_camerapos_sent=ofGetSystemTime()/1000;
		
		
		}

#endif
		string message=Receive_Message();
	
		if(message.length()>0)
			;
		else ;//gestureType=0;//cout<<"\n\n UDP received Message"<<message;
		
		
		if(message.compare("Left Swipe")==0&&gestureType==4&&(ofGetSystemTime()-time_begin_crossDimSelection)<2000)
		{
			//cout<<"Left Swipe";
			string sendMessage="sAct,"+ofToString(crossDimObjSelected);
			cout<<sendMessage<<"\n\n";
			int sent = udpSendConnection.Send(sendMessage.c_str(),6);
		}

		else if(message.compare("Stop")==0)
			gestureType=0;

		
		
		else if(message.length()>0)
			{
				convertPhonetoScreenCoordinates(message);
#ifdef USERSTUDY

				result=ofSplitString(ofToString(message),",");
				
				if(result[0].compare("Sensitivity")==0)
				{
					motionSensitivity=ofToFloat(result[1]);
					cout<<motionSensitivity<<endl;
				}
				
				else if(result[0].compare(("Initialize"))==0)
				{
					
					motionSensitivity=ofToFloat(result[2]);
					activeGesture=ofToInt(result[4])+1;
					AndroidPhoneResWidth=ofToInt(result[8]);
					AndroidPhoneResHeight=ofToInt(result[10]);
					startTime=ofGetElapsedTimeMillis();
					cout<<"message"<<message;
				}

				else if(result[0].compare("End")==0)
				{

					SaveAndRecordTimes();
					ofExit(0);
					
				}
				
				else if(result[0].compare(("Sensor"))==0)
				{
					gestureType=6;
					
					CursorPosition.x-=ofToFloat(result[2])/motionSensitivity;
					CursorPosition.y+=ofToFloat(result[4])/motionSensitivity;
					
					if(CursorPosition.x<0)
						CursorPosition.x=0;
					else if(CursorPosition.x>ofGetWidth())
						CursorPosition.x=ofGetWidth();
					
					if(CursorPosition.y<0)
						CursorPosition.y=0;
					else if(CursorPosition.y>ofGetHeight())
						CursorPosition.y=ofGetHeight();


				}

				else
				{
					drawTouchImpressions(result,false);
					checkifCursorIntersectsFinger();
				
				cout<<result[result.size()-2]<<endl;
				
				if(isCursorSelected)
				classifyGesture(message);
                }
#endif

		}

		

		if(gestureType==0&&last_gesture_selected&&convertedTouchPoints[0]==1)
		{
			gestureType=1;
			result=ofSplitString(ofToString(message),",");
			drawTouchImpressions(result,false);
			last_gesture_selected=!last_gesture_selected;
			cout<<"Blah";
		}
		else if(gestureType>=1&&gestureType<=6 &&gestureType!=4)
#ifdef USERSTUDY
		{
			cout<<"result\n\n";
			if(message.length()>0)
			{

			
			result=ofSplitString(ofToString(message),",");
			cout<<"translate\n\n";
			translateCursor(result);
			}
		}
#endif	

#ifndef USERSTUDY

	translate_3D_Model(message); // This should be replaced with Translate Cursor ..

#endif

		else {	
			beginSelection=false;
			calc.touch_selected=0;
			gestureType=0;
			
#ifndef USERSTUDY
resetModelVariables();
			updateModelPositions();

			Check_for_crosshair_model_intersection();
			UDPReceive();

			if(message.length()>0&&message.compare("Left Swipe")!=0)
				drawTouches(message);

			else calc.touch_selected=0;
			if(crosshair_selected)
				DrawDescription(ModelsList[crosshair_selected-1].getDescription());
#endif			
				
			
		}
	}
	else if(dirn.compare("up")==0)
	{
		int sent = udpSendConnection.Send("Change",6);;//// Send a Change message and change to dirn.compare==up;
		dirn="down";
	}
	
	
	if(convertedTouchPoints.size()>0)
	{		ofDrawBitmapString(ofToString(Check_for_Finger_Intersections()),ofGetWidth()-430,30);
	convertedTouchPoints.clear();
	}

#ifdef NEWTRACKBLOBS
	string statusStr =  "status: " + server.getStateStr();
	statusStr += " -- sent "+ofToString(server.getPctSent(), 2)+"%";

	ofSetColor(255, 0, 255);

	if(showText)
	{
	ofDrawBitmapString(statusStr, 10, 20);
	ofDrawBitmapString("server - launch client than hit s key to send current frame", 10, ofGetHeight()-20);
	}
#endif
	// Just commenting this out for the viewport experiments ..
	drawRadar();

	//	drawCrosshair();

	checkifTargetisonScreen();
	drawTargetandCursor();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	yaw++;


	// Viewport Experiments 
	if(key==OF_KEY_DOWN)
		viewport_y++;
	else if(key==OF_KEY_RIGHT)
		g_fYaw--;
	else if(key==OF_KEY_LEFT)
		g_fYaw++;
	else if(key==OF_KEY_UP)
		viewport_y--;
#ifdef NEWTRACKBLOBS	
	if(key=='s')
		{
			ScreenGrabbedImage.saveImage("test.jpg");
			
				server.sendPixels(ScreenGrabbedImage.getPixels());
		ScreenGrabbedImage.saveImage("test.jpg");
	}

	else if(key=='t')
		showText=!showText;
	#endif
	else if(key=='f')
		ofToggleFullscreen();
}
// --------------------------------------------------------------. 
void testApp::keyReleased(int key){
}
// --------------------------------------------------------------. 
void testApp::mouseMoved(int x, int y ){
}
// --------------------------------------------------------------. 
void testApp::mouseDragged(int x, int y, int button){
}
// --------------------------------------------------------------. 
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
		//cout<<"NO VR920 iwrdriver support", "VR920 Driver ERROR";
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void testApp::drawAxes()
///
/// \brief Draw axes.
///
/// \author Rahul
/// \date 10/28/2012
////////////////////////////////////////////////////////////////////////////////////////////////////

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
	//cout<<udpMessage;
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
		//cout<<"\n\n"<<messageParts[1]<<"\t\t"<<messageParts[2]; 
		Notes.push_back(temp_note); 
		//cout<<messageParts[3]<<"\t\t"<<messageParts[4];
		calc.Calculate2dCoordinates(calc.lat1,calc.long1,ofToFloat(messageParts[1]),ofToFloat(messageParts[2])); 
		/// \brief .
	}
	//cout<<"Xposition\t"<<calc.x_temp_poi;  //cout<<"\n\nYposition\t"<<calc.y_temp_poi;  
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
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn drawAugmentedPlane(float xPosition,float yPosition,ofColor textColor,
/// ofColor bgColor,int i,string note_heading,string note_text)
///
/// \brief Draw augmented plane.
///
/// \author Rahul
/// \date 9/21/2012
///
/// \param xPosition    The position.
/// \param yPosition    The position.
/// \param textColor    The text color.
/// \param bgColor	    The background color.
/// \param i		    Zero-based index of the.
/// \param note_heading The note heading.
/// \param note_text    The note text.
////////////////////////////////////////////////////////////////////////////////////////////////////
void testApp::drawAugmentedPlane(float xPosition,float yPosition,ofColor textColor,ofColor bgColor,int i,string note_heading,string note_text)
{
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
	ofPopMatrix();

}

void testApp::drawTouches(string udpMessage)
{
	vector<string> components= ofSplitString(udpMessage,",");
	ofSetColor(255,0,0,20);
	storeFingerPosition(components);
	ofFill();
	ofEnableAlphaBlending();
	//Check_if_Finger_Intersects_3DModel(components);
	drawTouchImpressions(components,false);
	Check_for_Finger_Intersections();
	////cout<<"           "<<components[components.size()-2]<<endl;
	if(ofToInt(components[0])==1&&components[components.size()-2]=="S"&&calc.touch_selected!=0)
	{
		gestureType=1;
		//cout<<"\nThe 3D Model is"<<calc.touch_selected;
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==2&&components[components.size()-2]=="HC"&&calc.touch_selected!=0)
	{
		gestureType=2;
		//cout<<"\n Head Crusher";
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==2&&components[components.size()-2]=="SF"&&calc.touch_selected!=0)
	{
		gestureType=3;
		cout<<"\n Scaling Gesture";
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==1&&components[components.size()-1]=="CD"&&calc.touch_selected!=0)
	{
		gestureType=4;
		//cout<<"\n Cross Dimensional";
		time_begin_crossDimSelection=ofGetSystemTime();
		crossDimObjSelected=calc.touch_selected;
	}

	////cout<<"g-Fyaw"<<g_fYaw<<endl;
	components.clear();
	ofDisableAlphaBlending();
	ofSetColor(255);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn void testApp::translate_3D_Model(string message)
///
/// \brief Translate 3 d model.%ip%
///
/// \author Rahul
/// \date 10/28/2012
///
/// \param message The message.
////////////////////////////////////////////////////////////////////////////////////////////////////

void testApp::translate_3D_Model(string message)
{
	result=ofSplitString(ofToString(message),",");
	drawTouchImpressions(result,true);
	translateModel(result);

}


void testApp::setupFonts()
{
	Serif_10.loadFont("Serif.ttf",10);
	Serif_25.loadFont("Serif.ttf",25);
	
	///load our type
	mono.loadFont("type/mono.ttf", 9);
	monosm.loadFont("type/mono.ttf", 8);
}
void testApp::setupUDPConnections()
{
	udpConnection.Create();
	udpConnection.Bind(12001);
	udpConnection.SetNonBlocking(true);
	udpSendConnection.Create();
	udpSendConnection.Connect("10.0.0.3",13001);
	udpSendConnection.SetNonBlocking(true);
	udpReceiveConnection.Create();
	udpReceiveConnection.Bind(12003);
	udpReceiveConnection.SetNonBlocking(true);

	udpSendCameraPosition.Create();
	udpSendCameraPosition.Connect("10.0.0.3",12005);
	udpSendCameraPosition.SetNonBlocking(true);
}
void testApp::setupCrosshair()
{
	crosshair.loadImage("crosshair.png");
	crosshair.saveImage("asdasdsa.png");
	crosshair.resize(crosshair.width*1.5,crosshair.height*1.5);
}
void testApp::updateModelPositions()
{
	for(int i	=0;i<ModelsList.size();i++)
		ModelsList[i].updatePosition();
}
void testApp::drawCrosshair()
{
	ofEnableAlphaBlending();
	ofSetColor(255);
	crosshair.draw(ofGetWidth()/2-crosshair.width/2,ofGetHeight()/2-crosshair.height/2 );
	ofDisableAlphaBlending();
}
string testApp::Receive_Message()
{		
	char udpMessage[100000];
	udpReceiveConnection.Receive(udpMessage,100000);
	return ofToString(udpMessage);
}



void testApp::drawTouchImpressions(vector <string>message,bool somethingSelected=false)
{
	if(ofToInt(message[0])==1)
	{
		ofFill();
		//ofEnableAlphaBlending();

		if(somethingSelected)
		{
			ofSetColor(0,255,0,90);
			ofCircle(ofGetWidth()*ofToFloat(message[1])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[2])/AndroidPhoneResHeight,0,40);
		}
		else 
		{
			ofSetColor(0,0,255);
			ofCircle(ofGetWidth()*ofToFloat(message[1])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[2])/AndroidPhoneResHeight,0,20);
		}
		ofDisableAlphaBlending();
	}
	else if(ofToInt(message[0])==2)
	{
		ofFill();
		ofEnableAlphaBlending();
		if(somethingSelected)
		{
			ofSetColor(0,255,0,90);

			ofCircle(ofGetWidth()*ofToFloat(message[1])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[2])/AndroidPhoneResHeight,0,40);
			ofCircle(ofGetWidth()*ofToFloat(message[3])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[4])/AndroidPhoneResHeight,0,40);
		}
		else 
		{
			ofSetColor(255,0,0);
			ofCircle(ofGetWidth()*ofToFloat(message[1])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[2])/AndroidPhoneResHeight,0,20);
			ofCircle(ofGetWidth()*ofToFloat(message[3])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[4])/AndroidPhoneResHeight,0,20);
		}
		ofDisableAlphaBlending();
	}
}

void testApp::translateModel(vector <string>message)
{

	/* if(ofToInt(message[0])==1&&gestureType==2)
	{
	gestureType=1;
	cout<<"Changed from Two Fingers to One Finger"<<"";
	}*////
	if(gestureType==2)
		last_gesture_selected=true;

	if(ofToInt(message[0])==1&&gestureType==1)
	{
		cout<<"Passing";
		//find which model is selected	;
		if(!beginSelection)
		{	
			last_single_touch[0]=(ofToFloat(message[1]));
			last_single_touch[1]=(ofToFloat(message[2]));
			beginSelection=!beginSelection;
		}
		for(int i=0;i<ModelsList.size();i++)
		{
			/// Have to add the other calc.touch_selected because deleting it will cause multiple objects to be dragged along and translated ..
			if(ModelsList[i].touch_selected&&calc.touch_selected==i+1)
				ModelsList[i].translateModel(message,last_single_touch,1,dummy);
		}


	}
	else  if(ofToInt(message[0])==2&&gestureType==2)
	{
		//find which model is selected	;
		if(!beginSelection)
		{	
			last_single_touch[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
			last_single_touch[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
			beginSelection=!beginSelection;
		}

		for(int i=0;i<ModelsList.size();i++)
		{
			if(ModelsList[i].touch_selected&&calc.touch_selected==i+1)
				ModelsList[i].translateModel(message,last_single_touch,2,cam,windowWidth,windowHeight,AndroidPhoneResWidth,AndroidPhoneResHeight);
		}

	}
	else if(ofToInt(message[0])==2&&gestureType==3)
	{
	
		for(int i=0;i<ModelsList.size();i++)
		{
			if(calc.touch_selected==i+1)
				ModelsList[i].setScale(ofToFloat(message[message.size()-1]),ofToFloat(message[message.size()-1]),ofToFloat(message[message.size()-1]));
			if(ModelsList[i].getModel().getScale().x<1.2)
				ModelsList[i].setPreviousScale();
			else ModelsList[i].updateScale();
		}
	}

	else gestureType=0;


	//cout<<gestureType<<"\n\n";
}
void testApp::storeFingerPosition(vector<string> components)
{
	/* If its one finger */
	if(ofToFloat(components[0])==1)
	{
		last_single_touch[1]=ofToFloat(components[1]);
		last_single_touch[2]=ofToFloat(components[2]);
		////cout<<sw<<endl;
	}
	/* If its 2 fingers */ 
	else if(ofToFloat(components[0])==2)
	{
		last_single_touch[1]=(ofToFloat(components[1])+ofToFloat(components[3]))/2;
		last_single_touch[2]=(ofToFloat(components[2])+ofToFloat(components[4]))/2;
	}
}


int testApp:: Check_for_crosshair_model_intersection()
{
	Crosshair_coords.push_back(1);
	Crosshair_coords.push_back(ofGetWidth()/2);
	Crosshair_coords.push_back(ofGetHeight()/2);
	crosshair_selected=0;

	for (int i=0;i<ModelsList.size();i++)
	{
		if(ModelsList[i].intersect_model(Crosshair_coords,cam)&&calc.touch_selected==0)	
			crosshair_selected=i+1;
	}

	Crosshair_coords.clear();
	return crosshair_selected;
}

int testApp::Check_for_Finger_Intersections()
{
	int selection=0;
	for (int i=0;i<ModelsList.size();i++)
	{
		if(ModelsList[i].intersect_model(convertedTouchPoints,cam)&&calc.touch_selected==0)
		{	ModelsList[i].touch_selected=true;
		calc.touch_selected=i+1;
		}
	}
	//cout<<"\n\nSelected	"<<selection;
	return calc.touch_selected;

	
}


void testApp::convertPhonetoScreenCoordinates(string rawTouchPoints)
{
	vector<string> splitComponents= ofSplitString(rawTouchPoints,",");
	//cout<<"Pass 1 "<<splitComponents[0];

	//convertedTouchPoints[0]=ofToInt(splitComponents[0]);
	convertedTouchPoints.push_back(ofToInt(splitComponents[0]));
	//	cout<<"Pass 2 ";
	for(int i=1;i<=2*convertedTouchPoints[0];i+=2)
	{
		convertedTouchPoints.push_back(ofGetWidth()*ofToFloat(splitComponents[i])/AndroidPhoneResWidth);
		convertedTouchPoints.push_back(ofGetHeight()*ofToFloat(splitComponents[i+1])/AndroidPhoneResHeight);
	}
	//cout<<"Pass 3 "<<convertedTouchPoints.size();
}

void testApp::loadModelsfromXML()
{
	ModelsFile.loadFile("Models.xml");	
	ModelsFile.pushTag("ModelsList");
	//cout<<ModelsFile.getNumTags("Model");
	for(int i=0;i<ModelsFile.getNumTags("Model");i++)
	{
		ModelsFile.pushTag("Model",i);
		Models model_object;
		model_object.setPath(ModelsFile.getValue("Path","",0));
		model_object.setId(ModelsFile.getValue("Id",0,0));
		model_object.setScale(ModelsFile.getValue("Scale::x",0.0,0),ModelsFile.getValue("Scale::y",0.0,0),ModelsFile.getValue("Scale::z",0.0,0));
		model_object.setRotationAxisandAngle(ModelsFile.getValue("Rotation::Angle",0.0,0),ModelsFile.getValue("Rotation::Axis::x",0.0,0),ModelsFile.getValue("Rotation::Axis::y",0.0,0),ModelsFile.getValue("Rotation::Axis::z",0.0,0));
		model_object.setPosition(ModelsFile.getValue("Position::x",0.0,0),ModelsFile.getValue("Position::y",0.0,0),ModelsFile.getValue("Position::z",0.0,0));
		model_object.setDescription(ModelsFile.getValue("Description","",0));
		ModelsFile.popTag();
		model_object.loadandSetupModels();

		ModelsList.push_back(model_object);
		cout<<model_object.Position<<"\t\t"<<model_object.RotationAxis;

	}


}
void testApp::drawModelsXML()
{
	for(int i=0;i<ModelsList.size();i++)
		ModelsList[i].drawModel();

}
void testApp::resetModelVariables()
{
	/// Just to reset the touch_selected variable of the models ,to show that nothing selected ..
	for(int i=0;i<ModelsList.size();i++)
		ModelsList[i].touch_selected=false;
}

void testApp::SetupImageMatrices()
{
	cameraWidth= 640;
	cameraHeight= 480;
	
	vidGrabber.setDeviceID(0);
	vidGrabber.initGrabber(cameraWidth, cameraHeight);
	vidGrabber.listDevices(); // Use this to check which camera to use ..
	vidGrabber.setDeviceID(1);
	colorImage.allocate(cameraWidth, cameraHeight);
	grayImage.allocate(cameraWidth, cameraHeight);
	grayThres.allocate(cameraWidth, cameraHeight);

	 
}

void testApp::setupARToolkitStuff()
{
	//artk.setup(cameraWidth, cameraHeight);
	artk.setup(cameraWidth, cameraHeight,"USB_Cam.cal","markerboard_480-499.cfg");
	threshold = 180;
	//threshold=90;
	artk.setThreshold(threshold);
}

void testApp::GrabCameraFrameandConvertMatrices()
{
	vidGrabber.grabFrame();
	bool bNewFrame = vidGrabber.isFrameNew();

	if(bNewFrame) {

		colorImage.setFromPixels(vidGrabber.getPixels(), cameraWidth, cameraHeight);

		// convert our camera image to grayscale
		grayImage = colorImage;
		// apply a threshold so we can see what is going on
		grayThres = grayImage;
		grayThres.threshold(threshold);

		// Pass in the new image pixels to artk
		artk.update(grayImage.getPixels());

	}
}

void testApp::checkifMarkerDetected()
{

	int myIndex = artk.getMarkerIndex(0);
	if(myIndex >= 0) {	
		// Get the corners
		vector<ofPoint> corners;
		artk.getDetectedMarkerBorderCorners(myIndex, corners);
		// Can also get the center like this:
		// ofPoint center = artk.getDetectedMarkerCenter(myIndex);
		ofSetHexColor(0x0000ff);
		for(int i=0;i<corners.size();i++) {
			ofCircle(corners[i].x, corners[i].y, 70);
		}
	}

	

}

void testApp::drawRadar()
{
	// Setup an overhead Camera ...

	radarCam.setPosition(0,0,750);
	radarCam.lookAt(ofVec3f(0,0,0),ofVec3f(0.0,1.0,0));
	//radarCam.roll(g_fYaw);

	ofSetColor(255,255,255);
	
	radarCam.begin();
	
	ofViewport(0,ofGetHeight()-200,200,200);
	//drawModelsXML();
	ofSetColor(255,255,255);
	ofSphere(TargetPosition.x,TargetPosition.y,50);
	radarCam.end();

	// The Circles ...

	ofEnableAlphaBlending();

	ofNoFill();
	
	ofSetColor(255,255,255);
	ofCircle(105,ofGetHeight()-100,100);
	ofCircle(105,ofGetHeight()-100,60);
	ofCircle(105,ofGetHeight()-100,30);
	
	ofPushMatrix();
	
	ofTranslate(105,ofGetHeight()-100);
	ofRotate(-g_fYaw,0,0,1);
	

	ofSetColor(255,255,255,98);
	ofFill();
	ofSetPolyMode(OF_POLY_WINDING_NONZERO);

	ofBeginShape();
		ofVertex(0,0);
		ofVertex(45,-80);
		ofVertex(-45,-80);	
	ofEndShape();
	
	ofLine(0,0,45,-80);
	ofLine(0,0,-45,-80);

	ofPopMatrix();
	
	

	ofSetColor(0,255,0,40);
    ofCircle(105,ofGetHeight()-100,100);
	
	

	ofDisableAlphaBlending();
	
}

void testApp::DrawDescription(string description)
{
	

//ofEnableAlphaBlending();
	ofSetColor(255,0,0);
	
	//ofEnableArbTex();
	glAlphaFunc ( GL_GREATER, 0.5) ;  
	glEnable ( GL_ALPHA_TEST ) ;
	Serif_25.drawString(description,ofGetWidth()-guiWidth+40,45);
	glDisable(GL_ALPHA_TEST);	 

	ofSetColor(255);

	//// Draw the White Background 
	ofRect(ofGetWidth()-guiWidth-10,10,guiWidth,guiHeight);

	ofBeginShape();
	
	ofVertex(ofGetWidth()/2,ofGetHeight()/2);
	ofVertex(ofGetWidth()-guiWidth-10,100);
	ofVertex(ofGetWidth()-guiWidth+50,guiHeight);
	
	ofEndShape();

		//ofDisableAlphaBlending();
	//ofTranslate(0,0,10);

	//ofTranslate(0,0,-10);

	
}

void testApp::setupImgMatrices()
{
	cameraWidth= 640;
	cameraHeight= 480;
	
	vidGrabber.setDeviceID(0);
	
	vidGrabber.initGrabber(cameraWidth, cameraHeight);
	//vidGrabber.listDevices(); // Use this to check which camera to use ..
	vidGrabber.listDevices();cout<<"\n\n Device";
	vidGrabber.videoSettings();
	colorImage.allocate(cameraWidth,cameraHeight);
	
	frame=0;
	
}


void testApp::GetThresholdedImage(IplImage* img,string color,int coordinates[])
{
	// Convert the image into an HSV image
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);

	if(color.compare("blue")==0)
	// Blue
    cvInRangeS(imgHSV, cvScalar(100, 100, 100), cvScalar(125, 255, 255), imgThreshed);

	else if(color.compare("green")==0)
	// Green Color 
	cvInRangeS(imgHSV, cvScalar(45, 100, 100), cvScalar(85, 255, 255), imgThreshed);

	else if(color.compare("red")==0)
	{	// Green Color 
		IplImage *temp1=cvCreateImage(cvGetSize(img), 8, 1);
		IplImage *temp2=cvCreateImage(cvGetSize(img), 8, 1);

		
		cvInRangeS(imgHSV, cvScalar(0, 135, 135), cvScalar(20, 255, 255), temp1);
		cvInRangeS(imgHSV, cvScalar(159, 135, 135), cvScalar(179, 255, 255), temp2);
		cvOr(temp1,temp2, imgThreshed);

		cvReleaseImage(&temp1);
		cvReleaseImage(&temp2);
	}

	else if(color.compare("yellow")==0)
		// Green Color 
		cvInRangeS(imgHSV, cvScalar(18, 100, 100), cvScalar(32, 255, 255), imgThreshed);

	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThreshed, moments, 1);

	// The actual moment values
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);

	cout<<"\n\n Color "<<color<<"Area"<<area<<"\n";
	// Holding the last and current ball positions
	int posX = 0;
	int posY = 0;
	
	posX = moment10/area;
	posY = moment01/area;

	if(area>200)
	{	coordinates[0]=posX;coordinates[1]=posY;}
	else coordinates[0]=coordinates[1]=0;
	// Print it out for debugging purposes
	// 
	// Release the thresholded image+moments... we need no memory leaks.. please
	cvReleaseImage(&imgThreshed);
	//cvReleaseImage(&img);
	cvReleaseImage(&imgHSV);
	delete moments;


}

void testApp::GrabCameraFrameandSetThreshold()
{
	vidGrabber.grabFrame();
	bool bNewFrame = vidGrabber.isFrameNew();

	if(bNewFrame) 
	{
		colorImage.setFromPixels(vidGrabber.getPixels(), cameraWidth, cameraHeight);
		frame=colorImage.getCvImage();

		GetThresholdedImage(frame,"yellow",yellowBlobCoordinates);
		GetThresholdedImage(frame,"red",redBlobCoordinates);
		GetThresholdedImage(frame,"green",greenBlobCoordinates);
		GetThresholdedImage(frame,"blue",blueBlobCoordinates);

	}
}

void testApp::setupCameraforBlobs()
{
	cameraWidth=320;cameraHeight=240;

	vidGrabber.setDeviceID(0);
	vidGrabber.initGrabber(cameraWidth, cameraHeight, true);
	vidGrabber.videoSettings();

	rgb.allocate(cameraWidth, cameraHeight);
	hsb.allocate(cameraWidth, cameraHeight);
	hue.allocate(cameraWidth, cameraHeight);
	sat.allocate(cameraWidth, cameraHeight);
	bri.allocate(cameraWidth, cameraHeight);

	filtered.allocate(cameraWidth, cameraHeight);
	filtered2.allocate(cameraWidth, cameraHeight);


}

void testApp::GrabFrameandFindContours()
{

	vidGrabber.update();	

	if(vidGrabber.isFrameNew())
	{

	

//copy webcam pixels to rgb image
rgb.setFromPixels(vidGrabber.getPixels(), cameraWidth,cameraHeight);

//mirror horizontal
//rgb.mirror(false, true);

//duplicate rgb
hsb = rgb;

//convert to hsb
hsb.convertRgbToHsv();

//store the three channels as grayscale images
hsb.convertToGrayscalePlanarImages(hue, sat, bri);

//filter image based on the hue value were looking for
for (int i=0; i<cameraWidth*cameraHeight; i++) {
	filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],20,30) ? 255 : 0;
			filtered2.getPixels()[i] = ofInRange(hue.getPixels()[i],55,65) ? 255 : 0;
}
filtered.flagImageChanged();
filtered2.flagImageChanged();
//run the contour finder on the filtered image to find blobs with a certain hue
contours.findContours(filtered, 50, cameraWidth*cameraHeight/2, 1, false);
contours2.findContours(filtered2, 50, cameraWidth*cameraHeight/2, 1, false);

}


}

void testApp::RandomizeTargetPosition()
{
	TargetPosition.x=ofRandom(-200,200);
	TargetPosition.y=ofRandom(-200,200);
	TargetPosition.z=ofRandom(-35,35);


}

void testApp::drawTargetandCursor()
{
	
	if(gestureType>0)
		ofSetColor(0,255,0);
	else if(isCursorSelected)
		ofSetColor(0,100,0);
	else ofSetColor(240,86,86);

	ofCircle(CursorPosition.x,CursorPosition.y,50);
	
	if(isTargetVisible)
	{
		ofSetColor(255,255,255);
		//ofCircle(TargetPositionScreenCoods.x,TargetPositionScreenCoods.y,50);
		ofEnableAlphaBlending();
		bullsEye.draw(TargetPositionScreenCoods.x,TargetPositionScreenCoods.y);
		ofDisableAlphaBlending();
	}
}

void testApp::SaveAndRecordTimes()
{
	if(!Timings.loadFile("Timings.xml"))
		Timings.addTag("Timings");

	Timings.pushTag("Timings");

	int numberofSessions=Timings.getNumTags("Session");

	Timings.addTag("Session");
	Timings.pushTag("Session",numberofSessions);

	time_t t = time(0);   
	struct tm * now = localtime( & t );
	string currentDate=ofToString(now->tm_mday)+"-" +ofToString(now->tm_mon + 1)+"-"+ofToString(((now->tm_year)+ 1900));

	Timings.addValue("Id",numberofSessions+1);
	Timings.addValue("Date",currentDate);

	string currentTime=ofToString(now->tm_hour)+"-" +ofToString(now->tm_min)+"-"+ofToString(now->tm_sec);

	cout<<endl<<currentTime<<endl;

	Timings.addValue("Time",currentTime);
	
	Timings.addValue("X Error",ofToString(CursorPosition.x-TargetPosition.x));
	Timings.addValue("Y Error",ofToString(CursorPosition.y-TargetPosition.y));

	/// Duration ..
	Timings.addValue("Duration",ofToString(ofGetElapsedTimeMillis()-startTime));

	/// Gesture Type ...

	/// Accuracy %age ..Cartesian Difference in error as a percentage ..

	Timings.addValue("Accuracy",ofToString(sqrt(pow(CursorPosition.x-TargetPosition.x,2)+pow(CursorPosition.y-TargetPosition.y,2))));
		
	/// 

	

/// Session pop ..

	Timings.popTag();

	//Timings.popTag();

	Timings.saveFile("Timings.xml");
	
}

void testApp::checkifTargetisonScreen()
{
	TargetPositionScreenCoods=cam.worldToScreen(TargetPosition);

	if(TargetPositionScreenCoods.z<1)
		isTargetVisible=true;
	else isTargetVisible=false;

	if(isTargetVisible)
		;//cout<<"ssaawwwwwwwwwww";

	;//cout<<"\n\n\n"<<TargetPosition.x;
}

void testApp::checkifCursorIntersectsFinger()
{
	if(convertedTouchPoints[0]==1)
	{
		;//cout<<"pass";
		if(abs(convertedTouchPoints[1]-CursorPosition.x)<25&&abs(convertedTouchPoints[2]-CursorPosition.y)<25)
           isCursorSelected=true;//cout<<"iT IS TOUCHING ...";
		else isCursorSelected=false;//cout<<"It is not touching ..";
	}

	if(convertedTouchPoints[0]==2)
	{
		float midPointTouch_x=((convertedTouchPoints[1])+(convertedTouchPoints[3]))/2;
		float midPointTouch_y=((convertedTouchPoints[2])+(convertedTouchPoints[4]))/2;
		

		if(abs(midPointTouch_x-CursorPosition.x)<25&&abs(midPointTouch_y-CursorPosition.y)<25)
			{isCursorSelected=true;//cout<<"iT IS TOUCHING ...";
		cout<<"touch"<<endl;
		}
		else isCursorSelected=false;


	}
	
}

void testApp::classifyGesture(string message)
{

	vector<string> components= ofSplitString(message,",");
		
	if(ofToInt(components[0])==1&&components[components.size()-2]=="S")
	{
		gestureType=1;
		//cout<<"\nThe 3D Model is"<<calc.touch_selected;
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==2&&components[components.size()-2]=="HC")
	{
		gestureType=2;
		//cout<<"\n Head Crusher";
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==2&&components[components.size()-2]=="SF"&&calc.touch_selected!=0)
	{
		gestureType=3;
		cout<<"\n Scaling Gesture";
		time_begin_crossDimSelection=10000;
	}
	if(ofToInt(components[0])==1&&components[components.size()-1]=="CD"&&calc.touch_selected!=0)
	{
		gestureType=4;
		//cout<<"\n Cross Dimensional";
		time_begin_crossDimSelection=ofGetSystemTime();
		crossDimObjSelected=calc.touch_selected;
	}

	if(ofToInt(components[0])==1&&components[components.size()-2]=="TP")
	{
		gestureType=5;
		time_begin_crossDimSelection=10000;
		cout<<"Tap";
	}
	


	
	cout<<components[components.size()-2]<<"\t\t"<<gestureType;
	////cout<<"g-Fyaw"<<g_fYaw<<endl;
	components.clear();
} 

void testApp::translateCursor(vector<string> message)
{
	
	if((gestureType==1||gestureType==5)&&ofToFloat(message[1])>0&&ofToFloat(message[2])>0&&(activeGesture==1||activeGesture==3)) // NEEDS TO BE CHANGED ..
		{

		if(!beginSelection)
		{	
			last_single_touch[0]=(ofToFloat(message[1]));
			last_single_touch[1]=(ofToFloat(message[2]));
			difference[0]=CursorPosition.x-last_single_touch[0];
			difference[1]=CursorPosition.y-last_single_touch[1];
			beginSelection=!beginSelection;
			CursorPosition.x=convertedTouchPoints[1];
			CursorPosition.y=convertedTouchPoints[2];
		}
		else
		{

			CursorPosition.x=convertedTouchPoints[1];
			CursorPosition.y=convertedTouchPoints[2];
		

		cout<<"Translating";
		cout<<(ofToFloat(message[1])-last_single_touch[0])<<"     "<<(ofToFloat(message[2])-last_single_touch[1]);
		}
	}
	
	else if(gestureType==2&&ofToFloat(message[1])>0&&ofToFloat(message[2])>0&&activeGesture==2)
	{

		CursorPosition.x=(abs(convertedTouchPoints[1])+abs(convertedTouchPoints[3]))/2;
		CursorPosition.y=(abs(convertedTouchPoints[2])+abs(convertedTouchPoints[4]))/2;
	}

	else gestureType=0;	

}



