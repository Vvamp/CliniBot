#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

void searchLine(const int & lastDirection) {
	if (lastDirection == -1) {
		//ga rechts zoeken als laatste bocht links was
		BP.set_motor_position_relative(PORT_B, -90);
		BP.set_motor_position_relative(PORT_C, 90);
	}

	if (lastDirection == 1) {
		//ga links zoeken als laatste bocht rechts was
		BP.set_motor_position_relative(PORT_B, 90);
		BP.set_motor_position_relative(PORT_C, -90);
	}

	cout << " Searching - ";
}

void moveStop() {
	BP.set_motor_power(PORT_B, 0);
	BP.set_motor_power(PORT_C, 0);
	// Zet stroom van poort B en C op 0, waardoor de robot stopt.

	cout << " Stopped - ";
}

void moveFwd(int &numbersLeft, int &numbersRight) {
	BP.set_motor_dps(PORT_B, 360);
	BP.set_motor_dps(PORT_C, 360);
	numbersLeft = 0;
	numbersRight = 0;
	// Draai de motor op port B en C 360 graden
	cout << " Forward - ";

}

void moveLeft(int &lastDirection, int &numbersLeft) {
	if (numbersLeft < 3)
	{
		BP.set_motor_position_relative(PORT_B, 45);
		BP.set_motor_position_relative(PORT_C, -45);
		lastDirection = -1;
		numbersLeft++;
	}
	else
	{
		numbersLeft = 0;
		searchLine(lastDirection);
	}
	 
	/*BP.set_motor_dps(PORT_B, 360);
	BP.set_motor_dps(PORT_C, 130);*/
	// Draai het wiel op port B 270 graden en de wiel op port C -270 graden

	cout << " Left - ";

}

void moveRight(int &lastDirection, int &numbersRight) {
	if (numbersRight < 3) {
		BP.set_motor_position_relative(PORT_B, -45);
		BP.set_motor_position_relative(PORT_C, 45);
		numbersRight++;
		lastDirection = 1;
	}
	else
	{
		numbersRight = 0;
		searchLine(lastDirection);
	}
	 
	/*BP.set_motor_dps(PORT_B, 130);
	BP.set_motor_dps(PORT_C, 360);*/
	// Draai het wiel op port B -270 graden en de wiel op port C 270 graden

	cout << " Right - ";

}

void moveBack() {
	BP.set_motor_dps(PORT_B, -360);
	BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden

	cout << " Back - ";

}



int averageValues(const int red, const int green, const int blue) {
	int average = (red + green + blue) / 3;
	return average;
}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);

	sensor_color_t      Color1;

	int lastDirection = 0;
	int numbersRight = 0;
	int numbersLeft = 0;
	int average = 0;

	while (true) {
		
		if (BP.get_sensor(PORT_1, Color1) == 0) {
			average = averageValues((int)Color1.reflected_red, (int)Color1.reflected_green, (int)Color1.reflected_blue);
			if (average >= 450 && average <= 490) {
				moveFwd(numbersLeft,numbersRight);
			}
			else if (average > 490){
				moveLeft(lastDirection,numbersLeft);
			}
			else if (average < 310){
				moveRight(lastDirection,numbersRight);
			}
			else
			{
				searchLine(lastDirection);
			}
			
			cout << "Average = " << average << endl;
			/*cout << "Color sensor (S1): detected  " << (int)Color1.color;
			cout << " R:" << setw(4) << Color1.reflected_red << endl;
			cout << " G:" << setw(4) << Color1.reflected_green;
			cout << " B:" << setw(4) << Color1.reflected_blue;
			cout << " A:" << setw(4) << Color1.ambient << "]" << endl;*/
		}
		usleep(1000000);//slaap een seconde (1 usleep = 1 miljoenste van een seconde)
	}
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}