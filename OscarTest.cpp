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
	return;
}

void moveFwd() {
	BP.set_motor_dps(PORT_B, 180);
	BP.set_motor_dps(PORT_C, 180);
	// Draai de motor op port B en C 360 graden
	cout << " Forward - ";
	return;
}

void moveLeft() {
	
		BP.set_motor_dps(PORT_B, 80);
		BP.set_motor_dps(PORT_C, -80);
		/*BP.set_motor_position_relative(PORT_B, 45);
		BP.set_motor_position_relative(PORT_C, -45);*/

	cout << " Left - ";
	return;
}

void moveRight() {
		BP.set_motor_dps(PORT_B, -80);
		BP.set_motor_dps(PORT_C, 80);
		/*BP.set_motor_position_relative(PORT_B, -45);
		BP.set_motor_position_relative(PORT_C, 45);*/

	cout << " Right - ";
	return;
}

void moveBack() {
	BP.set_motor_dps(PORT_B, -360);
	BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden

	cout << " Back - ";
	return;
}

void findNewPath() {

	sensor_ultrasonic_t Ultrasonic2;
	cout << "searching path" << endl;

	int counterStraight = 0;
	int counterLeft = 0;
	int counterRight = 0;
	//links zoeken
	while (true) {

		if (Ultrasonic2.cm < 10) {
			moveLeft();
			counterLeft++;
			cout << "turning left for search ";
			usleep(1000000);
		}
		else if (counterStraight != counterLeft)
		{
			moveFwd();
			counterStraight++;
			cout << "driving straight for search";
			usleep(1000000);
		}
		else if (counterRight != counterLeft *2) {
			moveRight();
			counterRight++;
			cout << "driving right for search";
			counterStraight = 0;
			usleep(1000000);
		}
		else if (counterStraight != counterLeft){
			moveFwd();
			counterStraight++;
			cout << "driving left for search";
			usleep(1000000);
		}
		else {
			break;
		}
	}
	return;
	
}

void driveByLine() {

	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	sensor_color_t      Color1;
	sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light3;

	int measurement = 0;

	while (true) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			cout << Ultrasonic2.cm << endl;
			if (Ultrasonic2.cm > 10) {


				if (BP.get_sensor(PORT_3, Light3) == 0) {
					measurement = Light3.reflected;
					if (measurement >= 1900 && measurement <= 2300) {
						moveFwd();
						//rechtdoor
					}
					else if (measurement > 1800 && measurement < 1900) {
						moveLeft();
						//als ie het wit in gaat
					}
					else if (measurement > 2300) {
						moveRight();
						//als ie het zwart in gaat
					}
					usleep(250000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)
					
				}
			}
			else
			{
				findNewPath();
			}
		}
	}
	



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

	sensor_color_t      Color1;
	sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light3;

	driveByLine();

	/*int average = 0;*/
	

	
		/*if (BP.get_sensor(PORT_1, Color1) == 0) {
			average = averageValues((int)Color1.reflected_red, (int)Color1.reflected_green, (int)Color1.reflected_blue);
			if (average >=240  && average <= 320) {
				movefwd();
			}
			else if (average > 150 && average < 240){
				moveleft();
			}
			else if (average > 320 && average < 600){
				moveright();
			}*/
			
			
			//cout << "Average = " << average << endl;
			/*cout << "Color sensor (S1): detected  " << (int)Color1.color;
			cout << " R:" << setw(4) << Color1.reflected_red << endl;
			cout << " G:" << setw(4) << Color1.reflected_green;
			cout << " B:" << setw(4) << Color1.reflected_blue;
			cout << " A:" << setw(4) << Color1.ambient << "]" << endl;*/
		/*}*/
		
	
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}