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

const bool enableDebug = false;
enum direction{
    left,
    right,
    forward,
    backwards,
    none
};
enum directType{
    motorRelative,
    timeScale,
    motorDPS,
    stop,
    mb
};

struct movement{
    direction dir;    //Direction robot went
    directType type;        //Type of direction(movement by relative motor or a time-based)
    int stepsL;              //Amount of steps taken left
    int stepsR;             //AMount of steps taken right
};

vector<movement> pathLogger;

// Error handler
void eHandler(int s){
    cout << "Exiting..." << endl;
    string out1 = "";
    int x = 0;
    for(movement moveStep : pathLogger){
        string out2;
        string direction;
        switch(moveStep.dir){
            case left:
                direction = "left";
                break;
            case right:
                direction = "right";
                break;
            case forward:
                direction = "forward";
                break;
            case backwards:
                direction = "backwards";
                break;
            case none:
                direction = "none";
                break;
            default:
                direction = "error in movement switch/case";
                break;
        };
        out2 = "- " + direction + " -L: " + to_string(moveStep.stepsL) + " -R: " + to_string(moveStep.stepsR);
        if(out1 == out2){
            x++;
        }else{
            out1 = out2;
            out1 += " * " + x;
            x = 0;
        }
        cout << out2 << endl;
    }
    BP.reset_all();
    exit(0);
}

//-- Movement functions
// Stop the robot by setting the motor power to '0'
void moveStop(){
    movement movementStep;
    movementStep.dir = none;
    movementStep.type = stop;
    movementStep.stepsL = 0;
    movementStep.stepsR = 0;
    pathLogger.push_back(movementStep);

    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);

    // Zet stroom van poort B en C op 0, waardoor de robot stopt.
}

void moveBot(const int measurement, const int valueLeft, const int valueRight) {
    movement movementStep;
    movementStep.dir = forward;
    movementStep.type = mb;
    movementStep.stepsL = valueLeft;
    movementStep.stepsR = valueRight;
    pathLogger.push_back(movementStep);

	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor
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

    movement movementStep;
    movementStep.dir = left;
    movementStep.type = motorDPS;
    movementStep.stepsL = speed;
    movementStep.stepsR = -speed;

    pathLogger.push_back(movementStep);

    BP.set_motor_dps(PORT_B, speed);
    BP.set_motor_dps(PORT_C, -speed);
}

void moveRight() {
    int speed = 80;

    movement movementStep;
    movementStep.dir = right;
    movementStep.type = motorDPS;
    movementStep.stepsL = -speed;
    movementStep.stepsR = speed;

    pathLogger.push_back(movementStep);

	BP.set_motor_dps(PORT_B, -speed);
	BP.set_motor_dps(PORT_C, speed);
}

void moveFwd() {
    int speed = 420;

    movement movementStep;
    movementStep.dir = forward;
    movementStep.type = motorRelative;
    movementStep.stepsL = speed;
    movementStep.stepsR = speed;

    pathLogger.push_back(movementStep);
    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, speed);
}

void moveBack() {
    int speed = -420;

    movement movementStep;
    movementStep.dir =backwards;
    movementStep.type = motorRelative;
    movementStep.stepsL = speed;
    movementStep.stepsR = speed;

    pathLogger.push_back(movementStep);
    BP.set_motor_position_relative(PORT_B, -speed);
    BP.set_motor_position_relative(PORT_C, -speed);
}

void moveFwd(const int & time) {
    int speed = 20;

    movement movementStep;
    movementStep.dir = forward;
    movementStep.type = timeScale;
    movementStep.stepsL = time;
    movementStep.stepsR = time;

    pathLogger.push_back(movementStep);
	BP.set_motor_power(PORT_B, speed);
	BP.set_motor_power(PORT_C, speed);
	usleep(time);
}

void moveLeft(const int & time) {
    movement movementStep;
    movementStep.dir = left;
    movementStep.type = timeScale;
    movementStep.stepsL = time;
    movementStep.stepsR = time;

    pathLogger.push_back(movementStep);
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
}

void moveRight(const int & time) {
    movement movementStep;
    movementStep.dir = right;
    movementStep.type = timeScale;
    movementStep.stepsL = time;
    movementStep.stepsR = time;

    pathLogger.push_back(movementStep);
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, 20);
	usleep(time);
}

void moveBack(const int &time) {
    movement movementStep;
    movementStep.dir = backwards;
    movementStep.type = timeScale;
    movementStep.stepsL = time;
    movementStep.stepsR = time;

    pathLogger.push_back(movementStep);
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
}

