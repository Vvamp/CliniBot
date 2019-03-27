#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

void moveBot(const int & valueLeft, const int & valueRight) {
	BP.set_motor_power(PORT_B, valueRight);
	BP.set_motor_power(PORT_C, valueLeft);
	cout << "[L: " << valueLeft << "] [R: " << valueRight << "]";
}

int averageValues(const int red, const int green, const int blue) {
	int average = (red + green + blue) / 3;
	return average;
}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	sensor_color_t      Color1;
	sensor_light_t      Light3;

	int average = 0;
	int measurement = 0;

	while (true) {

		if (BP.get_sensor(PORT_3, Light3) == 0) {
			measurement = Light3.reflected;
			if (measurement >= 1900 && measurement <= 2300) {
				moveBot(180, 180);
			}
			else if (measurement > 1800 && measurement < 1900) {
				moveBot(80, -80);
			}
			else if (measurement > 2300) {
				moveBot(-80, 80);
			}
				
		}
		
		}
		usleep(125000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)
	}
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}