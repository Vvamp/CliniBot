#include <iostream>      // Basic IO
#include <unistd.h>      // for usleep and sleep(for linux)
#include <signal.h>      // for catching exit signals
#include "BrickPi3.h"    // BrickPi3 Header file - used to control the ROBO-MAN
#include <string>        // String variables
#include <vector>       // Vector variables
#include <cstdlib>  // random numbers

// Using statements
using std::cout;
using std::endl;
using std::getline;
using std::cin;
using std::string;
using std::to_string;
using std::vector;

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

//Sensor definitions
sensor_light_t      Light3; //RGB Light sensor
sensor_color_t      Color1; //Infrared sensor
sensor_ultrasonic_t Ultrasonic2; //Ultrasonic sensor
sensor_touch_t      Touch4;		// Touch sensor

const bool enableDebug = true;

bool isReversing = false;
int blackHigh = 0;
int blackLow = 1000000;
int whiteHigh = 0;
int whiteLow = 10000000;
int RGBBlackLow = 1000000;
int RGBBlackHigh = 0;
int RGBWhiteLow = 1000000;
int RGBWhiteHigh = 0;

bool buttonPressed(){

	unsigned int i = 0;
	while (i <= 4000) {
		i++;
		if (BP.get_sensor(PORT_4, Touch4) == 0) {
			if(Touch4.pressed){
				return true;
			}
		}

	}
	return false;
}

void Calibration() {
	int time = 4000;
	int average;
	char input;

	while (true) {
		if(!buttonPressed()){
			cout << "*** DO NOT FORGET THE CUP, PLEASE!!! ***" << endl;
		}
		if (blackHigh == 0) {
			cout << "place the robot on black and press s + enter to start" << endl;
			cin >> input;
			if (input == 's') {
				for (int i = 0; i < time; i++) {
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected > blackHigh) {
							blackHigh = Light3.reflected;
						}
						if (Light3.reflected < blackLow) {
							blackLow = Light3.reflected;
						}
					}
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBBlackHigh) {
							RGBBlackHigh = average;
						}
						if (average < RGBBlackLow) {
							RGBBlackLow = average;
						}
					}
					usleep(1000);
				}
			}
			input = ' ';
			cout << "black high value is: " << blackHigh << endl;
			cout << "black low value is: " << blackLow << endl;
			cout << "RGB black high value is: " << RGBBlackHigh << endl;
			cout << "RGB black low value is: " << RGBBlackLow << endl;
		}
		if (whiteHigh == 0) {
			cout << "place the robot on white and press s + enter to start" << endl;
			cin >> input;
			if (input == 's') {
				for (int a = 0; a < time; a++) {
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected > whiteHigh) {
							whiteHigh = Light3.reflected;
						}
						if (Light3.reflected < whiteLow) {
							whiteLow = Light3.reflected;
						}
					}
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBWhiteHigh) {
							RGBWhiteHigh = average;
						}
						if (average < RGBWhiteLow) {
							RGBWhiteLow = average;
						}
					}
					usleep(1000);
				}
			}
			input = ' ';
			cout << "white high value is: " << whiteHigh << endl;
			cout << "white low value is: " << whiteLow << endl;
			cout << "RGB white high value is: " << RGBWhiteHigh << endl;
			cout << "RGB white low value is: " << RGBWhiteLow << endl;
		}
		if (blackLow > whiteHigh && RGBWhiteLow > RGBBlackHigh) {
			break;
		}
		else {
			cout << "calibration went wrong... starting again." << endl;
			blackLow = 1000000;
			blackHigh = 0;
			whiteLow = 1000000;
			whiteHigh = 0;
			RGBBlackLow = 1000000;
			RGBBlackHigh = 0;
			RGBWhiteLow = 1000000;
			RGBWhiteHigh = 0;
			input = ' ';
		}
	}
	cout << "calibration complete..." << endl;
	return;
}

enum direction{
    left,
    right,
    forward,
    backwards,
};

struct movement{
    direction dir;    //Direction robot went
};

vector<movement> pathLogger;
void eHandler(int s);
void controlGrid();


//-- Movement functions
// Stop the robot by setting the motor power to '0'
void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);

    // Zet stroom van poort B en C op 0, waardoor de robot stopt.
}