void turnLeft(){
    int speed = 420;
    movement movementStep;
    movementStep.dir = left;
    movementStep.type = motorRelative;
    movementStep.stepsL = speed;
    movementStep.stepsR = -speed;

    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, -speed);
}

void turnRight(){
    int speed = 420;
    movement movementStep;
    movementStep.dir = right;
    movementStep.type = motorRelative;
    movementStep.stepsL = -speed;
    movementStep.stepsR = speed;

    pathLogger.push_back(movementStep);
    BP.set_motor_position_relative(PORT_B, -speed);
    BP.set_motor_position_relative(PORT_C, speed);
}

void obstacleAvoidenceFwd() {
	cout << "obstacle avoidence, going forward..." << endl;
	int object = false;
	int timer = 0;
	BP.set_motor_position_relative(PORT_D, -90);
	moveFwd(4000000);
	moveStop();
	while (timer < 2000) {
		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			timer++;
			usleep(1000);
			if (Ultrasonic2.cm < 30) {
				object = true;
			}
			else {
				object = false;
			}
		}
		else {
			cout << "can't find ultrasonic sensor" << endl;
		}

	}
	BP.set_motor_position_relative(PORT_D, 90);
	usleep(1000000);
	if (object == true) {
		obstacleAvoidenceFwd();
	}
	else
	{
		return;
	}
}

void obstacleAvoidenceLeft() {
	cout << "obstacle avoidence, going left..." << endl;
	BP.set_motor_position_relative(PORT_B, 290);
	BP.set_motor_position_relative(PORT_C, -290);
	usleep(1500000);
	return;
}
void obstacleAvoidenceRight() {
	cout << "obstacle avoidence, going left..." << endl;
	BP.set_motor_position_relative(PORT_B, -270);
	BP.set_motor_position_relative(PORT_C, 270);
	usleep(1500000);
	return;
}

void obstacleAvoidenceEnding() {
	cout << "obstacle avoidence, searching for the line" << endl;
	while (true) {
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			if (Light3.reflected > 1800 && Light3.reflected < 2100) {
				moveFwd(100000);
			}
			else
			{
				moveFwd(500000);
				moveLeft(250000);
				moveStop();
				return;
			}

		}
	}
}

void avoidObstacle() {
	cout << "starting obstacle detection..." << endl;
	obstacleAvoidenceLeft();
	obstacleAvoidenceFwd();
	obstacleAvoidenceRight();
	obstacleAvoidenceFwd();
	obstacleAvoidenceRight();
	obstacleAvoidenceEnding();
}

