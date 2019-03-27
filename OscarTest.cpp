#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;
sensor_color_t      Color1;
sensor_ultrasonic_t Ultrasonic2;
sensor_light_t      Light3;

void exit_signal_handler(int signo);

void moveStop() {
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	// Zet stroom van poort B en C op 0, waardoor de robot stopt.
	return;
}

void moveFwd(const int & time) {
	BP.set_motor_dps(PORT_B, 180);
	BP.set_motor_dps(PORT_C, 180);
	usleep(time);
	// Draai de motor op port B en C 360 graden
	return;
}

void moveLeft(const int & time) {
	
		BP.set_motor_dps(PORT_B, 90);
		BP.set_motor_dps(PORT_C, -90);
		usleep(time);
	return;
}

void moveRight(const int & time) {
		BP.set_motor_dps(PORT_B, -90);
		BP.set_motor_dps(PORT_C, 90);
		usleep(time);
	return;
}

void moveBack(const int &time) {
	BP.set_motor_dps(PORT_B, -180);
	BP.set_motor_dps(PORT_C, -180);
	usleep(time);
	// Draai de motor op port B en C -360 graden
	return;
}

void avoidObstacle() {
	cout << "starting obstacel detection..." << endl;
	int stepOne = 0;
	int stepTwo = 0;
	int stepThree = 0;
	int stepFour = 0;
	int stepFive = 0;
	while (true) {
		if (stepOne == 0) {
			BP.set_motor_position_relative(PORT_B, 360);
			BP.set_motor_position_relative(PORT_C, -360);
			stepOne = 1;
			usleep(2000000);
		}
		else if (stepTwo == 0 && stepOne == 1) {
			moveFwd(4000000);
			stepTwo = 1;
		}
		else if (stepThree == 0 && stepTwo == 1) {
			BP.set_motor_position_relative(PORT_B, -360);
			BP.set_motor_position_relative(PORT_C, 360);
			stepThree = 1;
			usleep(2000000);
		}
		else if (stepFour == 0 && stepThree == 1) {
			moveFwd(4000000);
			stepFour = 1;
		}
		else if (stepFive == 0 && stepFour == 1) {
			BP.set_motor_position_relative(PORT_B, -360);
			BP.set_motor_position_relative(PORT_C, 360);
			stepFive = 1;
			usleep(2000000);
		}
	}	
	return;
}



void driveByLine() {

	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	int measurement = 0;

	while (true) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			cout << "searching line..." << endl;
			if (Ultrasonic2.cm > 10) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
					measurement = Light3.reflected;
					if (measurement >= 1900 && measurement <= 2300) {
						moveFwd(100000);
						//rechtdoor
					}
					else if (measurement > 1800 && measurement < 1900) {
						moveLeft(100000);
						//als ie het wit in gaat
					}
					else if (measurement > 2300) {
						moveRight(100000);
						//als ie het zwart in gaat
					}
					//usleep(250000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)
				}
			}
			else
			{
				avoidObstacle();
				break;
			}
		}
	}
	driveByLine();
}

int averageValues(const int red, const int green, const int blue) {
	int average = (red + green + blue) / 3;
	return average;
}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	driveByLine();

	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}