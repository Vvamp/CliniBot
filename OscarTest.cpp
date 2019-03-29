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
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, 20);
	usleep(time);
	// Draai de motor op port B en C 360 graden
	return;
}

void moveLeft(const int & time) {
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
	return;
}

void moveRight(const int & time) {
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, 20);
		usleep(time);
	return;
}

void moveBack(const int &time) {
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
	// Draai de motor op port B en C -360 graden
	return;
}
void obstacleAvoidenceFwd() {

	int looking = 0;
	bool object = false;
	moveFwd(1500000);
	moveStop();
	BP.set_motor_position_relative(PORT_D, -110);
	usleep(1000000);
	while (looking <= 2000) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
				looking++;
				cout << Ultrasonic2.cm << " cm" << endl;
				if (Ultrasonic2.cm < 60) {
					object = true;
				}
				else {
					object = false;
				}
			}
		}
	}
	BP.set_motor_position_relative(PORT_D, 110);
	usleep(1000000);
	if (object == true) {
		obstacleAvoidenceFwd();
	}
	else {
		return;
	}
}

void obstacleAvoidenceLeft() {
	while (true) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (Ultrasonic2.cm < 30) {
				moveLeft(100000);
			}
			else if (Ultrasonic2.cm > 30) {
				moveLeft(1500000);
				return;
			}
		}
	}
	
}
void obstacleAvoidenceRight() {
	int looking = 0;
	bool object = false;
	moveRight(500000);
	moveStop();
	BP.set_motor_position_relative(PORT_D, -80);
	usleep(1000000);
	while (looking <= 2000) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			looking++;
			if (Ultrasonic2.cm > 50) {
				object = true;
			}
			else {
				object = false;
			}
		}
	}
	BP.set_motor_position_relative(PORT_D, 80);
	if (object == true) {
		usleep(1000000);
		obstacleAvoidenceRight();
	}
	else {
		return;
	}
}

void obstacleAvoidenceEnding() {
	while (true) {
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			if (Light3.reflected > 1800 && Light3.reflected < 2000) {
				moveFwd(1000000);
			}
			else
			{
				moveStop();
				return;
			}
			
		}
	}
}

void avoidObstacle() {
	cout << "starting obstacel detection..." << endl;
	obstacleAvoidenceLeft();
	obstacleAvoidenceFwd();
	obstacleAvoidenceRight();
	obstacleAvoidenceFwd();
	obstacleAvoidenceRight();
	obstacleAvoidenceEnding();
}



void driveByLine() {

	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	if (BP.get_voltage_battery() >= 9) {
		while (true) {
			if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
					cout << "searching line..." << endl;

					if (Ultrasonic2.cm > 10) {
						cout << Light3.reflected << endl;
						if (Light3.reflected >= 2000 && Light3.reflected <= 2200) {
							cout << "half" << endl;
							moveFwd(100000);
							//rechtdoor
						}
						else if (Light3.reflected > 1800 && Light3.reflected < 2000) {
							cout << "wit" << endl;
							moveLeft(100000);
							//als ie het wit in gaat
						}
						else if (Light3.reflected > 2200) {
							moveRight(100000);
							cout << "zwart" << endl;
							//als ie het zwart in gaat
						}
						cout << "\033[2J\033[1;1H";
					}
					else {
						avoidObstacle();
					}
				}
				else
				{
					cout << "Can't read ultra-red sensor..." << endl;
				}
			}
			else {
				cout << "Can't read ultrasonic sensor..." << endl;
			}
		}
		usleep(100000);
	}
	else {
		cout << "Battery voltage is: " << BP.get_voltage_battery() << ". This is to low to continue..." << endl;
	}
	cout << "Robot stopped..." << endl;
	
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

