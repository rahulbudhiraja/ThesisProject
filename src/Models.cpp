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
}

void Models::loadandSetupModels()
{
	touch_selected=false;

	Model.loadModel(_path,1);
	
	Model.setScaleNomalization(false);
	Model.setScale(Scale[0],Scale[1],Scale[2]);

	Model.setPosition(-Model.getSceneCenter().x,-Model.getSceneCenter().y,-Model.getSceneCenter().z);
    Model.setRotation(0,_RotationAngle,RotationAxis[0],RotationAxis[1],RotationAxis[2]);


	calculateSceneMaxandMin();
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

		ofTranslate(Position);
		ofRotateZ(0);

		if(touch_selected)
		Model.drawWireframe();
		else Model.drawFaces();

	ofPopMatrix();

}


void Models::calculateSceneMaxandMin()
{
	UpdatedSceneMax=Position+Model.getSceneMax(true).rotate(_RotationAngle,RotationAxis);
	UpdatedSceneMin=Position+Model.getSceneMin(true).rotate(_RotationAngle,RotationAxis);

}

void Models::checkifFingersTouchModel(vector<float>TouchPoints)
{

	touch_selected=intersect_model(UpdatedSceneMax,UpdatedSceneMin,TouchPoints);
}

bool Models::intersect_model(ofVec3f pt1,ofVec3f pt2,vector <float>Touches)
{
	bool doesintersect=false;

	//cout<<"\n\n    "<<pt1<<"     "<<pt2;

	for(int i=1;i<=2*Touches[0];i+=2)
	{ 	
		if(Touches[i]>min(pt1.x,pt2.x)&&Touches[i]<max(pt1.x,pt2.x)&&Touches[i+1]>min(pt1.y,pt2.y)&&Touches[i+1]<max(pt1.y,pt2.y))
			doesintersect= true;
	}

	if(pt1.z>1||pt2.z>1)
	{doesintersect=false;/*cout<<"zzzzzzz "<<pt1.z<<"\t\t"<<pt2.z<<"\n\n\n\n";*/}

	return doesintersect;
}