void driveByLine() {

	if (BP.get_voltage_battery() >= 9) {
		while (true) {
			if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
					cout << "searching line..." << endl;

					if (Ultrasonic2.cm > 10) {
						cout << Light3.reflected << endl;
						if (Light3.reflected > 1800 && Light3.reflected < 2000) {
							cout << "wit" << endl;
							moveLeft(100000);
							//als ie het wit in gaat
						}
						else if (Light3.reflected >= 2000 && Light3.reflected <= 2200) {
							cout << "half" << endl;
							moveFwd(100000);
							//rechtdoor
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
// Check if there is a regular crossing(both sensors would be black)
bool isCrossing(){
    int measurement = 0;
    bool s1 = false;
    bool s2 = false;

    // Check if RGB sensor reads black
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(enableDebug){
        cout << "rgb: " << measurement << endl;
        }
        if(measurement >=200 && measurement < 450){
            s1 = true;
        }
    }


    // Check if IR sensor reads black
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        if(enableDebug){
        cout << "ir: " << measurement << endl;
        }
        if(measurement >= 2200){
            s2 = true;
        }
    }

    // if both read black it's a crossing
    if(s1 && s2){
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
    if(enableDebug){
    cout << endl << "cm: " << Ultrasonic2.cm << endl;
    }
    return false;

}

// Check if there is a line
bool lineDetected(){
    int measurement = 0;
    if(enableDebug){
    cout << endl << endl << "checking line";
    }
    bool s1 = false;
    bool s2 = false;
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        if(enableDebug){
        cout << endl << "measured RGB: " << measurement << " borders: 2000 <= x < 2700" << endl;
        }
        if(measurement >=2000 && measurement < 2700){
            s1 = true;
        }else{
            s1 = false;
        }
    }
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(enableDebug){
        cout << endl << "m: " << measurement << " borders: 200 <= x < 400" << endl;
        }
        if(measurement < 400 && measurement > 200){
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


// Check a crossing
void checkGrid(){
    const unsigned int routesToCheck = 3;
    const int sleepTime = 2;

    vector<bool> values = {false, false, false};
    bool driveRequired = false;


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
                if(enableDebug){
                cout << "checking if path...";
                }
                // Bring wheels to crossing center and turn left
                moveFwd(1500000);
                turnLeft();

                // Go to left-line
                sleep(sleepTime);
                moveFwd();
                sleep(sleepTime);
                moveStop();

                // Check if there is a line
                if(!lineDetected()){
                    if(enableDebug){
                    cout << "no path" << endl;
                    }
                }else{
                    if(enableDebug){
                    cout << "path found" << endl;
                    }
                    values[1] = true;
                }
                // Go back to center
                moveBack();
                sleep(sleepTime);

                // Turn back to face the middle-line and move back to original position
                turnRight();
                sleep(sleepTime);
                moveBack(1500000);

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
                if(!obstacleDetected()){
                    if(enableDebug){
                    cout << "...clear!" << endl;
                    }
                    lookLeft();         // Reset eyes
                    if(enableDebug){
                    cout << "checking if path...";
                    }

                    // Move wheels to center and turn right
                    moveFwd(1500000);
                    turnRight();
                    sleep(sleepTime);

                    // Go to right-line
                    moveFwd();
                    sleep(sleepTime);
                    moveStop();

                    // Check if there is a line
                    if(!lineDetected()){
                        if(enableDebug){
                        cout << "no path" << endl;
                        }
                    }else{
                        if(enableDebug){
                        cout << "path found" << endl;
                        }
                        values[2] = true;
                    }

                    // Go back to the center of crossing
                    moveBack();
                    sleep(sleepTime);

                    // Turn back to face the middle-line and go back to original position
                    turnLeft();
                    sleep(sleepTime);
                    moveBack(1500000);
                }else{
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
        cout << "None" << endl;
        cout << "Moving back" << endl; //rotate 180 and follow line
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
        randomDirectionChooser = rand() % 4 + 1;
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
                break;
        }

        if(uinDirection == "left"){
            if(values[1]){
                moveFwd(1500000);
                turnLeft();
                sleep(sleepTime);
                if (BP.get_sensor(PORT_3, Light3) == 0) {
                    if (Light3.reflected >= 2000 && Light3.reflected <= 2200) {
                        turnLeft();
                    }
                }
                return;
            }


        }else if(uinDirection == "right"){
            if(values[2]){
                moveFwd(1500000);
                turnRight();
                sleep(sleepTime);
                if (BP.get_sensor(PORT_3, Light3) == 0) {
                    if (Light3.reflected >= 2000 && Light3.reflected <= 2200) {
                        turnRight();
                    }
                }
                return;

            }
        }else if(uinDirection == "forward"){
            if(values[0]){
                moveFwd(1500000);
                moveFwd();
                sleep(sleepTime);

                return;

            }
        }else{
            cout << "Unknown direction" << endl;
        }
    }
}




//- Main functions
// Let CliniBot follow a grid
void controlGrid(){
    int measurement = 0;
    // Check if the battery is still sufficiently charged, else shutdown
    if (BP.get_voltage_battery() >= 9) {
		while (true) {

			if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
				if (BP.get_sensor(PORT_3, Light3) == 0) {
                    if(enableDebug){
                    cout << "searching line..." << endl;
                    }
					if (Ultrasonic2.cm > 10) {
						if (Light3.reflected >= 2000 && Light3.reflected <= 2200) {
                            if(enableDebug){
                                cout << "half" << endl;
                            }
							//moveFwd(100000);
                            moveBot(Light3.reflected, 50, 50);
							//rechtdoor
						}
						else if (Light3.reflected > 1800 && Light3.reflected < 2000) {
                            if(enableDebug){
                                cout << "wit" << endl;
                            }
							//moveLeft(100000);
                            moveBot(Light3.reflected, 5, 50);
							//als ie het wit in gaat
						}
						else if (Light3.reflected > 2200) {
                            if(enableDebug){
                                cout << "zwart" << endl;
                            }
                            if(isCrossing()){
                                moveStop();
                                sleep(2);
                                checkGrid();
                            }
                            moveBot(measurement, 50, 5);
							//moveRight(100000);

							//als ie het zwart in gaat

						}
					}

				}
				else
				{
					cout << "ERROR: Can't read ultra-red sensor..." << endl;
				}
			}
			else {
				cout << "ERROR: Can't read ultrasonic sensor..." << endl;
			}
		}
		usleep(100000);
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
