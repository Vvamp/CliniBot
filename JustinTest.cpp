#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <string>       // Include strings
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

int calcSpeed(){

}

//Function to move robot (left, right)
void moveBot(const int valueLeft, const int valueRight) {
	BP.set_motor_dps(PORT_C, valueLeft); //Left motor
    BP.set_motor_dps(PORT_B, valueRight); // Right motor
    cout < " " << endl;
    cout << "Clinibot travel values --" << endl;
    cout << "Left: " << valueLeft << endl;
    cout << "Right: " << valueRight << endl;
}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light3;

	int average = 0;
	int measurement = 0;

	while (true) {

		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (Ultrasonic2.cm > 10) {

				if (BP.get_sensor(PORT_3, Light3) == 0) {
					measurement = Light3.reflected;
					if (measurement >= 1900 && measurement <= 2300) {
						moveBot(180, 180); //Forward
					}
					if (measurement > 1800 && measurement < 1900) {
						moveBot(-180, 180); //Left
					}
					else if (measurement > 2300) {
						moveBot(180, -180); //Right
					}
				}
			}
			else
			{
				moveBot(0, 0);
			}

            usleep(50000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)

		}
		else
		{
			cout << "can't find the ultrasonic sensor" << endl;
		}
	}	
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}