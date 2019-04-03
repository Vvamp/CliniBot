#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <string>       // Include strings
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision
#include <fstream>      // for creating a log files
#include <vector>

using namespace std;

BrickPi3 BP;
sensor_ultrasonic_t Ultrasonic2;
sensor_light_t Light3;

void exit_signal_handler(int signo);

int calibrateRight(){
    cout << "-REACHED RIGHT!" << endl;
    usleep(1000000);

    int calbRight = 0;

    cout << "-- Started calibration of right" << endl;

    for(unsigned int i=1; i <= 5; i++){
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			usleep(125000);
            calbRight += Light3.reflected;
            cout << "- Calibrating right: " << i << " sec, value: "<< Light3.reflected << endl;
            usleep(500000);
		}
        
    };

    cout << endl;
    return calbRight / 5;

}

int calibrateFoward(){

    int calbFoward = 0;

    cout << "-- Started calibration of foward" << endl;

    BP.set_motor_position_relative(PORT_C, 90);
    BP.set_motor_position_relative(PORT_B, -90);

    usleep(3000000);

    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);

    for(unsigned int i=1; i <= 5; i++){
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			usleep(125000);
            calbFoward += Light3.reflected;
            cout << "- Calibrating foward: " << i << " sec, value: "<< Light3.reflected << endl;
            usleep(500000);
		}
        
    };

    BP.set_motor_position_relative(PORT_C, -90);
    BP.set_motor_position_relative(PORT_B, 90);

    usleep(3000000);

    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);

    cout << endl;
    return calbFoward / 5;

}

int calibrateLeft(){

    int calbLeft = 0;

    cout << "-- Started calibration of left" << endl;

    BP.set_motor_position_relative(PORT_C, -90);
    BP.set_motor_position_relative(PORT_B, 90);

    usleep(3000000);

    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);

    for(unsigned int i=1; i <= 5; i++){
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			usleep(125000);
            calbLeft += Light3.reflected;
            cout << "- Calibrating left: " << i << " sec, value: "<< Light3.reflected << endl;
            usleep(500000);
		}
        
    };

    BP.set_motor_position_relative(PORT_C, 90);
    BP.set_motor_position_relative(PORT_B, -90);

    usleep(3000000);

    BP.set_motor_power(PORT_C, 0);
    BP.set_motor_power(PORT_B, 0);

    cout << endl;
    return calbLeft / 5;

}

//Function to move robot (left, right)
void moveBot(int measurement, int valueLeft, int valueRight, string botStatus) {
	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor

    cout << "\033[2J\033[1;1H"; //Clear screen
    cout << "CUPBOT ==============" << endl;

    cout << endl << "-BOT STATUS:" << endl;
    cout << " " << botStatus << endl;

    cout << endl << "-INFORMATION:" << endl;
    cout << " Battery voltage: " << BP.get_voltage_battery() << endl;
    cout << " 9v voltage: " << BP.get_voltage_9v() << endl;
    cout << " 5v voltage: " << BP.get_voltage_5v() << endl;
    cout << " 3.3v voltage: " << BP.get_voltage_3v3() << endl;

    cout << endl << "-MOTOR VALUES: " << endl;
    cout << " Left: " << valueLeft << "v" << endl;
    cout << " Right: " << valueRight << "v" << endl;

    cout << endl << "-DETECTION:" << endl ;
    cout << " LIGHT REFLECTION: " << measurement << endl;
    cout << " ULTRASONIC: " << endl;

    cout << endl << "=====================" << endl;

}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	int average = 0;
	int measurement = 0;

    // ofstream logfile;

    cout << "--Starting calb" << endl;
    usleep(1000000);

    int getRight = calibrateRight();
    int getFoward = calibrateFoward();
    int getLeft = calibrateLeft();

    cout << "-- Successfully calibrated with values:" << endl;
    cout << "[L:" << getLeft << "] [F:" << getFoward << "] [R:" << getRight << "]" << endl; 
    usleep(3000000);

	while (true) {

        int logUpdate = 0;

        // logfile.open("log-new.txt", ios::app);

        cout << "--CHECK" << endl;

		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {

            cout << "--CHECK 1" << endl;

			if (Ultrasonic2.cm > 10) {

                cout << "--CHECK 2" << endl;

                if (BP.get_sensor(PORT_3, Light3) == 0) {

                    cout << "--CHECK 3" << endl;
                    
                    measurement = Light3.reflected;

                    cout << "-MEASUREMENT: " << measurement << endl;
                    
                    if (measurement >= getFoward && measurement <= getRight) {
                        cout << "--CHECK F" << endl;
                        // moveBot(measurement, 50, 50, "Moving forward"); //Forward
                        moveBot(measurement, 50, 50, "Moving forward"); //Forward
                        cout << "Left" << endl;
                    }
                    else if (measurement > getLeft && measurement < getFoward) {
                        cout << "--CHECK L" << endl;
                        // moveBot(measurement, 5, 50, "Moving left"); //Left
                        moveBot(measurement, 5, 50, "Moving left"); //Left
                        cout << "Foward" << endl;
                    }
                    else if (measurement > getRight) {
                        cout << "--CHECK R" << endl;
                        // moveBot(measurement, 50, 5, "Moving right"); //Right
                        moveBot(measurement, 50, 5, "Moving right"); //Right
                        cout << "Right" << endl;
                        // logfile << "Moving right" << " =[ " << 0 << "," << 0 << " ]\n";
                        // logfile.close();
                    }
                }
			}
			else{
                cout << "--CHECK S" << endl;
				moveBot(measurement, 0, 0, "Stopped moving");
                cout << "Stopped moving" << endl;
                // logfile << "Stopped moving" << " =[ " << 0 << "," << 0 << " ]\n";
                // logUpdate == 10 ? (logfile << "Stopped moving" << " =[ " << 0 << ", " << 0 << "]\n") : cout << endl;
                // logUpdate == 10 ? logUpdate = 0 : logUpdate++;
                // logfile.close();
                
			}

            usleep(1000000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)

		}
		else {
			moveBot(measurement, 0, 0, "Ultra sonic not found");
            cout << "--CHECK U" << endl;
            cout << "Ultra not found" << endl;
            // logfile << "Ultra sonic not found" << " =[ " << 0 << "," << 0 << " ]\n";
            // logUpdate == 10 ? (logfile << "Ultra sonic not found" << " =[ " << 0 << ", " << 0 << "]\n") : cout << endl;
            // logUpdate == 10 ? logUpdate = 0 : logUpdate++;
            // logfile.close();
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