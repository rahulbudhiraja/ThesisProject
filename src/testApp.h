////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file src\testApp.h
///
/// \brief Declares the test application class.
////////////////////////////////////////////////////////////////////////////////////////////////////

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
#include "ofxXmlSettings.h"
#include <list>
#include <Models.h>
#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

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
  
	/// \defgroup OpenFrameWorks Default Functions 
	/// @{
	///	
	/// \brief These are the default Functions that are included in an empty OpenFrameWorks Project.
	/// Main functions to look at are the setup() and draw() functions
	

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::setup()
	///
	/// \brief OpenFrameworks First method that is called at the beginning of the program.Initialize all Variables Here.
	///
	////////////////////////////////////////////////////////////////////////////////////////////////////

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

	/// @} 
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::UpdateTracking();
	///
	/// \brief Updates the tracking received from the Vuzix i-Wear Tracker.
	///
	/// \author Rahul
	/// \date 9/21/2012
	////////////////////////////////////////////////////////////////////////////////////////////////////
			
	void UpdateTracking();
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::setupTracker()
	///
	/// \brief Sets up the Vuzix iWear Tracker.
	///
	/// \author Rahul
	/// \date 10/28/2012
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setupTracker();
	


	/*
	* \defgroup Network
	* @{
	*/
	/// \name UDP Connection Variables
	/// These Variables will be used to setup the UDP connections 

	/// \brief The UDP connection.
	ofxUDPManager udpConnection;

	/// \brief The UDP send connection.
	ofxUDPManager udpSendConnection;
	/// \brief The UDP receive connection.
	ofxUDPManager udpReceiveConnection;

	ofxUDPManager udpSendCameraPosition;

	/// \brief Sets up the Connections and Opens the ports
	void setupUDPConnections();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::UDPReceive()
	/// \name UDPReceive
	///
	/// \brief This function will receive the UDP Packets and check whether the command sent from the Android Phone includes information about adding a Notes Pane.
	///        If a note is added,then the message will be received in the format 
	///        "note,Latitude_of_note,Longitude_of_note,Note Heading,Note Text"
	///         
	/// \author Rahul
	/// \date 10/28/2012
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void UDPReceive();

	///@}
	
	/// \brief The Font Variable,Will store the fonts used for debugging...
	ofTrueTypeFont  mono;
	/// \brief The monosm.
	ofTrueTypeFont  monosm;


	/*
	* \defgroup GUI
	* @{
	*/
    /// \name GUI stuff ..
    /// These Functions can be easily replaced as OpenFrameWorks already has functions for the same ,but I am keeping them here for now ..
	/// \brief GUIFunctions 

	void drawAxes();

	void drawPlane();

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


	
///@}


	

	
	
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
	/// \property int windowWidth,windowHeight
	///
	/// \brief Gets the width and height of the window.
	///
	/// \brief The height of the window.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	int windowWidth,windowHeight;

	int gestureType;
	bool last_gesture_selected;

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


	/** \addtogroup GUI
	*  @{
	*/

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::setupFonts()
	///
	/// \brief Sets up the fonts
	/// These are not used much but can be used during debugging when you want to check which model is selected ..The selected model will appear on upper-middle part 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setupFonts();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::drawTouchImpressions(vector <string>message,bool somethingSelected=false)
	///
	/// \brief Draw touch impressions.
	///
	/// \author Rahul
	/// \date 10/28/2012
	///
	/// \param message   The Split components of the message
	/// \param somethingSelected (optional) Is something selected ?.
	////////////////////////////////////////////////////////////////////////////////////////////////////7
	
	void drawTouchImpressions(vector <string>message,bool);
	
	/* @}*/


	
	
	void setupCrosshair();
	void updateModelPositions();

	bool beginSelection;

	void drawCrosshair();
	string Receive_Message();

	

    void translateModel(vector <string>);
	void storeFingerPosition(vector<string>);
	void Check_if_Finger_Intersects_3DModel(vector <string>);
	void resetModelVariables();
	
	float scalexPosition,scaleyPosition,scaleFactor;
	
	vector <float> convertedTouchPoints;
	
	int Check_for_Finger_Intersections();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn void testApp::convertPhonetoScreenCoordinates(string rawTouchPoints)
	///
	/// \brief Convert phone to screen coordinates.
	///
	/// \param rawTouchPoints The raw touch points.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void convertPhonetoScreenCoordinates(string rawTouchPoints);

	void temp_calculateMaxandMin();

	vector<float>Crosshair_coords;
	int Check_for_crosshair_model_intersection();
	int crosshair_selected;


	/*
	* \defgroup XML
	* @{
	*/
	/// \name XML Stuff
	/// Takes care of loading the Models from the XML file,Storage,Displaying and Checking if the fingers touch the Model.
	/// \brief 
	

	ofxXmlSettings ModelsFile;

	vector<Models> ModelsList;

	/// Load Models from the XML file
	void loadModelsfromXML();

	/// Draws the models Loaded from the XML file (Default:data/Models.xml).
	void drawModelsXML();
	
	/// @}

	/// Width  of the Android Phone's Screen Resolution
	int AndroidPhoneResWidth;

	/// Height of the Android Phone's Screen Resolution
	int AndroidPhoneResHeight;


	/// \defgroup ARToolkitPlus Variables
	/// @{
	///	
	/// \brief These are the variables and Functions that ARToolkit uses.

	/* ARToolKitPlus class */	
	ofxARToolkitPlus artk;	
	int threshold;

	void setupARToolkitStuff();
	void checkifMarkerDetected();

	/// @}

	/// \defgroup OpenCV Variables
	/// @{
	///	
	/// \brief These are the variables and Functions that OpenCV uses.
	/* OpenCV images */
	
	ofVideoGrabber vidGrabber;
	ofxCvColorImage colorImage;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage	grayThres;

	int cameraWidth, cameraHeight;

	void SetupImageMatrices();
	void GrabCameraFrameandConvertMatrices();

	/// @}

	ofxAssimpModelLoader iphone5Model;


	/// Radar code ..

	void drawRadar();
	
};

#endif
