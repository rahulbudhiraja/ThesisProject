#include "ofx3DModelLoader.h"

class Models
{
	
public:
	
	
	float y;

	Models()
	{
	}

	void ModelsDraw()
	{
		ofPushMatrix();
		ofTranslate(116.747,-163.28,10);
		ofRotateZ(y);

		//bbox = new BoundingBox(this, bb_model);
		//   println(bbox.getCenter());

		//bb_model.translateToCenter();
		bb_model.draw();
		ofPopMatrix();
		//   pushMatrix();
		//   //translate(60,60);
		//   rotateX(y++);
		ofPushMatrix();
		ofTranslate(280.72705,244.184,10);
		//bbox = new BoundingBox(this, cr_model);
		ofRotateX(-PI/2);
		ofRotateY(y);

		//cr_model.translateToCenter();
		//cr_model.draw();//

		ofPopMatrix();
		//   popMatrix();

		ofPushMatrix();

		ofTranslate(-250,-300);
		ofRotateX(-PI/2);
		ofRotateY(y);
		//lr_model.translateToCenter();
		//lr_model.draw();

		ofPopMatrix();

		ofPushMatrix();

		ofTranslate(-100.23,129.49);
		ofRotateX(-PI/2);
		ofRotateY(y);
		//sr_model.translateToCenter();
		//sr_model.draw();

		ofPopMatrix();

		y+=0.1;

	}

};