VUZIX SDK AR Extension README
Copyright(c) 2011 Vuzix Corporation

The Vuzix AR Extension is an extension to the standard Vuzix SDK.  It contains 2 additional sample programs intended to demonstrate how to interface with the WrapAR cameras and use the Alvar marker tracking libraries to manipulate a 3D scene.

----------------------------------------------------------------
REQUIREMENTS:

The Vuzix AR Extension requires the following libraries to be installed on your computer and linked to your development environment.
DirectShow Side by Side Sample:
	- Vuzix SDK 3.1 [http://vrdeveloper.vr920.com/]
	- Windows Platform SDK for Windows Server 2003 R2 or later 	[http://www.microsoft.com/downloads/en/details.aspx?	FamilyId=0BAF2B35-C656-4969-ACE8-	E4C0C0716ADB&displaylang=en]
	- DirectX 9.0 SDK 	[http://www.microsoft.com/downloads/en/details.aspx?	FamilyID=24a541d6-0486-4453-8641-1eee9e21b282]
DirectShow Alvar Sample (in addition to the previous libraries):
	- Alvar 1.4.0.2959 	[http://virtual.vtt.fi/virtual/proj2/multimedia/]
	- OpenCV 1.0 	[http://sourceforge.net/projects/opencvlibrary/files/openc	v-win/1.0/OpenCV_1.0.exe/download]

----------------------------------------------------------------
BUILD INSTRUCTIONS:

Debug builds are designed to run in windowed mode to ease debugging, while release builds run in fullscreen.

DirectShow Side by Side Sample:
  Add the following folder paths to your include directories:
	- <Vuzix SDK Path>\inc
	- <Platform SDK Path>\include
	- <Platform SDK Path>\samples\multimedia\directshow	\baseclasses
	- <DirectX SDK Path>\Include
  Add the following folder paths to your library directories:
	- <Vuzix SDK Path>\lib
	- <Platform SDK Path>\lib
	- <DirectX SDK Path>\lib

DirectShow Alvar Sample:
  Add the following folder paths to your include directories:
	- <Alvar Install Path>\include
	- <OpenCV Install Path>\cv\include
	- <OpenCV Install Path>\cvaux\include
	- <OpenCV Install Path>\cvcore\include
  Add the following folder paths to your library directories:
	- <Alvar Install Path>\bin\msvc90
	- <OpenCV Install Path>\lib

----------------------------------------------------------------
PROJECT DESCRIPTIONS:

DirectShow Side by Side Sample:

This sample program demonstrates how to capture and display video from two sources and display them in either a side by side or frame sequential format.  The program detects which Vuzix iWear product is connected to the PC and sets the stereo method accordingly.  When the Vuzix VR920 with CamAR is connected, the program will display a monoscopic camera image with frame sequential stereoscopic rendering.  When the Vuzix Wrap920AR is connected, it will display a pair of stereoscopic camera streams in a side by side arrangement.

The project uses DirectShow to retrieve the camera streams from the Vuzix AR cameras.  These streams are applied to DirectX quadrilateral as a texture, updating each frame.  It does not render anything aside from the streams.

DirectShow Alvar Sample:

This sample builds upon the previous program to add marker tracking with the Alvar image tracking library and 3D rendering with Direct3D.  This is accomplished with the addition of the Max3DARInterface.cpp file.  This file provides a variety of functions for accessing the necessary functions in the Alvar library with additional parameters to suit the needs of a stereoscopic rendering environment.  It also includes functions for covnerting Alvar's OpenGL environment to a Direct3D environment.

This sample tracks a marker in the images provided by the cameras and uses the Alvar library to obtain a transformation matrix.  It then applies that matrix to the Direct3D view matrix to manipulate the world in relation to the marker's location.  The sample demonstrates how to obtain the matrix from the Alvar library and use it to manipulate the rendering position of a simple cube.  The cube will appear to bestuck to the patterned side of the marker and will follow that marker as long as the pattern is visible.

This sample also provides an example for tracking secondary, movable markers as well.  A second transformation matrix can be obtained and applied to the Direct3D world matrix to manipulate the position of a second cube.  This cube will follow the position of a movable marker with respect to the standard marker.  If the program loses sight of the movable marker, it will stay in position in relation to the standard marker and will react accordingly when the standard marker is moved.

The program is set up to support up to 64 standard(stationary) markers and 64 movable markers, but currently only demonstrates tracking one of each.