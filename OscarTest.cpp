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

	cout << " Stopped - ";
}

void moveFwd() {
	BP.set_motor_dps(PORT_B, 360);
	BP.set_motor_dps(PORT_C, 360);
	// Draai de motor op port B en C 360 graden
	cout << " Forward - ";

}

void moveLeft() {
	// BP.set_motor_position_relative(PORT_B, 45);
	// BP.set_motor_position_relative(PORT_C, -45);
	BP.set_motor_dps(PORT_B, 360);
	BP.set_motor_dps(PORT_B, 100);
	// Draai het wiel op port B 270 graden en de wiel op port C -270 graden

	cout << " Left - ";

}

void moveRight() {
	// BP.set_motor_position_relative(PORT_B, -25);
	// BP.set_motor_position_relative(PORT_C, 25);
	BP.set_motor_dps(PORT_B, 100);
	BP.set_motor_dps(PORT_B, 360);
	// Draai het wiel op port B -270 graden en de wiel op port C 270 graden

	cout << " Right - ";

}

void moveBack() {
	BP.set_motor_dps(PORT_B, -360);
	BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden

	cout << " Back - ";

}

int main() {

	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

	sensor_color_t      Color1;

	while (true) {

		if (BP.get_sensor(PORT_1, Color1) == 0) {
			if ((int)Color1.reflected_red < 300 && (int)Color1.reflected_red > 150) {
				moveLeft();
			}
			else if ((int)Color1.reflected_red <= 150){
				moveFwd();
			}
			else if ((int)Color1.reflected_red >= 450){
				moveRight();
			}
			else
			{
				// moveStop();
				moveFwd();
			}
			// cout << "Color sensor (S1): detected  " << (int)Color1.color;
			cout << " [R:" << setw(4) << Color1.reflected_red;
			cout << " G:" << setw(4) << Color1.reflected_green;
			cout << " B:" << setw(4) << Color1.reflected_blue;
			cout << " A:" << setw(4) << Color1.ambient << "]" << endl;
		}
		usleep(100000);
	}
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}