void moveBot(const int measurement, const int valueLeft, const int valueRight) {

	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor
	usleep(50000);
/*
    cout << "\033[2J\033[1;1H"; //Clear screen
    cout << "CLINIBOT ============" << endl;



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
*/
}

void moveLeft() {
    int speed = 80;


    BP.set_motor_dps(PORT_B, speed);
    BP.set_motor_dps(PORT_C, -speed);
}

void moveRight() {
    int speed = 80;


	BP.set_motor_dps(PORT_B, -speed);
	BP.set_motor_dps(PORT_C, speed);
}

void moveFwd() {
    int speed = 420;


    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, speed);
}

void moveBack() {
    int speed = -420;


    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, speed);
}

void moveFwd(const int & time) {
    int speed = 25;


	BP.set_motor_power(PORT_B, speed);
	BP.set_motor_power(PORT_C, speed);
	usleep(time);
}

void moveLeft(const int & time) {

	BP.set_motor_power(PORT_B, 25);
	BP.set_motor_power(PORT_C, -25);
	usleep(time);
}

void moveRight(const int & time) {

	BP.set_motor_power(PORT_B, -25);
	BP.set_motor_power(PORT_C, 25);
	usleep(time);
}

void moveBack(const int &time) {

	BP.set_motor_power(PORT_B, -25);
	BP.set_motor_power(PORT_C, -25);
	usleep(time);
}

void turnLeft(){
    int speed = 420;

    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, -speed);
}

void turnRight(){
    int speed = 420;

    BP.set_motor_position_relative(PORT_B, -speed);
    BP.set_motor_position_relative(PORT_C, speed);
}


//-Eye functions
const int lookAngle = 105;

// Turn the eyes left
void lookLeft(){
    BP.set_motor_position_relative(PORT_D, lookAngle);
}

// Turn the eyes right
void lookRight(){
    BP.set_motor_position_relative(PORT_D, -lookAngle);
}


//- Control functions
void reverseBot() {
	cout << "Reversing..." << endl;
	isReversing = true;
	//rotate 180 degrees
	moveRight(1000000);
	moveStop();
	while (true) {
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			if (Light3.reflected < blackLow) {
				moveRight(100000);
			}
			else {
				moveStop();
				usleep(1000000);
				cout << "end of reversing" << endl;
				break;
			}
		}
	}
	return controlGrid();
}
// Check if there is a regular crossing(both sensors would be black)
bool isCrossing(){
    int measurement = 0;
    bool s1 = false;
    bool s2 = false;

    // Check if RGB sensor reads black
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        /*if(enableDebug){
        cout << "rgb: " << measurement << endl;
    }*/
        if(measurement >=RGBBlackLow && measurement < RGBBlackHigh){
            s1 = true;
        }
    }


    // Check if IR sensor reads black
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
    /*    if(enableDebug){
        cout << "ir: " << measurement << endl;
    }*/
        if(measurement >= blackLow){
            s2 = true;
        }
    }

    // if both read black it's a crossing
    if(s1 && s2){
        cout << " > Crossing detected" << endl;
        return true;
    }else{
        return false;
    }
}

// Check if there is an obstacle in FRONT of the robot
bool obstacleDetected(){
    int obstacleDetectionDistance = 25;
    int timeout = 0;
    while(true){
        timeout++;
        if(timeout > 4000){
            break;
        }
        if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
                if(Ultrasonic2.cm <= obstacleDetectionDistance){
                    return true;

                }

        }
    }
    //if(enableDebug){
    //cout << endl << "cm: " << Ultrasonic2.cm << endl;
    //}
    return false;

}

// Check if there is a line
bool lineDetected(){
    int measurement = 0;
//    if(enableDebug){
//    cout << endl << endl << "checking line";
//    }
    bool s1 = false;
    bool s2 = false;
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        //if(enableDebug){
    //   cout << endl << "measured RGB: " << measurement << " borders: 2000 <= x < 2700" << endl;
    //    }
        if(measurement >=whiteHigh && measurement < blackHigh){
            s1 = true;
        }else{
            s1 = false;
        }
    }
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
    //    if(enableDebug){
    //    cout << endl << "m: " << measurement << " borders: 200 <= x < 400" << endl;
    //    }
        if(measurement < RGBBlackHigh && measurement > RGBWhiteHigh){
            s2 = true;
        }else{
            s2 = false;

        }
    }
    if(s1 == true || s2 == true){
        return true;
    }else{
        return false;
    }

}
void turnAround(){
    while(!lineDetected){
        moveRight(2);
    }
    cout << "Found line";
    moveStop();

}

