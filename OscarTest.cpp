#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

void moveStop() {
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	// Zet stroom van poort B en C op 0, waardoor de robot stopt.

	cout << "I AM NOT MOVING" << endl;
}

void moveFwd() {
	BP.set_motor_dps(PORT_B, 360);
	BP.set_motor_dps(PORT_C, 360);
	// Draai de motor op port B en C 360 graden
	cout << "I AM MOVING FORWARD" << endl;

}

void moveLeft() {
	BP.set_motor_position_relative(PORT_B, 270);
	BP.set_motor_position_relative(PORT_C, -270);
	// Draai het wiel op port B 270 graden en de wiel op port C -270 graden

	cout << "I AM MOVING LEFT" << endl;

}

void moveRight() {
	BP.set_motor_position_relative(PORT_B, -270);
	BP.set_motor_position_relative(PORT_C, 270);
	// Draai het wiel op port B -270 graden en de wiel op port C 270 graden

	cout << "I AM MOVING RIGHT" << endl;

}

void moveBack() {
	BP.set_motor_dps(PORT_B, -360);
	BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden

	cout << "I AM MOVING BACKWARDS" << endl;

}

int main() {

	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

	sensor_color_t      Color1;

	while (true) {

		if (BP.get_sensor(PORT_1, Color1) == 0) {
			if ((int)Color1.reflected_red < 300) {
				moveFwd();
			}
			else if ((int)Color1.reflected_red < 150){
				moveLeft();
			}
			else if ((int)Color1.reflected_red > 500){
				moveRight();
			}
			else
			{
				moveStop();
			}
			cout << "Color sensor (S1): detected  " << (int)Color1.color;
			cout << " red" << setw(4) << Color1.reflected_red;
			cout << " green" << setw(4) << Color1.reflected_green;
			cout << " blue" << setw(4) << Color1.reflected_blue;
			cout << " ambient" << setw(4) << Color1.ambient << endl;
		}
		sleep(1);
	}
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}