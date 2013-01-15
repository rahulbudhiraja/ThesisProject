#include<Models.h>

void Models::setPath(string path)
{
	_path=path;

}

void Models::setId(int id)
{
	Id=id;
}


void Models::setRotationAxisandAngle(float rotationAngle,float rotx,float roty,float rotz)
{
	RotationAxis=ofVec3f(rotx,roty,rotz);
	_RotationAngle=rotationAngle;
}

void Models::setScale(float scaleX,float scaleY,float scaleZ)
{
	Scale[0]=scaleX;		
	Scale[1]=scaleY;
	Scale[2]=scaleZ;
	
	Model.setScale(Scale[0],Scale[1],Scale[2]);
}

void Models::loadandSetupModels()
{
	touch_selected=false;

	Model.loadModel(_path,1);
	
	Model.setScaleNomalization(false);
	Model.setScale(Scale[0],Scale[1],Scale[2]);

	Model.setPosition(-Model.getSceneCenter().x,-Model.getSceneCenter().y,-Model.getSceneCenter().z);
    Model.setRotation(0,_RotationAngle,RotationAxis[0],RotationAxis[1],RotationAxis[2]);

	temp_translate.set(0,0,0);
	calculateSceneMaxandMin();

	rotZ=0;
}

void Models::setPosition(float x,float y,float z)
{
	Position.x=x;
	Position.y=y;
	Position.z=z;

}
void Models::drawModel()
{

	ofPushMatrix();

		ofTranslate(Position+temp_translate);
		ofRotateZ(rotZ);

		if(touch_selected)
		Model.drawWireframe();
		else Model.drawFaces();

	ofPopMatrix();

	rotZ+=0.1;
}


void Models::calculateSceneMaxandMin()
{
	UpdatedSceneMax=Position+Model.getSceneMax(true).rotate(_RotationAngle,RotationAxis);
	UpdatedSceneMin=Position+Model.getSceneMin(true).rotate(_RotationAngle,RotationAxis);
}

void Models::checkifFingersTouchModel(vector<float>TouchPoints)
{

	//touch_selected=intersect_model(TouchPoints);
}

bool Models::intersect_model(vector <float>Touches,ofCamera camera)
{
	bool doesintersect=false;

	calculateSceneMaxandMin();
	//cout<<"\n\n    "<<UpdatedSceneMax_screencoods<<"     "<<UpdatedSceneMin_screencoods;
	UpdatedSceneMax_screencoods=camera.worldToScreen(UpdatedSceneMax);
	UpdatedSceneMin_screencoods=camera.worldToScreen(UpdatedSceneMin);

	if(Touches[0]==2)
	{
			
		MidPoint.x=(Touches[1]+Touches[3])/2;
		MidPoint.y=(Touches[2]+Touches[4])/2;

		if(MidPoint.x>min(UpdatedSceneMax_screencoods.x,UpdatedSceneMin_screencoods.x)&&MidPoint.x<max(UpdatedSceneMax_screencoods.x,UpdatedSceneMin_screencoods.x)&&MidPoint.y>min(UpdatedSceneMax_screencoods.y,UpdatedSceneMin_screencoods.y)&&MidPoint.y<max(UpdatedSceneMax_screencoods.y,UpdatedSceneMin_screencoods.y))
			doesintersect= true;

	}

	else if(Touches[0]==1)
		{
			if(Touches[1]>min(UpdatedSceneMax_screencoods.x,UpdatedSceneMin_screencoods.x)&&Touches[1]<max(UpdatedSceneMax_screencoods.x,UpdatedSceneMin_screencoods.x)&&Touches[2]>min(UpdatedSceneMax_screencoods.y,UpdatedSceneMin_screencoods.y)&&Touches[2]<max(UpdatedSceneMax_screencoods.y,UpdatedSceneMin_screencoods.y))
			doesintersect= true;
		}

	if(UpdatedSceneMax_screencoods.z>1||UpdatedSceneMin_screencoods.z>1)
	{doesintersect=false;/*cout<<"zzzzzzz "<<UpdatedSceneMax_screencoods.z<<"\t\t"<<UpdatedSceneMin_screencoods.z<<"\n\n\n\n";*/}


	return doesintersect;
}

void Models::translateModel(vector<string>udpMessage,float last_single_touch[2],int gestureType,ofCamera cam,float windowWidth,float windowHeight,float AndroidPhoneResWidth,float AndroidPhoneResHeight)
{
	if(gestureType==1)
	{
		temp_translate.x=(ofToFloat(udpMessage[1])-last_single_touch[0])/3;
		temp_translate.y=(ofToFloat(udpMessage[2])-last_single_touch[1])/3;
	}

	else if(gestureType==2)
	{
		Position.x=(ofToFloat(udpMessage[1])+ofToFloat(udpMessage[3]))/2;
		Position.y=(ofToFloat(udpMessage[2])+ofToFloat(udpMessage[4]))/2;		

		Position=cam.screenToWorld( ofVec3f( windowWidth*Position.x/AndroidPhoneResWidth, windowHeight*Position.y/AndroidPhoneResHeight,  0.975f ) );

		Position.z=10;
		
	}

	temp_translate.z=1;

	touch_selected=true;

	cout<<"\n\n"<<Position<<"\n";

}

void Models::updatePosition()
{

	Position.x+=temp_translate.x;
	Position.y+=temp_translate.y;

	temp_translate.x=temp_translate.y=0;

}

ofxAssimpModelLoader Models::getModel()
{
	return Model;

}

void Models::setPreviousScale()
{
	if(Previous_Scale[0]>1.2)
	{
		Scale[0]=Previous_Scale[0];
		Scale[1]=Previous_Scale[1];
		Scale[2]=Previous_Scale[2];
	}
	
	else Scale[0]=Scale[1]=Scale[2]=1.2;
}

void Models::updateScale()
{
	Previous_Scale[0]=Scale[0];
	Previous_Scale[1]=Scale[1];
	Previous_Scale[2]=Scale[2];

}

void Models::setDescription(string description)
{
	Description=description;

} 

string Models::getDescription()
{
	return Description;
	
}
