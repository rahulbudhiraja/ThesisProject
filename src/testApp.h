////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file src\testApp.h
///
/// \brief Declares the test application class.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _TEST_APP

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \def _TEST_APP
///
/// \brief A macro that defines test application.
///
/// \author Rahul
/// \date 9/21/2012
////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \class testApp
///
/// \brief Test application.
///
/// \author Rahul
/// \date 9/21/2012
////////////////////////////////////////////////////////////////////////////////////////////////////

class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::mousePressed(int x, int y, int button);
	///
	/// \brief Mouse pressed.
	///
	/// \author Rahul
	/// \date 9/21/2012
	///
	/// \param x	  The x coordinate.
	/// \param y	  The y coordinate.
	/// \param button The button.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::UpdateTracking();
	///
	/// \brief Updates the tracking.
	///
	/// \author Rahul
	/// \date 9/21/2012
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateTracking();
	void setupTracker();
	void setupModels();
	void drawModels();

	// UDP Stuff ..

	/// \brief The UDP connection.
	ofxUDPManager udpConnection;

	/// \brief The UDP send connection.
	ofxUDPManager udpSendConnection;
	/// \brief The UDP receive connection.
	ofxUDPManager udpReceiveConnection;

	/// \brief The mono.
	ofTrueTypeFont  mono;
	/// \brief The monosm.
	ofTrueTypeFont  monosm;

	/**
	 * \defgroup IntVariables Global integer variables
	 * @{
	 */
	
	


	///@{
	
	///	\name The AssimpModelLoader Variables ..
	///		  These Variables will load all the Models that were used for the BlackBayAR demo .
	///	\brief BlackBay Models ..	  
	
	ofxAssimpModelLoader bb_model,cr_model,lr_model,ls_model,pgn_model,pgy_model,pog_model,rc_model,sr_model,tbox_model;
	
	ofxAssimpModelLoader squirrelModel;

	///@}

	void drawAxes();
	void drawPlane();
	void UDPReceive();

	void addObjecttoScene(string);

	ofColor Returncolor(string);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::drawAugmentedPlane(float ,float,ofColor,ofColor,int ,string,string);
	///
	/// \brief Takes information from Handheld Phone and draws augmented plane in the AR view.
	///
	/// \author Rahul
	/// \date 9/28/2012
	///
	/// \param xPosition    The x-position.
	/// \param yPosition    The y-position.
	/// \param textColor    The color of the text written on the plane.
	/// \param bgColor	    The background color of the plane.
	/// \param i		    Index,Which Plane is being changed.
	/// \param note_heading The note heading.
	/// \param note_text    The note text.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void drawAugmentedPlane(float ,float,ofColor,ofColor,int ,string,string);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::drawTouches(string udpMessage);
	///
	/// \brief If the Handheld is being used a controller,This function will take the touch information and draw it on the AR view.
	///
	/// \author Rahul
	/// \date 9/28/2012
	///
	/// \param udpMessage The Message received though UDP ..
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void drawTouches(string udpMessage);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \property int windowWidth,windowHeight
	///
	/// \brief Gets the height of the window.
	///
	/// \brief The height of the window.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	int windowWidth,windowHeight;

	int gestureType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::translate_3D_Model(string);
	///
	/// \brief This function will translate the 3D Model depending on users finger movement on the touch screen ...
	///
	/// \author Rahul
	/// \date 10/6/2012
	///
	/// \param string The UDP string that was Received ..
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void translate_3D_Model(string);

	float Bbaymodel_origin[3],Destination_origin[3],Friend_model_origin[3],Reciever_model_origin[3];
	float Bbaymodel_translate[3],Destination_translate[3],Friend_model_translate[3],Reciever_model_translate[3];
	
	float Bbaymodel_rotation,Destination_rotation,Friend_rotation,Receiver_rotation;
	ofVec3f Bbaymodel_rotAxis,Destination_rotAxis,Friend_rotAxis,Receiver_rotAxis;
	ofVec3f Bbaymodel_UpdatedSceneMax,Bbaymodel_UpdatedSceneMin,Destination_UpdatedSceneMax,Destination_UpdatedSceneMin,Friend_UpdatedSceneMax,Friend_UpdatedSceneMin,Receiver_UpdatedSceneMax,Receiver_UpdatedSceneMin;
	


	void updateScreenMaxandMin();


	void set_model_initial_position();
	void setupFonts();
	void setupUDPConnections();
	void setupCrosshair();
	void updateModelPositions();

	bool beginSelection;

	void drawCrosshair();
	string Receive_Message();
	void drawTouchImpressions(vector <string>message);
    void translateModel(vector <string>);
	void storeFingerPosition(vector<string>);
	void Check_if_Finger_Intersects_3DModel(vector <string>);
	
	float scalexPosition,scaleyPosition,scaleFactor;
	
	vector <float> convertedTouchPoints;
	
	int Check_for_Finger_Intersections();
	bool intersect_model(ofVec3f ,ofVec3f ,vector <float>);

	void convertPhonetoScreenCoordinates(string rawTouchPoints);


	void temp_calculateMaxandMin();

};

#endif
