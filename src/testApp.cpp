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



// Provide for a filtering mechanism on the VR920 tracker reading. 

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \fn IWRFilterTracking( long *yaw, long *pitch, long *roll );
///
/// \brief The Filtered tracking output
///
/// \author Rahul
/// \date 9/21/2012
///
/// \param [in,out] yaw    the yaw.
/// \param [in,out] pitch  the pitch.
/// \param [in,out] roll   the roll.
////////////////////////////////////////////////////////////////////////////////////////////////////

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

//GUIStuff gui;
ofCamera cam;

/// -- Processing ...

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

void testApp::setup()
{
	gestureType=0;

	setupTracker();
	setupModels();
	set_model_initial_position();
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

	/// This function sets the initial position of the models 
	set_model_initial_position();

	UpdateTracking();
	initialyaw=g_fYaw;
	last_time_select=0;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw()
{

	windowWidth=ofGetWidth();
	windowHeight=ofGetHeight();

	ofBackground(0,0,0);
	UpdateTracking();

	if(g_fPitch>-25)
	{
		if(dirn.compare("down")==0)
		{
			int sent = udpSendConnection.Send("Change",6);//// Send a Change message and change to dirn.compare==up;
			dirn="up";
		}

		cam.setPosition(0,0,10);
		cam.lookAt(ofVec3f(0,100,10),ofVec3f(0,0,1.0)); // have changed this a bit ..
		

		cam.pan(g_fYaw);
		cam.tilt(g_fPitch);
		cam.begin();

		drawAxes();
		drawPlane();
		drawModels();

		ofSetColor(255, 255, 255);
		ofFill();

		for(i=0;i<scenes.size();i++)
		{

			ofPushMatrix();
			ofTranslate(scenes[i].x,scenes[i].y);
			drawAugmentedPlane(scenes[i].x,scenes[i].y,Notes[i].text_color,Notes[i].bg_color,i,Notes[i].note_heading,Notes[i].note_text);
			ofPopMatrix();
		}


		cam.end();


		string message=Receive_Message();
		//cout<<ofGetSystemTime()<<endl;

		if(gestureType!=0)
		{
			    translate_3D_Model(message);
				//updateModelPositions();
				//cout<<"With Sticky"<<ofGetSystemTime()<<"\n\n";
			
		}

		else {	
		
	        beginSelection=false;

			updateModelPositions();

			UDPReceive();

				//cout<<"Without Sticky"<<ofGetSystemTime()<<"\n\n";

			calc.check_intersection(calc.convertDegreestoRadians(g_fYaw),calc.convertDegreestoRadians(g_fPitch),scenes);

			if(message.length()>0)
			{
				drawTouches(message);
			}

			else calc.touch_selected=0;

		}

	}

	else if(dirn.compare("up")==0)
	{
		int sent = udpSendConnection.Send("Change",6);;//// Send a Change message and change to dirn.compare==up;
		dirn="down";
	}


	drawCrosshair();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

		yaw++;
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
	Destination_origin[2]=1;
	cout<<Destination_origin[2]<<endl;

}


void testApp::drawModels()
{
	//ofNoFill();

	ofSetColor(255,255,255);

	if(calc.touch_selected==1)
    ofSetColor(0,0,255);

	ofPushMatrix();
	ofTranslate(Bbaymodel_origin[0]+Bbaymodel_translate[0],Bbaymodel_origin[1]+Bbaymodel_translate[1],Bbaymodel_origin[2]+Bbaymodel_translate[2]);
	ofRotateZ(rotAngle);

	bb_model.drawFaces();

	ofPopMatrix();

	ofPushMatrix();
	
	ofSetColor(255,255,255);

	ofTranslate(Friend_model_origin[0]+Friend_model_translate[0],Friend_model_origin[1]+Friend_model_translate[1],Friend_model_origin[2]+Friend_model_translate[2]);

	ofRotateZ(rotAngle);

	if(calc.touch_selected==3)
    cr_model.drawWireframe();
	
	else cr_model.drawFaces();
	//cr_model.drawVertices();

	ofPopMatrix();

	ofPushMatrix();

	ofSetColor(255,255,255);

	ofTranslate(Destination_origin[0]+Destination_translate[0],Destination_origin[1]+Destination_translate[1],Destination_origin[2]+Destination_translate[2]);
	cout<<Destination_origin[2]<<endl;

	ofRotateZ(rotAngle);

	if(calc.touch_selected==2)
		lr_model.drawWireframe();
	else lr_model.drawFaces();

	ofPopMatrix();

	ofPushMatrix();

	
	ofSetColor(255,255,255);

	
	ofTranslate(Reciever_model_origin[0]+Reciever_model_translate[0],Reciever_model_origin[1]+Reciever_model_translate[1],Reciever_model_origin[2]+Reciever_model_translate[2]);

	
	ofRotateZ(rotAngle);

	if(calc.touch_selected==4)
		sr_model.drawWireframe();

	else sr_model.drawFaces();

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

		/// \brief .
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
	
	ofSetColor(255,0,0,90);

	storeFingerPosition(components);
	ofFill();

	ofEnableAlphaBlending();

	Check_if_Finger_Intersects_3DModel(components);

	for(int i=1;i<=2*ofToInt(components[0]);i+=2)
	{ 		 
		ofCircle(windowWidth*ofToFloat(components[i])/1280,windowHeight*ofToFloat(components[i+1])/720,0,20);
		
		if(ofToInt(components[0])==1)
		calc.check_touch_intersection(ofVec2f(windowWidth*ofToFloat(components[i])/1280-windowWidth/2,windowHeight*ofToFloat(components[i+1])/720-windowHeight/2),windowWidth,windowHeight,calc.convertDegreestoRadians(g_fYaw));
		else if (ofToInt(components[0])==2) 
		{
			midPoint_x=(ofToFloat(components[1])+ofToFloat(components[3]))/2;
			midPoint_y=(ofToFloat(components[2])+ofToFloat(components[4]))/2;

			calc.check_touch_intersection(ofVec2f(windowWidth*midPoint_x/1280-windowWidth/2,windowHeight*midPoint_y/720-windowHeight/2),windowWidth,windowHeight,calc.convertDegreestoRadians(g_fYaw));
		}

	}

	//cout<<"           "<<components[components.size()-2]<<endl;

	if(ofToInt(components[0])==1&&components[components.size()-2]=="S"&&calc.touch_selected!=0)
	{
		gestureType=1;
		cout<<"\nThe 3D Model is"<<calc.touch_selected;
	}

	if(ofToInt(components[0])==2&&components[components.size()-2]=="HC"&&calc.touch_selected!=0)
	{
		gestureType=2;
		cout<<"\n Head Crusher";
	}

	if(ofToInt(components[0])==2&&components[components.size()-2]=="SF"&&calc.touch_selected!=0)
	{
		gestureType=3;
		cout<<"\n Scaling Gesture";
		//scalexPosition=

	}

	
	//cout<<"g-Fyaw"<<g_fYaw<<endl;

	components.clear();
	ofDisableAlphaBlending();

	ofSetColor(255);


}

void testApp::translate_3D_Model(string message)
{
	result=ofSplitString(ofToString(message),",");

	drawTouchImpressions(result);
	translateModel(result);
	

//	if(ofToInt(result[0])==1&&gestureType==1)
//	{
//	
//	//find which model is selected	;
//	if(!beginSelection)
//	{	
//		last_single_touch[0]=(ofToFloat(result[1]));
//		last_single_touch[1]=(ofToFloat(result[2]));
//
//		beginSelection=!beginSelection;
//	}
//		
//	if(calc.touch_selected==1)
//	{
//		//cout<<"BBay is selected";
//		Bbaymodel_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Bbaymodel_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	
//
//		//ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*ofToFloat(result[1])/1280, windowHeight*ofToFloat(result[2])/720,  0.975f ) );  
//		//cout<<sw<<endl;
//
//		//Bbaymodel_origin[0]=sw.x;
//		//Bbaymodel_origin[1]=sw.y;
//
//		////Bbaymodel_origin[2]=sw.z;
//
//		//	calc.updateBBayOrigin(Bbaymodel_origin[0],Bbaymodel_origin[1],10);
//
//		//Bbaymodel_translate[2]=sw.z;
//
//		//cout<<"BlackBay Model"<<Bbaymodel_translate[0]<<"      "<<Bbaymodel_translate[1]<<"\n\n";
//		//cout<<"BlackBay Model Origin"<<Bbaymodel_origin[0]<<"      "<<Bbaymodel_origin[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==2)
//	{
//		cout<<"Destination is selected";
//		Destination_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Destination_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	
//		//cout<<"Destination Model"<<Destination_translate[0]<<"      "<<Destination_translate[1]<<"\n\n";
//		//cout<<"Destination Model Origin"<<Destination_origin[0]<<"      "<<Destination_origin[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==3)
//	{
//		//cout<<"Destination is selected";
//		Friend_model_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Friend_model_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	
////		cout<<"Friend Model"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
//	//	cout<<"Friend Model Origin"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==4)
//	{
//		//cout<<"Receiver is selected";
//		Reciever_model_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Reciever_model_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	
//	/*	cout<<"Reciever Model"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";
//		cout<<"Reciever Model Origin"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";*/
//	}
//	
//	}
//
//	else  if(ofToInt(result[0])==2&&gestureType==2)
//	{
//
//		//find which model is selected	;
//	if(!beginSelection)
//	{	
//		
//		last_single_touch[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		last_single_touch[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//
//		beginSelection=!beginSelection;
//	}
//		
//	if(calc.touch_selected==1)
//	{
//		//cout<<"BBay is selected";
//		/*Bbaymodel_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Bbaymodel_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);*/
//	
//		Bbaymodel_origin[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		Bbaymodel_origin[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//		
//		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Bbaymodel_origin[0]/1280, windowHeight*Bbaymodel_origin[1]/720,  0.975f ) );  
//		cout<<sw<<endl;
//
//		Bbaymodel_origin[0]=sw.x;
//		Bbaymodel_origin[1]=sw.y;
//		Bbaymodel_origin[2]=10;
//
//		calc.updateBBayOrigin(Bbaymodel_origin[0],Bbaymodel_origin[1],10);
//
//		//cout<<"BlackBay Model"<<Bbaymodel_translate[0]<<"      "<<Bbaymodel_translate[1]<<"\n\n";
//		//cout<<"BlackBay Model Origin"<<Bbaymodel_origin[0]<<"      "<<Bbaymodel_origin[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==2)
//	{
//		cout<<"Destination is selected";
//		Destination_origin[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		Destination_origin[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//		
//
//		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Destination_origin[0]/1280, windowHeight*Destination_origin[1]/720,  0.975f ) );  
//		cout<<sw<<endl;
//
//		Destination_origin[0]=sw.x;
//		Destination_origin[1]=sw.y;
//		Destination_origin[2]=1;
//
//		calc.updateDestinationOrigin(Destination_origin[0],Destination_origin[1],1);
//
//
//		//cout<<"Destination Model"<<Destination_translate[0]<<"      "<<Destination_translate[1]<<"\n\n";
//		//cout<<"Destination Model Origin"<<Destination_origin[0]<<"      "<<Destination_origin[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==3)
//	{
//		//cout<<"Destination is selected";
//		//Friend_model_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		//Friend_model_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	
//		Friend_model_origin[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		Friend_model_origin[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//		
//		
//		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Friend_model_origin[0]/1280, windowHeight*Friend_model_origin[1]/720,  0.975f ) );  
//		cout<<sw<<endl;
//
//		Friend_model_origin[0]=sw.x;
//		Friend_model_origin[1]=sw.y;
//		Friend_model_origin[2]=1;
//
//		calc.updateDestinationOrigin(Friend_model_origin[0],Friend_model_origin[1],1);
//
//
////		cout<<"Friend Model"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
//	//	cout<<"Friend Model Origin"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
//	}
//
//	else if(calc.touch_selected==4)
//	{
//		//cout<<"Receiver is selected";
//		/*Reciever_model_translate[0]=(ofToFloat(result[1])-last_single_touch[0]);
//		Reciever_model_translate[1]=(ofToFloat(result[2])-last_single_touch[1]);
//	*/
//	/*	Reciever_model_translate[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		Reciever_model_translate[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//	*/
//
//		Reciever_model_origin[0]=(ofToFloat(result[1])+ofToFloat(result[3]))/2;
//		Reciever_model_origin[1]=(ofToFloat(result[2])+ofToFloat(result[4]))/2;
//		
//		
//		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Reciever_model_origin[0]/1280, windowHeight*Reciever_model_origin[1]/720,  0.975f ) );  
//		cout<<sw<<endl;
//
//		Reciever_model_origin[0]=sw.x;
//		Reciever_model_origin[1]=sw.y;
//		Reciever_model_origin[2]=1;
//
//		calc.updateDestinationOrigin(Reciever_model_origin[0],Reciever_model_origin[1],1);
//
//
//
//	/*	cout<<"Reciever Model"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";
//		cout<<"Reciever Model Origin"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";*/
//	}
//
//
//	}
//
//	else gestureType=0;
//
}

void testApp::set_model_initial_position()
{
	Bbaymodel_origin[0]=116.747;
	Bbaymodel_origin[1]=-163.28;
	Bbaymodel_origin[2]=1;


	Destination_origin[0]=-250;
	Destination_origin[1]=-300;
	Destination_origin[2]=1;

	///< //////////////.
	Friend_model_origin[0]=280.72705;
	Friend_model_origin[1]=244.184;
	Friend_model_origin[2]=1;

	Reciever_model_origin[0]=-100.23;
	Reciever_model_origin[1]=129.49;
	Reciever_model_origin[2]=1;

	Bbaymodel_translate[0]=Bbaymodel_translate[1]=Bbaymodel_translate[2]=0;
	Destination_translate[0]=Destination_translate[1]=Destination_translate[2]=0;
}

void testApp::setupFonts()
{
	Serif_10.loadFont("Serif.ttf",10);
	Serif_15.loadFont("Serif.ttf",15);

	//load our type
	mono.loadFont("type/mono.ttf", 9);
	monosm.loadFont("type/mono.ttf", 8);

}
void testApp::setupUDPConnections()
{

	udpConnection.Create();
	udpConnection.Bind(12001);
	udpConnection.SetNonBlocking(true);

	udpSendConnection.Create();
	udpSendConnection.Connect("192.168.43.1",13001);
	udpSendConnection.SetNonBlocking(true);

	udpReceiveConnection.Create();
	udpReceiveConnection.Bind(12003);
	udpReceiveConnection.SetNonBlocking(true);
}

void testApp::setupCrosshair()
{
	crosshair.loadImage("crosshair.png");
	crosshair.saveImage("asdasdsa.png");
	crosshair.resize(crosshair.width*1.5,crosshair.height*1.5);
}

void testApp::updateModelPositions()
{
	if(Bbaymodel_translate[0]||Bbaymodel_translate[1])
	{
			Bbaymodel_origin[0]+=Bbaymodel_translate[0];
	        Bbaymodel_translate[0]=0;
			Bbaymodel_origin[1]+=Bbaymodel_translate[1];
	        Bbaymodel_translate[1]=0;
	
			calc.updateBBayOrigin(Bbaymodel_origin[0],Bbaymodel_origin[1],Bbaymodel_origin[2]);
	}
	
	if(Destination_translate[0]||Destination_translate[1])
	{
			Destination_origin[0]+=Destination_translate[0];
			Destination_translate[0]=0;	
			Destination_origin[1]+=Destination_translate[1];
			Destination_translate[1]=0;

			calc.updateDestinationOrigin(Destination_origin[0],Destination_origin[1],Destination_origin[2]);
	}
	
	if(Friend_model_translate[0]||Friend_model_translate[1])
	{
			Friend_model_origin[0]+=Friend_model_translate[0];
			Friend_model_translate[0]=0;

			Friend_model_origin[1]+=Friend_model_translate[1];
			Friend_model_translate[1]=0;

			calc.updateFriendModelOrigin(Friend_model_origin[0],Friend_model_origin[1],Friend_model_origin[2]);
	}
	
	if(Reciever_model_translate[0]||Reciever_model_translate[1])
	{
			Reciever_model_origin[0]+=Reciever_model_translate[0];
			Reciever_model_translate[0]=0;

			Reciever_model_origin[1]+=Reciever_model_translate[1];
			Reciever_model_translate[1]=0;

			calc.updateReceiverModelOrigin(Reciever_model_origin[0],Reciever_model_origin[1],Reciever_model_origin[2]);
	}
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

void testApp::drawTouchImpressions(vector <string>message)
{
	
	if(ofToInt(message[0])==1)
	{
		ofFill();
		ofSetColor(0,255,0,90);

		ofEnableAlphaBlending();
		ofCircle(ofGetWidth()*ofToFloat(message[1])/1280,ofGetHeight()*ofToFloat(message[2])/720,0,40);
	
		ofDisableAlphaBlending();
	}

	else if(ofToInt(message[0])==2)
	{
		ofFill();
		ofSetColor(0,255,0,90);

		ofEnableAlphaBlending();
		ofCircle(ofGetWidth()*ofToFloat(message[1])/1280,ofGetHeight()*ofToFloat(message[2])/720,0,40);
		ofCircle(ofGetWidth()*ofToFloat(message[3])/1280,ofGetHeight()*ofToFloat(message[4])/720,0,40);
	
		ofDisableAlphaBlending();
	}
}


void testApp::translateModel(vector <string>message)
{
	if(ofToInt(message[0])==1&&gestureType==1)
	{
	
	//find which model is selected	;
	if(!beginSelection)
	{	
		last_single_touch[0]=(ofToFloat(message[1]));
		last_single_touch[1]=(ofToFloat(message[2]));

		beginSelection=!beginSelection;
	}
		
	if(calc.touch_selected==1)
	{
		//cout<<"BBay is selected";
		Bbaymodel_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Bbaymodel_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	

		//ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*ofToFloat(result[1])/1280, windowHeight*ofToFloat(result[2])/720,  0.975f ) );  
		//cout<<sw<<endl;

		//Bbaymodel_origin[0]=sw.x;
		//Bbaymodel_origin[1]=sw.y;

		////Bbaymodel_origin[2]=sw.z;

		//	calc.updateBBayOrigin(Bbaymodel_origin[0],Bbaymodel_origin[1],10);

		//Bbaymodel_translate[2]=sw.z;

		//cout<<"BlackBay Model"<<Bbaymodel_translate[0]<<"      "<<Bbaymodel_translate[1]<<"\n\n";
		//cout<<"BlackBay Model Origin"<<Bbaymodel_origin[0]<<"      "<<Bbaymodel_origin[1]<<"\n\n";
	}

	else if(calc.touch_selected==2)
	{
		cout<<"Destination is selected";
		Destination_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Destination_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	
		//cout<<"Destination Model"<<Destination_translate[0]<<"      "<<Destination_translate[1]<<"\n\n";
		//cout<<"Destination Model Origin"<<Destination_origin[0]<<"      "<<Destination_origin[1]<<"\n\n";
	}

	else if(calc.touch_selected==3)
	{
		//cout<<"Destination is selected";
		Friend_model_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Friend_model_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	
//		cout<<"Friend Model"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
	//	cout<<"Friend Model Origin"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
	}

	else if(calc.touch_selected==4)
	{
		//cout<<"Receiver is selected";
		Reciever_model_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Reciever_model_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	
	/*	cout<<"Reciever Model"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";
		cout<<"Reciever Model Origin"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";*/
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
		
	if(calc.touch_selected==1)
	{
		//cout<<"BBay is selected";
		/*Bbaymodel_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Bbaymodel_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);*/
	
		Bbaymodel_origin[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
		Bbaymodel_origin[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
		
		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Bbaymodel_origin[0]/1280, windowHeight*Bbaymodel_origin[1]/720,  0.975f ) );  
		cout<<sw<<endl;

		Bbaymodel_origin[0]=sw.x;
		Bbaymodel_origin[1]=sw.y;
		Bbaymodel_origin[2]=10;

		calc.updateBBayOrigin(Bbaymodel_origin[0],Bbaymodel_origin[1],10);

		//cout<<"BlackBay Model"<<Bbaymodel_translate[0]<<"      "<<Bbaymodel_translate[1]<<"\n\n";
		//cout<<"BlackBay Model Origin"<<Bbaymodel_origin[0]<<"      "<<Bbaymodel_origin[1]<<"\n\n";
	}

	else if(calc.touch_selected==2)
	{
		cout<<"Destination is selected";
		Destination_origin[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
		Destination_origin[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
		

		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Destination_origin[0]/1280, windowHeight*Destination_origin[1]/720,  0.975f ) );  
		cout<<sw<<endl;

		Destination_origin[0]=sw.x;
		Destination_origin[1]=sw.y;
		Destination_origin[2]=1;

		calc.updateDestinationOrigin(Destination_origin[0],Destination_origin[1],1);


		//cout<<"Destination Model"<<Destination_translate[0]<<"      "<<Destination_translate[1]<<"\n\n";
		//cout<<"Destination Model Origin"<<Destination_origin[0]<<"      "<<Destination_origin[1]<<"\n\n";
	}

	else if(calc.touch_selected==3)
	{
		//cout<<"Destination is selected";
		//Friend_model_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		//Friend_model_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	
		Friend_model_origin[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
		Friend_model_origin[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
		
		
		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Friend_model_origin[0]/1280, windowHeight*Friend_model_origin[1]/720,  0.975f ) );  
		cout<<sw<<endl;

		Friend_model_origin[0]=sw.x;
		Friend_model_origin[1]=sw.y;
		Friend_model_origin[2]=1;

		calc.updateDestinationOrigin(Friend_model_origin[0],Friend_model_origin[1],1);


//		cout<<"Friend Model"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
	//	cout<<"Friend Model Origin"<<Friend_model_translate[0]<<"      "<<Friend_model_translate[1]<<"\n\n";
	}

	else if(calc.touch_selected==4)
	{
		//cout<<"Receiver is selected";
		/*Reciever_model_translate[0]=(ofToFloat(message[1])-last_single_touch[0]);
		Reciever_model_translate[1]=(ofToFloat(message[2])-last_single_touch[1]);
	*/
	/*	Reciever_model_translate[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
		Reciever_model_translate[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
	*/

		Reciever_model_origin[0]=(ofToFloat(message[1])+ofToFloat(message[3]))/2;
		Reciever_model_origin[1]=(ofToFloat(message[2])+ofToFloat(message[4]))/2;
		
		
		ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*Reciever_model_origin[0]/1280, windowHeight*Reciever_model_origin[1]/720,  0.975f ) );  
		cout<<sw<<endl;

		Reciever_model_origin[0]=sw.x;
		Reciever_model_origin[1]=sw.y;
		Reciever_model_origin[2]=1;

		calc.updateDestinationOrigin(Reciever_model_origin[0],Reciever_model_origin[1],1);



	/*	cout<<"Reciever Model"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";
		cout<<"Reciever Model Origin"<<Reciever_model_translate[0]<<"      "<<Reciever_model_translate[1]<<"\n\n";*/
	}


	}

	else gestureType=0;

}

void testApp::storeFingerPosition(vector<string> components)
{
	/* If its one finger */
	 
	if(ofToFloat(components[0])==1)
	{
		last_single_touch[1]=ofToFloat(components[1]);
		last_single_touch[2]=ofToFloat(components[2]);
     	ofVec3f sw = cam.screenToWorld( ofVec3f( windowWidth*ofToFloat(components[1])/1280, windowHeight*ofToFloat(components[2])/720,  0.965f ) );  
		cout<<sw<<endl;
	}

	/* If its 2 fingers */ 

	else if(ofToFloat(components[0])==2)
	{
		last_single_touch[1]=(ofToFloat(components[1])+ofToFloat(components[3]))/2;
		last_single_touch[2]=(ofToFloat(components[2])+ofToFloat(components[4]))/2;
	}

}


void testApp::Check_if_Finger_Intersects_3DModel(vector <string>components)
{
	for(int i=1;i<=2*ofToInt(components[0]);i+=2)
	{ 		 
		ofCircle(ofGetWidth()*ofToFloat(components[i])/1280,ofGetHeight()*ofToFloat(components[i+1])/720,0,20);
		
		if(ofToInt(components[0])==1)
		calc.check_touch_intersection(ofVec2f(ofGetWidth()*ofToFloat(components[i])/1280-ofGetWidth()/2,ofGetHeight()*ofToFloat(components[i+1])/720-ofGetHeight()/2),ofGetWidth(),ofGetHeight(),calc.convertDegreestoRadians(g_fYaw));
		else if (ofToInt(components[0])==2) 
		{
			midPoint_x=(ofToFloat(components[1])+ofToFloat(components[3]))/2;
			midPoint_y=(ofToFloat(components[2])+ofToFloat(components[4]))/2;

			calc.check_touch_intersection(ofVec2f(ofGetWidth()*midPoint_x/1280-ofGetWidth()/2,ofGetHeight()*midPoint_y/720-ofGetHeight()/2),ofGetWidth(),ofGetHeight(),calc.convertDegreestoRadians(g_fYaw));
		}

	}
}