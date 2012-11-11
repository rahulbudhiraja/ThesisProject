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
	AndroidPhoneResWidth=1280;AndroidPhoneResHeight=720; /// 

	SetupImageMatrices();
	setupARToolkitStuff();
 
	iphone5Model.loadModel("iphone/iPhone5.dae",false);
	iphone5Model.setScaleNomalization(false);
	iphone5Model.setScale(0.03,0.03,0.03);
	iphone5Model.setRotation(0,90,0,0,1);

}
//--------------------------------------------------------------
void testApp::update(){
GrabCameraFrameandConvertMatrices();
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
		//drawModels();
		drawModelsXML();
		ofSetColor(255, 255, 255);
		ofFill();
		for(i=0;i<scenes.size();i++)
		{
			ofPushMatrix();
			ofTranslate(scenes[i].x,scenes[i].y);
			drawAugmentedPlane(scenes[i].x,scenes[i].y,Notes[i].text_color,Notes[i].bg_color,i,Notes[i].note_heading,Notes[i].note_text);
			ofPopMatrix();
		}

		iphone5Model.drawFaces();
		if(artk.getNumDetectedMarkers()!=0)
		{
			iphone5Model.setPosition(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()-artk.getCameraPosition(0).y/10);
			//ofBox(ofVec3f(cam.getX()-artk.getCameraPosition(0).x/10,cam.getY()+artk.getCameraPosition(0).z/10,cam.getZ()+artk.getCameraPosition(0).y/10),20);
					
			//cout<<"\n\n Marker Detected"<<artk.getNumDetectedMarkers()<<"     "<<ofGetSystemTime();
			string CameraPosition="CameraPos,"+ofToString(-artk.getCameraPosition(0).x/10)+","+ ofToString(artk.getCameraPosition(0).z/10)+","+ofToString(-artk.getCameraPosition(0).y/10+10)+",end";
			int sent = udpSendCameraPosition.Send(CameraPosition.c_str(),CameraPosition.length());
			cout<<"\n\n Camera Position  "<<artk.getCameraPosition(0).x/10 <<"\t\t"<<artk.getCameraPosition(0).y/10<<"\t\t"<<artk.getCameraPosition(0).z/10;
		}
		cam.end();

		string message=Receive_Message();

		if(message.length()>0)
			;//cout<<"\n\n UDP received Message"<<message;
		if(message.compare("Left Swipe")==0&&gestureType==4&&(ofGetSystemTime()-time_begin_crossDimSelection)<2000)
		{
			//cout<<"Left Swipe";
			string sendMessage="sAct,"+ofToString(crossDimObjSelected);
			cout<<sendMessage<<"\n\n";
			int sent = udpSendConnection.Send(sendMessage.c_str(),6);
		}
		else if(message.length()>0)
			convertPhonetoScreenCoordinates(message);
		if(gestureType==0&&last_gesture_selected&&convertedTouchPoints[0]==1)
		{
			gestureType=1;
			last_gesture_selected=!last_gesture_selected;
		}
		else if(gestureType>=1&&gestureType<4)
			translate_3D_Model(message);
		else {	
			beginSelection=false;
			calc.touch_selected=0;
			resetModelVariables();
			updateModelPositions();
			UDPReceive();
			Check_for_crosshair_model_intersection();
			if(message.length()>0&&message.compare("Left Swipe")!=0)
				drawTouches(message);

			else calc.touch_selected=0;
			if(crosshair_selected)
				DrawDescription(ModelsList[crosshair_selected].getDescription());
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

	drawRadar();


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
	ofSetColor(255,0,0,90);
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
	udpSendConnection.Connect("192.168.43.1",13001);
	udpSendConnection.SetNonBlocking(true);
	udpReceiveConnection.Create();
	udpReceiveConnection.Bind(12003);
	udpReceiveConnection.SetNonBlocking(true);

	udpSendCameraPosition.Create();
	udpSendCameraPosition.Connect("192.168.43.1",12005);
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
		ofEnableAlphaBlending();

		if(somethingSelected)
		{
			ofSetColor(0,255,0,90);
			ofCircle(ofGetWidth()*ofToFloat(message[1])/AndroidPhoneResWidth,ofGetHeight()*ofToFloat(message[2])/AndroidPhoneResHeight,0,40);
		}
		else 
		{
			ofSetColor(255,0,0,90);
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
			ofSetColor(255,0,0,90);
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

	vidGrabber.initGrabber(cameraWidth, cameraHeight);

	colorImage.allocate(cameraWidth, cameraHeight);
	grayImage.allocate(cameraWidth, cameraHeight);
	grayThres.allocate(cameraWidth, cameraHeight);

	 
}

void testApp::setupARToolkitStuff()
{
	//artk.setup(cameraWidth, cameraHeight);
	artk.setup(cameraWidth, cameraHeight,"RahulMacbookPro.cal","markerboard_480-499.cfg");
	threshold = 90;
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
	drawModelsXML();
	
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
	ofRotate(g_fYaw,0,0,1);
	

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
	Serif_25.drawString(description,ofGetWidth()-guiWidth+70,45);
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