// Check a crossing
void checkGrid(){
    const unsigned int routesToCheck = 3;
    const int sleepTime = 2;

	int stepsLeft = 0;
	int stepsRight = 0;
	int average = 0;

    vector<bool> values = {false, false, false};
    bool driveRequired = false;

    if(!isReversing){
    for(unsigned int i = 0; i < routesToCheck; i++){
        switch(i){

            // Route forward check
            case 0:
                if(enableDebug){
                    cout << endl << "Forward: ";
                    cout << "Checking route: " << i;
                }

                // Check if there is an object
                if(!obstacleDetected()){
                    if(enableDebug){
                    cout << "...clear!" << endl;
                    cout << "checking if path...";
                    }

                    // Go to forward-line
                    moveFwd();
                    sleep(sleepTime);
                    moveStop();

                    // check if there is a line
                    if(!lineDetected()){
                        if(enableDebug){
                        cout << "no path" << endl;
                        }
                    }else{
                        if(enableDebug){
                        cout << "path found" << endl;
                        }
                        values[0] = true;
                    }

                    // Go back to original location
                    moveBack();
                    sleep(sleepTime);

                }else{
                    if(enableDebug){
                    cout << "...blocked!" << endl;
                    }
                }
            break;

            case 1:
            if(enableDebug){
            cout << endl << "Left: ";
            cout << "Checking route: " << i;
            }

            // Turn eyes left and check if there is an object
            lookLeft();
            sleep(sleepTime);
            if(!obstacleDetected()){
                if(enableDebug){
                cout << "...clear!" << endl;
                }
                lookRight();                    // Reset eyes
                sleep(sleepTime);               // give the eyes time to look
                if(enableDebug){
                cout << "checking if path...";
                }
                // Bring wheels to crossing center and turn left
                moveFwd(1000000);
				moveLeft(1000000);
				stepsLeft += 1000000;
				while (true) {
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected < blackLow) {
							if (stepsLeft > 3200000) {
								cout << "no path found" << endl;
								moveStop();
								usleep(2000000);
								break;
							}
							moveLeft(100000);
							moveStop();
							stepsLeft += 100000;
							usleep(50000);
						}
						else {
							cout <<"steps: " << stepsLeft << endl;
							cout << "path found" << endl;
							values[1] = true;
							moveStop();
							usleep(2000000);
							break;
						}
					}
				}


                // Turn back to face the middle-line and move back to original position
				moveRight(stepsLeft);
				stepsRight += stepsLeft;
				stepsLeft += 200000;
				while (true) {
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBBlackHigh)
						{
							if (stepsRight <= stepsLeft) {
								moveLeft(200000);
								moveStop();
								stepsRight = 0;
								stepsLeft = 0;
								usleep(50000);
								break;
							}
							moveRight(100000);
							moveStop();
							stepsRight += 100000;
							usleep(50000);
						}
						else {
							moveLeft(200000);
							moveStop();
							stepsRight = 0;
							stepsLeft = 0;
							usleep(50000);
							break;
						}
					}
				}
				while (true) {
					if (!isCrossing()) {
						moveBack(100000);
						moveStop();
						usleep(50000);
					}
					else {
						moveStop();
						usleep(50000);
						break;
					}
				}

            }else{
                if(enableDebug){
                cout << "...blocked!" << endl;
                }
                lookRight();            // Reset eyes
                sleep(sleepTime);

            }
            break;
            case 2:
                if(enableDebug){
                cout << endl << "Right: ";
                cout << "Checking route: " << i << endl;
                }

                // Turn eyes right and check if there is an object
                lookRight();
                sleep(sleepTime);

                if(!obstacleDetected()){
                    if(enableDebug){
                    cout << "...clear!" << endl;
                    }
                    lookLeft();         // Reset eyes
                    sleep(sleepTime);

                    if(enableDebug){
                    cout << "checking if path...";
                    }

                    // Move wheels to center and turn right
                    moveFwd(1000000);
					moveRight(1000000);
					stepsRight += 1000000;
					while (true) {
						if (BP.get_sensor(PORT_3, Light3) == 0) {
							if (Light3.reflected < blackLow) {
								if (stepsRight >= 2600000) {
									cout << "no path found" << endl;
									moveStop();
									usleep(2000000);
									break;
								}
								moveRight(100000);
								moveStop();
								stepsRight += 100000;
								usleep(50000);
							}
							else {
								cout << "steps: " << stepsRight << endl;
								cout << "path found" << endl;
								values[2] = true;
								moveStop();
								usleep(2000000);
								break;
							}
						}
					}

                    // Turn back to face the middle-line and go back to original position
					moveLeft(stepsRight);
					stepsLeft += stepsRight;
					stepsRight += 200000;
					while (true) {
						if (BP.get_sensor(PORT_3, Light3) == 0) {
							if (Light3.reflected < whiteHigh){
								if (stepsLeft <= stepsRight) {
									moveRight(200000);
									moveStop();
									usleep(50000);
									break;
								}
								moveLeft(100000);
								moveStop();
								stepsLeft+=100000;
								usleep(50000);
							}
							else {
								moveStop();
								usleep(50000);
								break;
							}
						}
					}
					while (true) {
						if (!isCrossing()) {
							moveBack(100000);
							moveStop();
							usleep(50000);
						}
						else {
							moveStop();
							usleep(50000);
							break;
						}
					}
                }
				else{
                    if(enableDebug){
                    cout << "...blocked!" << endl;
                    }
                    lookLeft();
                    sleep(sleepTime);
                }
            break;
                default:
                if(enableDebug){
                cout << endl << "[ERROR]";
                }
                exit(-2);
            break;
        }


    }

    moveStop();
    cout << "Possible paths: ";
    if(!values[0] && !values[1] && !values[2]){
        movement currentMovement;
        cout << "None" << endl;
        cout << "Moving back" << endl; //rotate 180 and follow line
        currentMovement.dir = backwards;
        pathLogger.push_back(currentMovement);
        turnRight();
        sleep(sleepTime);
        turnRight();
        sleep(sleepTime);
        return;

    }else{
        if(values[0]){
            cout << "Forward ";
        }
        if(values[1]){
            cout << "Left ";
        }
        if(values[2]){
            cout << "Right ";
        }
    }
    cout << endl;

    string uinDirection;
    int randomDirectionChooser = 0;
    while(true){
        movement currentMovement;
        srand((unsigned) time(0));
        randomDirectionChooser = rand() % 3 + 1;
        //cout << randomDirectionChooser << endl;
        switch(randomDirectionChooser){
            case 1:
                uinDirection =  "left";
                break;
            case 2:
                uinDirection = "right";
                break;
            case 3:
                uinDirection = "forward";
                break;
            default:
                uinDirection = "backwards";
                cout << "ERROR IN SHIT" << endl;
                break;
        }

        if(uinDirection == "left"){
            if(values[1]){
				currentMovement.dir = left;
				pathLogger.push_back(currentMovement);
                moveFwd(1000000);
                turnLeft();
                sleep(sleepTime);
                if (BP.get_sensor(PORT_3, Light3) == 0) {
                    if (Light3.reflected >= whiteHigh && Light3.reflected <= blackLow) {
						moveLeft(100000);
                    }
                }
                return;
            }


        }else if(uinDirection == "right"){
            if(values[2]){
				currentMovement.dir = right;
				pathLogger.push_back(currentMovement);
                moveFwd(1000000);
                turnRight();
                sleep(sleepTime);
                if (BP.get_sensor(PORT_1, Color1) == 0) {
					average = (Color1.reflected_blue + Color1.reflected_red + Color1.reflected_green) / 3;
                    if (average <= RGBBlackLow) {
						moveRight(100000);
                    }
                }
                return;

            }
        }else if(uinDirection == "forward"){
            if(values[0]){
                currentMovement.dir = forward;
                pathLogger.push_back(currentMovement);


                moveFwd(1500000);
                return;

            }
        }else{
            cout << "Unknown direction" << endl;
        }
    }
    }else{
        if(pathLogger[pathLogger.size() -1].dir == left){
            pathLogger.pop_back();
			moveFwd(500000);

			moveRight(1000000);
			moveStop();
			while (true) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
					if (Light3.reflected < blackLow) {
						moveRight(100000);
					}
					else {
						moveStop();
						usleep(1000000);
						break;
					}
				}
			}

        }else if(pathLogger[pathLogger.size() - 1].dir == right){
            pathLogger.pop_back();
			moveFwd(500000);

			moveLeft(1000000);
			moveStop();
			while (true) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
					if (Light3.reflected < blackLow) {
						moveLeft(100000);
					}
					else {
						moveStop();
						usleep(1000000);
						break;
					}
				}
			}
		}else if(pathLogger[pathLogger.size() -1].dir == forward){
            pathLogger.pop_back();
            moveFwd(1500000);
            moveFwd();
            sleep(sleepTime);

        }else{
            pathLogger.pop_back();
            turnRight();
            sleep(sleepTime);
            turnRight();
            sleep(sleepTime);
             }
    }
}




