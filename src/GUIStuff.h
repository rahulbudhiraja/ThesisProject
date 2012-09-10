
class GUIStuff
{

	int i,j;
	ofTrueTypeFont Serif_15;
	ofTrueTypeFont Serif_10;

public:

	GUIStuff() 
	{
		i=j=0;
		Serif_10.loadFont("Serif.ttf",10);
		Serif_15.loadFont("Serif.ttf",15);
	}

	void drawAxes()
	{

		ofSetColor(255, 255, 255);
		ofLine(100,0,0,0); 

		ofSetColor(0,255,0);
		ofLine(0,100,0,0); 

		ofSetColor(0,0,255);
		ofLine(0,0,0,0,0,100); 

	}

	void drawPlane()
	{

		ofSetColor(255,255,255);
		//  translate(0,0,-10);
		for(i=-200;i<200;i+=20)
		{
			ofLine(-200,i,200,i);
			ofLine(i,-200,i,200);
		}
		//    translate(0,0,10);
	}

	void drawAugmentedPlane(float xPosition,float yPosition,ofColor textColor,ofColor bgColor,int i,string note_heading,string note_text)
	{

		

		//  rotateZ( P(bearing)   );

		//  println("Bearing"+bearing);

		//  Notes.lastElement().

		ofTranslate(xPosition,yPosition,0); 

		ofRotateZ(PI/2+acos((xPosition/sqrt(xPosition*xPosition+yPosition*yPosition)) ) ); // Moving the plane so that it faces the user ...

		//println(PI/2-acos((xPosition/sqrt(xPosition*xPosition+yPosition*yPosition)) ) );
		ofPushMatrix();

		ofRotateZ(PI/2);

		ofSetColor(bgColor);

		ofBeginShape();

		ofVertex(0,-50,0);
		ofVertex(0,50,0);
		ofVertex(0,50,50);
		ofVertex(0,-50,50);

		ofEndShape();

		ofPopMatrix();

		ofSetColor(textColor);

		ofPushMatrix();
		ofRotateX(-PI/2); 

  		Serif_15.drawString(note_heading,-50,-40); // May require Changes ..
		Serif_10.drawString(note_text,-50,-30);


		ofPopMatrix();


	}
};