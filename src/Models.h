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
	
	float Scale[3];

	ofxAssimpModelLoader Model;

	bool touch_selected;
	ofVec3f UpdatedSceneMax,UpdatedSceneMin;

public:

	void setPath(string path);
	void setId(int);
	void setRotationAxisandAngle(float,float,float,float);
	void setScale(float,float,float);
	void loadandSetupModels();
	void drawModel();
	void setPosition(float,float,float);
	void calculateSceneMaxandMin();
	void checkifFingersTouchModel(vector<float>);
	bool intersect_model(ofVec3f,ofVec3f,vector <float>);

};