//- Main functions
// Let CliniBot follow a grid
void controlGrid(){
	// Check if the battery is still sufficiently charged, else shutdown
    if (BP.get_voltage_battery() >= 9) {
        while(true){
			cout << "Size of history: " << pathLogger.size() << endl;
			if(!isReversing){
				if(!buttonPressed()){
				cout << "Button is not pressed" << endl;
				return reverseBot();
				}
			}
            if(isReversing){
                if(pathLogger.size() == 0){
                    //isReversing = false;
					cout << "size = 0" << endl;
					moveStop();
                    return;
                }
            }

			if (BP.get_sensor(PORT_3, Light3) == 0) {
				if (Light3.reflected >= whiteHigh && Light3.reflected <= blackLow) { //2200 - 2300
                    moveBot(Light3.reflected, 30, 30);
					//rechtdoor
				}
				else if (Light3.reflected >= whiteLow && Light3.reflected < whiteHigh) { //1850 - 2200
                    moveBot(Light3.reflected, -30, 30);

					//als ie het wit in gaat
				}
				else if (Light3.reflected > blackLow) { // > 2300
                    if(isCrossing()){
                        moveStop();
                        sleep(2);
                        checkGrid();
                    }
					moveBot(Light3.reflected, 30, -30);
					//als ie het zwart in gaat
				}


			}
			else
			{
				cout << "ERROR: Can't read ultra-red sensor..." << endl;
			}


        }

	}
	else {
		cout << "Battery voltage is: " << BP.get_voltage_battery() << ". This is too low to continue..." << endl;
	}
	cout << "Robot stopped..." << endl;
}






