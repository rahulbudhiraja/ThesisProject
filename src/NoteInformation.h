#include <string>



#ifndef NoteInformation_h
#define NoteInformation_h

class NoteInformation{
	
public:
	string note_heading,note_text;
	ofColor text_color,bg_color;
        
	 NoteInformation(string textheading,string notetext,ofColor tcolor,ofColor bgcolor)
	{
		note_heading=textheading;
		text_color=tcolor;
		bg_color=bgcolor;
		note_text=notetext;	
	}
	 NoteInformation()
	 {

		 ;
	 }
};
#endif