#include<math.h>
#include"POIS.h"
class Calculations
{

public:

	float lat1,lat2,long1,long2;
	float lat1_rad,lat2_rad,long1_rad,long2_rad;
	float dlong,dlat;
	float bearing,bearing_correction;
	float xpos,ypos;
	float x_temp_poi,y_temp_poi;
	float Hdistance;
	int selected_model,selected_note,touch_selected;

	float intersection_pt1,intersection_pt2;


	//ofVec3f origin(0,0,10);
	//ofVec3f Bbay(116.747,-163.28,10);
	//ofVec3f receiver(-100.23,129.49,10);
	//ofVec3f destination1(-250,-300,10);
	//ofVec3f friend_loc(280.72705,244.184,10);
	ofVec3f test;

	ofVec3f origin,Bbay,receiver,destination1,friend_loc;
	ofVec2f touch_pos;

	Calculations()
	{ lat1=-43.522189; 
	long1=172.58292;

	lat2=-43.512705; // -43.512705,172.580267
	long2=172.580267; // y

	origin.set(0,0,10);
	Bbay.set(116.747,163.28,1);
	receiver.set(-100.23,-129.49,10);
	destination1.set(-250,300,10);
	friend_loc.set(280.72705,-244.184,10);
	test.set(0,1000,10);

	selected_model=0;
	selected_note=0;
	touch_selected=0;
	}

	float convertDegreestoRadians(float val)
	{

		// println("The value in radians are "+ (PI/180)*val);

		return ((PI/180)*val);


	}

	float convertRadianstoDegrees(float val)
	{return ((180/PI)*val);
	}

	float calculateHaversineDistance(float dlat,float dlong,float lat1_rad,float lat2_rad)
	{

		float R=6371000;

		float a = sin(dlat/2) * sin(dlat/2) +sin(dlong/2) * sin(dlong/2) * cos(lat1_rad) * cos(lat2_rad); 
		float c = 2 * atan(sqrt(a)/ sqrt(1-a) ); 
		float d = R * c;

		//println("Distance" + d);

		return d;  

	}

	void Calculate2dCoordinates(float lat1,float long1,float lat2,float long2)
	{

		lat1_rad= convertDegreestoRadians(lat1);
		lat2_rad= convertDegreestoRadians(lat2);
		long1_rad= convertDegreestoRadians(long1);
		long2_rad= convertDegreestoRadians(long2);

		dlong=long2_rad-long1_rad;
		dlat=lat2_rad-lat1_rad;

		bearing=atan((sin(dlong)*cos(lat2_rad))/( cos(lat1_rad)*sin(lat2_rad)-sin(lat1_rad)*cos(lat2_rad)*cos(dlong) ) );  

		//  println("Bearing is " + bearing );
		//  println("Bearing in degrees"+(bearing*180)/PI);

		bearing_correction=(int((bearing*180)/PI+360))%360; 

		//  println("Corrected Bearing"+bearing_correction);

		Hdistance=calculateHaversineDistance(dlat,dlong,lat1_rad,lat2_rad);

		x_temp_poi=sin(bearing)*Hdistance;
		y_temp_poi=cos(bearing)*Hdistance;

	}


	void check_intersection(float yaw,float pitch,vector <POIs>&scenes)
	{
		selected_model=0;

		ofVec3f long_ray(10000*sin(yaw),10000*cos(yaw),10+1000*sin(pitch));
		// 
		//// Check the Models First ......  
		if(ray_sphere_intersect(origin,long_ray,Bbay,30))
			selected_model=1;

		else if(ray_sphere_intersect(origin,long_ray,destination1,30))
			selected_model=2;

		else if(ray_sphere_intersect(origin,long_ray,friend_loc,30))
			selected_model=3;

		else if(ray_sphere_intersect(origin,long_ray,receiver,30))
			selected_model=4;

		else selected_model=0;
		// 


		/*if(ray_sphere_intersect(origin,ofVec3f(10000*sin(-0.62831837),10000*cos(-0.62831837),10),Bbay,40))
		cout<<"HIITTTTTTTTTTTTTT";*/

		for(int i=0;i<scenes.size();i++)
		{

			if(ray_sphere_intersect(origin,long_ray,ofVec3f(scenes[i].x,-scenes[i].y,10),15))
				selected_note=5+i;

			else selected_note=0;    
		}

		if(selected_note!=0)
			ofDrawBitmapString("Selected note is"+ofToString(selected_model),ofGetWidth()-230,80);

		if(selected_model!=0)
			ofDrawBitmapString(ofToString(selected_model),ofGetWidth()-230,30);

	}


