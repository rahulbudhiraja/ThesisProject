#include<string>
#include "ofxAssimpModelLoader.h"

using namespace std;

class Models
{

public:
	int Id;
	string _path;
	
	ofVec3f Position;
	
	ofVec3f RotationAxis;
	float _RotationAngle;
	float rotZ;

	float Scale[3];


	ofxAssimpModelLoader Model;

	bool touch_selected;
	ofVec3f UpdatedSceneMax,UpdatedSceneMin;
	ofVec2f MidPoint;
	ofVec3f UpdatedSceneMax_screencoods,UpdatedSceneMin_screencoods;
	ofVec3f temp_translate;
	string Description;


public:

	float Previous_Scale[3];

	void setPath(string path);
	void setId(int);
	void setRotationAxisandAngle(float,float,float,float);
	void setScale(float,float,float);
	void setDescription(string description);
	void loadandSetupModels();
	void drawModel();
	void setPosition(float,float,float);
	void calculateSceneMaxandMin();
	void checkifFingersTouchModel(vector<float>);
	bool intersect_model(vector <float>,ofCamera);
	void translateModel(vector<string>,float[2],int,ofCamera,float windowWidth=0,float windowHeight=0,float AndroidPhoneResWidth=0,float AndroidPhoneResHeight=0);
	void updatePosition();
	ofxAssimpModelLoader getModel();
	void setPreviousScale();
	void updateScale();
	string getDescription();
};
