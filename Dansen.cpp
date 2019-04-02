#include <iostream>
#include <string>
#include "BrickPi3.h"
#include <unistd.h>


using std::cin;
using std::string;
using std::cout;
using std::endl;
using std::getline;

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

//Sensor definitions
sensor_light_t      Light3; //Infrared sensor
sensor_color_t      Color1; //RGB light sensor
sensor_ultrasonic_t Ultrasonic2; //Ultrasonic sensor

int Rood;
int Groen;
int Blauw;

void Dansje(){

    if(Rood > Groen && Rood > Blauw){
        cout << "Doet iets bij rood" << endl;
    }
    else if(Groen > Rood && Groen >> Blauw){
        cout << "Doet iets bij groen" << endl;
    }
    else if(Blauw > Rood && Blauw > Groen){
        cout << "Doet iets bij blauw" << endl;
    }
    else{
        cout << "ongeldige waarde !!!" << endl;
    }

}

void MeetKleuren(){

while(true){

    sleep(1);

    if(BP.get_sensor(PORT_1, Color1) == 0){

    Rood = Color1.reflected_red;
    Groen = Color1.reflected_green;
    Blauw = Color1.reflected_blue;

    cout << endl;
    cout << "Kleurwaarde voor Rood = "<< Rood << endl;
    cout << "Kleurwaarde voor Groen = " << Groen << endl;
    cout << "Kleurwaarde voor Blauw = " << Blauw << endl;
    
    Dansje()

    }

    else{
        cout << "Sensor werkt niet" << endl;
        }   
    }
}

int main(){

BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

    MeetKleuren();

}