int main(){
// Initialize the ctrl + c catch
struct sigaction sigIntHandler;
sigIntHandler.sa_handler = eHandler;
sigemptyset(&sigIntHandler.sa_mask);
sigIntHandler.sa_flags = 0;
sigaction(SIGINT, &sigIntHandler, NULL);

// Check the voltage levels
int cvoltage = BP.get_voltage_battery();
if (cvoltage < 10) {
    cout << "[ERROR] BATTERY CRITICAL!" << endl;
    cout << "Voltage Level: " << cvoltage << endl;
    BP.reset_all();
    exit(0);
}
BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
BP.set_sensor_type(PORT_4, SENSOR_TYPE_TOUCH_NXT);

Calibration();
BP.set_motor_limits(PORT_B, 30, 0);
BP.set_motor_limits(PORT_C, 30, 0);
BP.set_motor_limits(PORT_D, 90, 0);

cout << " _______________________________________________" << endl;
cout << "|                                               |" << endl;
cout << "|        Hoe wilt u de robot gebruiken?         |" << endl;
cout << "|                                               |" << endl;
cout << "| Keuze 1: De robot over een grid laten rijden. |" << endl;
cout << "| Keuze 2: Keuzemenu verlaten.                  |" << endl;
cout << "|                                               |" << endl;
cout << "|    Selecteer uw keuze met 1, 2, 3, 4 of 5.    |" << endl;
cout << "|_______________________________________________|" << endl;
cout << endl;

while(true){

string Keuze;
getline(cin,Keuze);

if(Keuze == "1"){
    controlGrid();
}else if(Keuze == "3"){
    turnAround();
}
else if(Keuze == "2"){
    cout << "...Exiting Keuzemenu..." << endl;
    break;
}else{
    cout << "Geen geldige keuze! Probeer opnieuw..." << endl;
}
}
BP.reset_all();
}



// Error handler
void eHandler(int s){
    cout << "Exiting..." << endl;
    cout << "Reverse bot? ";
    string uin;
    cin >> uin;
    if(uin == "y"){
    reverseBot();
    }
    BP.reset_all();
    exit(0);
}
