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

vector<int> calibrateSensor(){

    //Set time of calc in seconds
    int setTime = 0;
    int black = 0;
    int left, foward, right;
    vector<int> calbValues;
    string tmp;

    cout << "-- Enter time of calibration in seconds: ";
    cin >> setTime;

    for(unsigned i=0; i <= setTime; i++){
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			usleep(125000);
				black += Light3.reflected;
				cout << "-- Calibrated: " << i << " sec" << endl;
				sleep(1);
		}
        
    };

    //Calc values
    black = black / setTime;

    right = black;
    foward = black - 200;
    left = black - 400;

    calbValues.push_back(left);
    calbValues.push_back(foward);
    calbValues.push_back(right);

    cout << "-- Successfully calibrated black, values:" << endl;

    return calbValues;
};

//Function to move robot (left, right)
void moveBot(int measurement, int valueLeft, int valueRight, const string botStatus, int calbLeft, int calbFowd, int calbRight) {
	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor

    cout << "\033[2J\033[1;1H"; //Clear screen
    cout << "CLINIBOT ============" << endl;

    cout << endl << "-BOT STATUS:" << endl;
    cout << " " << botStatus << endl;

    cout << endl << "-CALIBRATION:" << endl;
    cout << "Left: " << calbLeft << endl;  
    cout << "Foward: " << calbFowd << endl;  
    cout << "Right: " << calbRight << endl;  

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

    measurement = Light3.reflected;
    vector<int> calbValues = calibrateSensor();
    int getLeft = calbValues[0];
    int getFoward = calbValues[1];
    int getRight = calbValues[2];

    cout << "Left: [" << getLeft << "] Foward: [" << getFoward << "] Right: [" << getRight << "]" << endl;
    sleep(1);

	while (true) {

        int logUpdate = 0;

        // logfile.open("log-new.txt", ios::app);

		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (Ultrasonic2.cm > 10) {

                // if(!isCrossing()){

                    if (BP.get_sensor(PORT_3, Light3) == 0) {
                        
                        if (measurement >= getFoward && measurement <= getRight) {
                            // moveBot(measurement, 50, 50, "Moving forward"); //Forward
                            moveBot(measurement, 50, 50, "Moving forward", getLeft, getFoward, getRight); //Forward
                        }
                        if (measurement > getLeft && measurement < getFoward) {
                            // moveBot(measurement, 5, 50, "Moving left"); //Left
                            moveBot(measurement, 5, 50, "Moving left", getLeft, getFoward, getRight); //Right
                        }
                        else if (measurement > getRight) {
                            // moveBot(measurement, 50, 5, "Moving right"); //Right
                            moveBot(measurement, 50, 5, "Moving right", getLeft, getFoward, getRight); //Right
                            // logfile << "Moving right" << " =[ " << 0 << "," << 0 << " ]\n";
                            // logfile.close();
                        }
                    }
                // } else {
                //     askDirection();
                // }
			}
			else
			{
				moveBot(measurement, 0, 0, "Stopped moving", getLeft, getFoward, getRight);
                // logfile << "Stopped moving" << " =[ " << 0 << "," << 0 << " ]\n";
                // logUpdate == 10 ? (logfile << "Stopped moving" << " =[ " << 0 << ", " << 0 << "]\n") : cout << endl;
                // logUpdate == 10 ? logUpdate = 0 : logUpdate++;
                // logfile.close();
                
			}

            usleep(50000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)

		}
		else
		{
			moveBot(measurement, 0, 0, "Ultra sonic not found", getLeft, getFoward, getRight);
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