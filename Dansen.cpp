#include <iostream>
#include <string>
#include "BrickPi3.h"
#include <unistd.h>
#include <signal.h> 


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


//-Eye functions
const int lookAngle = 105;

// Turn the eyes left
void lookLeft(){
    BP.set_motor_position_relative(PORT_D, lookAngle);
}

// Turn the eyes right
void lookRight(){
    BP.set_motor_position_relative(PORT_D, -lookAngle);
}

// Error handler
void eHandler(int s){
    cout << "Exiting..." << endl;
    BP.reset_all();
    exit(0);
}

void Dansje(){

    BP.set_motor_limits(PORT_B, 90, 0);
    BP.set_motor_limits(PORT_C, 90, 0);

    if(Rood > Groen && Rood > Blauw){

        cout << "=> Rode dans !!" << endl;

        lookLeft();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookLeft();
        usleep(500000);
        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        sleep(1);

        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookLeft();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);
        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);
        
        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -410);
        BP.set_motor_position_relative(PORT_C, -410);
        usleep(500000);
        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 410);
        BP.set_motor_position_relative(PORT_C, 410);
        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        usleep(750000);
        lookLeft();
        sleep(3);

        }
    else if(Groen > Rood && Groen > Blauw){

        cout << "=> Groene Dans !!" << endl;
        
        BP.set_motor_position_relative(PORT_B, 360);
        usleep(500000); 
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -360);
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        sleep(1);

        BP.set_motor_position_relative(PORT_C, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);
        
        BP.set_motor_position_relative(PORT_C, 360);
        BP.set_motor_position_relative(PORT_C, 360);
        sleep(1);

        BP.set_motor_position_relative(PORT_C, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 360);
        usleep(500000); 
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 360);
        BP.set_motor_position_relative(PORT_C, 360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        sleep(1);

        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        sleep(3);

    }

    else if(Blauw > Rood && Blauw > Groen){

        cout << "=> Blauwe dans !!" << endl;

        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        usleep(1000000);
        BP.set_motor_position_relative(PORT_B, 1680);
        BP.set_motor_position_relative(PORT_C, -1680);
        usleep(1000000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookRight();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookRight();
        usleep(750000);
        BP.set_motor_position_relative(PORT_B, -840);
        BP.set_motor_position_relative(PORT_C, 840);
        usleep(750000);
        lookLeft();
        sleep(3);
    }
    else{
        cout << "Ongeldige waarde !!!" << endl;
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
    
    Dansje();

    }

    else{
        cout << "Sensor werkt niet" << endl;
        }   
    }
}

int main(){

BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

    // Initialize the ctrl + c catch
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = eHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    MeetKleuren();

}