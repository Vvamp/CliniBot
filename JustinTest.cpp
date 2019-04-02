#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <string>       // Include strings
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision
#include <fstream>      // for creating a log files

using namespace std;

BrickPi3 BP;
sensor_ultrasonic_t Ultrasonic2;
sensor_light_t Light3;

void exit_signal_handler(int signo);

int calibrateSensor(){

    //Set time of calc in seconds
    int setTime = 0;
    int black = 0;
    int left, foward, right = 0;
    string tmp;

    cout << "-- Enter time of calc in seconds: ";
    cin >> setTime;

    for(unsigned i=0; i >= setTime; i++){
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

    cout << "-- Successfully calibrated black, values:" << endl;
    cout << "Right: " << right << "Foward: " << foward << "Left: " << left << endl;

    return left, foward, right;
};

//Function to move robot (left, right)
void moveBot(const int measurement, const int valueLeft, const int valueRight, string botStatus) {
	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor

    cout << "\033[2J\033[1;1H"; //Clear screen
    cout << "CLINIBOT ============" << endl;

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

    measurement = Light3.reflected;
    int left, foward, right = calibrateSensor();

	while (true) {

        int logUpdate = 0;

        // logfile.open("log-new.txt", ios::app);

		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (Ultrasonic2.cm > 10) {

                // if(!isCrossing()){

                    if (BP.get_sensor(PORT_3, Light3) == 0) {
                        
                        if (measurement >= 2000 && measurement <= 2200) {
                            // moveBot(measurement, 50, 50, "Moving forward"); //Forward
                            moveBot(measurement, 0, 0, "Moving forward"); //Forward
                        }
                        if (measurement > 1800 && measurement < 2000) {
                            // moveBot(measurement, 5, 50, "Moving left"); //Left
                            moveBot(measurement, 0, 0, "Moving left"); //Right
                        }
                        else if (measurement > 2200) {
                            // moveBot(measurement, 50, 5, "Moving right"); //Right
                            moveBot(measurement, 0, 0, "Moving right"); //Right
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
				moveBot(measurement, 0, 0, "Stopped moving");
                // logfile << "Stopped moving" << " =[ " << 0 << "," << 0 << " ]\n";
                // logUpdate == 10 ? (logfile << "Stopped moving" << " =[ " << 0 << ", " << 0 << "]\n") : cout << endl;
                // logUpdate == 10 ? logUpdate = 0 : logUpdate++;
                // logfile.close();
                
			}

            usleep(50000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)

		}
		else
		{
			moveBot(measurement, 0, 0, "Ultra sonic not found");
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