#include <string>
#include<iostream>

using namespace std;

void addObjecttoScene(string message)
{
 
 string[] messageParts =split(message,',');
 
 if(messageParts[0].equals("note"))
 {
   println(messageParts[3] + messageParts[4]);
   Notes.add(new NoteInformation(messageParts[3],messageParts[4],white,black)); 
   
   println(messageParts[1]+messageParts[2]);
   Calculate2dCoordinates(lat1,long1,Float.parseFloat(messageParts[1]),Float.parseFloat(messageParts[2])); 
   
 
 }
 
 cout<<"Xposition\t"<<x_temp_poi;
 cout<<"\n\nYposition\t"+y_temp_poi;  
  
  scenes.add(new POIs(x_temp_poi,y_temp_poi));

}


