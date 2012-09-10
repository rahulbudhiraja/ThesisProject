#include "stdafx.h"
#include "IWRsdk.h"
#include <glut.h> // glut library includes standard gl and glu libraries as well
#include <glext.h> 
#include <wglext.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include<iostream>

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
void UpdateTracking();

bool g_tracking	= false;		// True = enable head tracking
float g_fYaw = 0.0f,  g_fPitch = 0.0f, g_fRoll = 0.0f;
long	iwr_status;

int	g_Filtering	= 0;			// Provide very primitive filtering process.
enum { NO_FILTER=0, APPLICATION_FILTER, INTERNAL_FILTER };

int	Pid	= 0;
using namespace std;

int main()
{
	HMODULE hMod;

	if( IWRGetProductID ) 
		Pid = IWRGetProductID();



	hMod = LoadLibrary("opengl32.dll");
// Open the VR920's tracker driver.
	iwr_status = IWRLoadDll();
    
	if( iwr_status != IWR_OK ) {
	//	std::cout<<"NO VR920 iwrdriver support", "VR920 Driver ERROR";
		IWRFreeDll();
		return 0;
	}

	while(1)
	{
		UpdateTracking();
		cout<<"\n\n "<<g_fPitch<<"\t "<<g_fRoll<<"\t"<<g_fYaw;
	}

	return 0;

}

void UpdateTracking(){
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