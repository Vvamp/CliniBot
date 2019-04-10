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

/* Control CliniBot with WASD
void controlTerminal(){
    // Show movement controls
    showControls();

    // Curses settings
    initscr();      // Init curses !!! Anything that gets printed after this will be printed weirdly
    cbreak();       // Sets that the code buffers per-key and not per newline
    noecho();       // Don't print the characters entered
    timeout(1750);  // Check once every 1,750ms

    // Control the robot
    while (true){
        int userIn = getch();   // Request a single character from the user
        refresh();              // Refresh the screen

        // Check the input of the user
        if (userIn == 'w'){
            moveFwd();
       }else if(userIn == 'a'){
           moveLeft();
       }else if(userIn == 'd'){
           moveRight();
       }else if(userIn == 's'){
            moveBack();
       }else if(userIn == 'e'){
           break;
       }else if(userIn == 'p'){
            moveStop();
        }
     }
}
*/

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

    BP.set_motor_limits(PORT_B, 120, 0);
    BP.set_motor_limits(PORT_C, 120, 0);

    if(Rood > Groen && Rood > Blauw){

        cout << "=> Rode dans !!" << endl;

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);

        //lookLeft();
        //sleep(2);

        /*BP.set_motor_dps(PORT_B, -360);
        BP.set_motor_dps(PORT_C, -360);    
        usleep(500000);
        BP.set_motor_dps(PORT_B, 360);
        BP.set_motor_dps(PORT_C, 360);
        usleep(500000);
        BP.set_motor_dps(PORT_B, 360);
        BP.set_motor_dps(PORT_C, -360);
        sleep(2);
        BP.set_motor_dps(PORT_B, -360);
        BP.set_motor_dps(PORT_C, -360);
        usleep(500000);
        BP.set_motor_dps(PORT_B, 360);
        BP.set_motor_dps(PORT_C, 360);
        usleep(500000);*/

        /*BP.set_motor_power(PORT_B, 90);
	    BP.set_motor_power(PORT_C, -90);
	    usleep(750000);
        BP.set_motor_power(PORT_B, 90);
        BP.set_motor_power(PORT_C, -90);
        usleep(750000);
        BP.set_motor_power(PORT_B, 90);
        BP.set_motor_power(PORT_C, -90);
        usleep(750000);
        BP.set_motor_power(PORT_B, 90);
        BP.set_motor_power(PORT_C, -90);
        sleep(2);
        BP.set_motor_power(PORT_B, 0);
        BP.set_motor_power(PORT_C, 0);*/

        //lookRight();
        //sleep(2);
        //lookRight();
        //sleep(2);

        /*BP.set_motor_power(PORT_B,-90);
        BP.set_motor_power(PORT_C, 90);
        usleep(750000);
        BP.set_motor_power(PORT_B,-90);
        BP.set_motor_power(PORT_C, 90);
        usleep(750000);
        BP.set_motor_power(PORT_B,-90);
        BP.set_motor_power(PORT_C, 90);
        usleep(750000);
        BP.set_motor_power(PORT_B,-90);
        BP.set_motor_power(PORT_C, 90);
        sleep(2);

        BP.set_motor_power(PORT_B, 0);
        BP.set_motor_power(PORT_C, 0);*/

        //lookLeft();
        //sleep(2);

        }
    else if(Groen > Rood && Groen > Blauw){

        cout << "=> Groene Dans !!" << endl;
        
        BP.set_motor_power(PORT_B, 20);
	    BP.set_motor_power(PORT_C, -20);

        sleep(1);
        lookLeft();
        sleep(1);

        BP.set_motor_power(PORT_B, -20);
        BP.set_motor_power(PORT_C, -20);
        usleep(750000);
        BP.set_motor_power(PORT_B, -20);
	    BP.set_motor_power(PORT_C, 20);
        usleep(750000);
        BP.set_motor_power(PORT_B, -20);
	    BP.set_motor_power(PORT_C, 20);

        sleep(1);
        lookRight();
        sleep(2);
        lookRight();
        sleep(1);

        BP.set_motor_power(PORT_B, -20);
        BP.set_motor_power(PORT_C, -20);
        usleep(750000);
        BP.set_motor_power(PORT_B, 12);
	    BP.set_motor_power(PORT_C, -12);
        sleep(1);
        lookLeft();
        sleep(1);
        
        /*BP.set_motor_dps(PORT_B, 120);
        BP.set_motor_dps(PORt_C, -60);
        sleep(1);
        BP.set_motor_dps(PORT_B, 120);
        BP.set_motor_dps(PORT_C, -60);
        sleep(1);
        BP.set_motor_dps(PORT_B, 120);
        BP.set_motor_dps(PORT_C, -60);
        sleep(1);*/

    }
    else if(Blauw > Rood && Blauw > Groen){

        cout << "=> Blauwe dans !!" << endl;

        BP.set_motor_power(PORT_B, 5);
        BP.set_motor_power(PORT_C, -10);
        sleep(1);
        BP.set_motor_power(PORT_B, -10);
        BP.set_motor_power(PORT_C, 5);
        sleep(1);
        BP.set_motor_power(PORT_B, -5);
        BP.set_motor_power(PORT_C, 10);
        sleep(1);
        BP.set_motor_power(PORT_B, 10);
        BP.set_motor_power(PORT_C, -5);
        sleep(1);
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