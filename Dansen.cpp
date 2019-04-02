#include <iostream>
#include <string>
#include "BrickPi3.h"


using std::cin;
using std::string;
using std::cout;
using std::endl;
using std::getline;

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

//Sensor definitions
sensor_light_t      Light3; //RGB Light sensor
sensor_color_t      Color1; //Infrared sensor
sensor_ultrasonic_t Ultrasonic2; //Ultrasonic sensor


void KleurenDans(){

while(true){
    if(BP.get_sensor(PORT_1, Color1) == 0){

    int Rood = Color1.reflected_red;
    int Groen = Color1.reflected_green;
    int Blauw = Color1.reflected_blue;


    cout << Rood << endl << Groen << endl << Blauw << endl;



    }
    else{
        cout << "dinges werkt niet" << endl;
    }
}
}
int main(){

BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

    KleurenDans();

}