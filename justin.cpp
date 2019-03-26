#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int main(){
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

    BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

    int error;

    //Get sensor
    BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
    sensor_color_t      Color1;

    while(true){
    error = 0;

    if(BP.get_sensor(PORT_1, Color1) == 0){
        cout << "Color sensor (S1): detected  " << (int) Color1.color;
        cout << " red" << setw(4) << Color1.reflected_red;
        cout << " green" << setw(4) << Color1.reflected_green;
        cout << " blue" << setw(4) << Color1.reflected_blue;
        cout << " ambient" << setw(4) << Color1.ambient << endl;
    }

    cout << "....." << endl;

    sleep(1);
    }
    }

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