	bool ray_sphere_intersect(ofVec3f p1,ofVec3f p2,ofVec3f sc,double r)
	{

		//  println("Vector 1\t" + p1 + "\tVector 2"+p2);

		//cout<<p1<<"\t\t"<<p2<<"\t"<<sc<<"\t\t"<<r<<"\n\n";


		float a,b,c;
		float bb4ac;
		ofVec3f diff=p2-p1;

		a = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		b = 2 * (diff.x * (p1.x - sc.x) + diff.y * (p1.y - sc.y) + diff.z * (p1.z - sc.z));
		c = sc.x * sc.x + sc.y * sc.y + sc.z * sc.z;
		c += p1.x * p1.x + p1.y * p1.y + p1.z * p1.z;
		c -= 2 * (sc.x * p1.x + sc.y * p1.y + sc.z * p1.z);
		c -= r * r;
		bb4ac = b * b - 4 * a * c;



		intersection_pt1=(-b+sqrt(bb4ac))/2*a;  
		intersection_pt2=(-b-sqrt(bb4ac))/2*a;

		if (bb4ac < 0||a<0||intersection_pt1>0) 
		{
			return(false);
		}

		//cout<<"intersection point 1"<<intersection_pt1<<"\tIntersection pt 2 is  \t"<<intersection_pt2;
		return true;


	}

	void check_touch_intersection(ofVec2f Touch_Pos,int windowWidth,int windowHeight,float yaw )
	{

		ofVec3f long_ray2(10000*sin(yaw+(convertDegreestoRadians(60*-(Touch_Pos.x/windowWidth)))),10000*cos(yaw+(convertDegreestoRadians(60*-(Touch_Pos.x/windowWidth)))),10+5000*sin((convertDegreestoRadians(60*-(Touch_Pos.y/windowHeight)))));

		touch_selected=0;
		//cout<<"   sfdsfs "<<60*Touch_Pos.x/windowWidth<<endl;
		///< Touch_Pos is the vector that stores the touch position from the center .

		if(ray_sphere_intersect(origin,long_ray2,Bbay,50))
		{
			touch_selected=1;
			cout<<"BBay"<<endl;
		}
		else if(ray_sphere_intersect(origin,long_ray2,destination1,50))
		{
			touch_selected=2;
			cout<<"Destination"<<endl;
		}
		else if(ray_sphere_intersect(origin,long_ray2,friend_loc,50))
		{
			touch_selected=3;
			cout<<"Friend Location"<<endl;
		}
		else if(ray_sphere_intersect(origin,long_ray2,receiver,50))
		{   
			touch_selected=4;
			cout<<"Receiver"<<endl;
		}

	}

	void updateBBayOrigin(float xPosition,float yPosition,float zPosition)
	{
		Bbay[0]=xPosition;
		Bbay[1]=-yPosition; /// Dunno y but the axis is Flipped ..
		Bbay[2]=zPosition;
	}

	void updateReceiverModelOrigin(float xPosition,float yPosition,float zPosition)
	{
		receiver[0]=xPosition;
		receiver[1]=-yPosition;
		receiver[2]=zPosition;
	}

	void updateFriendModelOrigin(float xPosition,float yPosition,float zPosition)
	{
		friend_loc[0]=xPosition;
		friend_loc[1]=-yPosition;
		friend_loc[2]=zPosition;
	}

	void updateDestinationOrigin(float xPosition,float yPosition,float zPosition)
	{
		destination1[0]=xPosition;
		destination1[1]=-yPosition;
		destination1[2]=zPosition;

	}

	
};