//--Includes
#include <iostream>      	// Basic IO
#include <unistd.h>      	// for usleep and sleep(in linux)
#include <signal.h>      	// for catching exit signals
#include "BrickPi3.h"    	// BrickPi3 Header file - used to control the ClinBot
#include <string>        	// String variables
#include <vector>       	// Vector variables
#include <cstdlib>  		// random numbers
#include "BluetoothSocket.h"//bluetooth capablitiy

//--Using statements
using std::cout;
using std::endl;
using std::getline;
using std::cin;
using std::string;
using std::to_string;
using std::vector;

//--Definitions
BrickPi3 BP; 								// Define an instance of BrickPi3, called 'BP'
BluetoothServerSocket serversock(2, 1);  	// Bluetooth socket for the server(CliniBot)
BluetoothSocket* clientsock;				// Bluetooth socket for the client(Phone)

// Sensor definitions
sensor_light_t      Light3; 				//Infrared sensor
sensor_color_t      Color1; 				//RGB Light sensor
sensor_ultrasonic_t Ultrasonic2; 			//Ultrasonic sensor
sensor_touch_t      Touch4;					//Touch sensor

// Dance colour variables
int Rood;									//Measured amount of 'red'
int Groen;									//Measured amount of 'green'
int Blauw;									//Measured amount of 'blue'

//--Public booleans
const bool enableDebug = true;				//Setting to use debug options(extra couts) (hard-coded)
bool useBluetooth = false;					//Setting to use bluetooth(set by user in main menu)
bool isReversing = false;					//Variable for the robot to see if it's currently reversing

//--Public ints
// Measured calibration values for the infrared sensor -- In infrared the BLACK values are HIGHER than the RGB values
int blackHigh = 0;							//Infrared upper black limit(highest measured value in black)
int blackLow = 1000000;						//Infrared lower black limit(lowest measured value in black)
int whiteHigh = 0;							//Infrared upper white limit(highest measured value in white)
int whiteLow = 10000000;					//Infrared lower white limit(lowest measured value in white)
// Measured calibration values for the RGB sensor -- In RGB the WHITE values are HIGHER than the infrared values
int RGBBlackHigh = 0;						//RGB upper black limit(highest measured value in black)
int RGBBlackLow = 1000000;					//RGB lower black limit(lowest measured value in black)
int RGBWhiteHigh = 0;						//RGB upper white limit(highest measured value in white)
int RGBWhiteLow = 1000000;					//RGB lower white limit(lowest measured value in white)
const int lookAngle = 105;					// Amount of degrees the robot will look left or right(hard-coded)

//--Reverse definitions
// An enum for the direction the robot went(possible directions: left/right/forward/backwards)
enum direction{
    left,
    right,
    forward,
    backwards,
};

// A struct to enable us to save the direction the robot went
struct movement{
    direction dir;    //The direction the robot went called 'dir'
};

// A vector to store each movement. Each movement is added to the back(most revent movement has the last index)
// !! Upon reverse, we loop from last index to first index.
vector<movement> pathLogger;


//--Check functions -- functions that check for a condition or state
// 'buttonPressed' returns TRUE if the touch sensor is pressed and FALSE if it's not pressed
bool buttonPressed(){
	//Loop 4000 times
	unsigned int i = 0;
	while (i <= 4000) {
		i++;

		// If the sensor is pressed, return true
		if (BP.get_sensor(PORT_4, Touch4) == 0) {
			if(Touch4.pressed){
				return true;
			}
		}

	}
	// If after 4000 times the sensor hasn't given 'true' once, return false
	return false;
}

// Check if there is a regular crossing(both sensors would be black)
bool isCrossing(){
    int measurement = 0;					// Measured value
    bool s1 = false;						// If the RGB sensor has seen black
    bool s2 = false;						// If the infrared sensor has seen black

    // Check if RGB sensor reads black
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(measurement >=RGBBlackLow && measurement <= RGBBlackHigh){
            s1 = true;
        }
    }

    // Check if IR sensor reads black
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        if(measurement >= blackLow){
            s2 = true;
        }
    }

    // if both read black it's a crossing
    if(s1 && s2){
        cout << " > Crossing detected" << endl;		// Notify the user a crossing has been found
        return true;
    }else{
        return false;
    }
}

// Check if there is an obstacle in FRONT of the robot
bool obstacleDetected(){
    int obstacleDetectionDistance = 25;				// Cm the disance an object should be for it to be a problem(default: 25)
    int timeout = 0;								// The amount of times the loop has 'looped'

    while(true){
        timeout++;
        if(timeout > 4000){
            break;									// After 4000 loops, break out of the loop and return false
        }
		// Check if an object is closer than the allowed distance. If so -> return true
        if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
                if(Ultrasonic2.cm <= obstacleDetectionDistance){
                    return true;

                }

        }
    }
    return false;

}

// Check if there is a line(One or both of the sensors should give black or half-black)
bool lineDetected(){
    int measurement = 0;							// Measured value
    bool s1 = false;								// If IR reads 50% black
    bool s2 = false;								// If RGB reads 50% black

	// Check if the infrared sensor reads higher than whiteHigh and lower than blackHigh
    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        if(measurement >=whiteHigh && measurement < blackHigh){
            s1 = true;
        }else{
            s1 = false;
        }
    }

	// Check if the RGB sensor reads higher than RGBwhiteHigh and lower than RGBblackHigh
    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(measurement < RGBBlackHigh && measurement > RGBWhiteHigh){
            s2 = true;
        }else{
            s2 = false;

        }
    }

	// If either sensor reads black, return true
    if(s1 == true || s2 == true){
        return true;
    }else{
        return false;
    }

}

//--Calibration script -- dynamically set the values for the sensors everytime the code is ran
void Calibration() {
	int time = 4000;	// The amount of times to check each sensor(default: 4000)
	int average;		// The average RGB colour value measured(red + green + blue / 3)
	char input;			// The character that has been pressed

	while (true) {

		// If no cup is present in the cupholder(thus the sensor not being pressed), notify the user so they don't forget
		if(!buttonPressed()){
			cout << "*** DO NOT FORGET THE CUP, PLEASE!!! ***" << endl;
		}

		// If the highest measured infrared value for black = 0(an impossible value), start calibration for black
		if (blackHigh == 0) {
			cout << "place the robot on black and press s + enter to start" << endl;
			cin >> input;

			// Wait until input == 's'
			if (input == 's') {

				// Loop 'time' amount of times
				for (int i = 0; i < time; i++) {

					// If the get_sensor has exit code 0, check if the value is higher than blackHigh and set it if that is TRUE
					// Else, check if the measured value is lower than the blackLow and set it if that is TRUE
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected > blackHigh) {
							blackHigh = Light3.reflected;
						}
						if (Light3.reflected < blackLow) {
							blackLow = Light3.reflected;
						}
					}

					// If the get_sensor has exit code 0, check if the value is higher than RGBblackHigh and set it if that is TRUE
					// Else, check if the measured value is lower than the RGBblackLow and set it if that is TRUE
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBBlackHigh) {
							RGBBlackHigh = average;
						}
						if (average < RGBBlackLow) {
							RGBBlackLow = average;
						}
					}
					usleep(1000); //Sleep 1000 microseconds
				}
			}
			input = ' '; // Clear Input

			// Show the user the calibration results for black
			cout << "black high value is: " << blackHigh << endl;
			cout << "black low value is: " << blackLow << endl;
			cout << "RGB black high value is: " << RGBBlackHigh << endl;
			cout << "RGB black low value is: " << RGBBlackLow << endl;
		}

		// if whiteHigh has an impossible number(default: 0)
		if (whiteHigh == 0) {

			cout << "place the robot on white and press s + enter to start" << endl;
			cin >> input;

			// Wait until input == 's'
			if (input == 's') {

				// Loop 'time' amount of times
				for (int a = 0; a < time; a++) {

					//If get_sensor has the exit code 0, check if the value is higher or lower than the already measured extremes. If so, set them accordingly.
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected > whiteHigh) {
							whiteHigh = Light3.reflected;
						}
						if (Light3.reflected < whiteLow) {
							whiteLow = Light3.reflected;
						}
					}

					//If get_sensor has the exit code 0, check if the value is higher or lower than the already measured extremes. If so, set them accordingly.
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBWhiteHigh) {
							RGBWhiteHigh = average;
						}
						if (average < RGBWhiteLow) {
							RGBWhiteLow = average;
						}
					}
					usleep(1000); // Sleep for 1000 microseconds
				}
			}
			input = ' ';	// Clear the Input

			// Show the user the results for the white calibrated values
			cout << "white high value is: " << whiteHigh << endl;
			cout << "white low value is: " << whiteLow << endl;
			cout << "RGB white high value is: " << RGBWhiteHigh << endl;
			cout << "RGB white low value is: " << RGBWhiteLow << endl;
		}

		// If the results are valid and make sense, break out of the infinite loop and use the calibrated values globally
		if (blackLow > whiteHigh && RGBWhiteLow > RGBBlackHigh) {
			break;
		}
		// If they are invalid, reset the calibration values to impossible values and calibrate again
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
	// Tell the user the calibration succeeded
	cout << "calibration complete..." << endl;
	return;
}

//-- Function declarations
void eHandler(int s);	// Function that catches if the user did 'ctrl + c'. We do this so we can reset the sensors when this happens
void controlGrid();		// The main grid function, this contains the follow-line script and checks for crossings when needed.

//-- Basic bovement functions
// Stop the robot by setting the motor power to '0' for both wheels
void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
}

// Main movement function, drive by setting the power.
void moveBot(const int measurement, const int valueLeft, const int valueRight) {
	BP.set_motor_power(PORT_C, valueLeft); 		// Left motor
    BP.set_motor_power(PORT_B, valueRight); 	// Right motor
	usleep(50000);								// Sleep for 50,000 microseconds
}

// Turn left
void moveLeft() {
    int speed = 80;								// The amount the wheel has to turn(default: 80)
    BP.set_motor_dps(PORT_B, speed);			// Right motor
    BP.set_motor_dps(PORT_C, -speed);			// Left motor
}

// Turn right
void moveRight() {
    int speed = 80;								// The amount the wheel has to turn(default: 80)
	BP.set_motor_dps(PORT_B, -speed);			// Right motor
	BP.set_motor_dps(PORT_C, speed);			// Left motor
}

// Go forward a little
void moveFwd() {
    int speed = 420;								// The degrees the wheel has to turn(default: 420)
    BP.set_motor_position_relative(PORT_B, speed);	// Right motor
    BP.set_motor_position_relative(PORT_C, speed);	// Left motor
}

// Turn left 420 degrees
void turnLeft(){
    int speed = 420;
    BP.set_motor_position_relative(PORT_B, speed);
    BP.set_motor_position_relative(PORT_C, -speed);
}

// Turn right 420 degrees
void turnRight(){
    int speed = 420;
    BP.set_motor_position_relative(PORT_B, -speed);
    BP.set_motor_position_relative(PORT_C, speed);
}

// Go backwards a little
void moveBack() {
	int speed = -420;								// The degrees the wheel has to turn(default: -420)
    BP.set_motor_position_relative(PORT_B, speed);	// Right motor
    BP.set_motor_position_relative(PORT_C, speed);	// Left motor
}

// Move forward for 'time' amount of time(time in MICROSECONDS | 1 second = 1,000,000 microseconds)
void moveFwd(const int & time) {
    int speed = 25;									// The amount of power set on the wheel(default: 25)
	BP.set_motor_power(PORT_B, speed);				// Right motor
	BP.set_motor_power(PORT_C, speed);				// Left motor
	usleep(time);									// Sleep for 'time' amount of time(wheels turn during this time)
}
// Turn left for 'time' amount of time(time in MICROSECONDS | 1 second = 1,000,000 microseconds)
void moveLeft(const int & time) {
	int speed = 25;									// The amount of power set on the wheel(default: 25)
	BP.set_motor_power(PORT_B, speed);				// Right motor
	BP.set_motor_power(PORT_C, -speed);				// Left motor
	usleep(time);									// Sleep for 'time' amount of time(wheels turn during this time)
}

// Turn right for 'time' amount of time(time in MICROSECONDS | 1 second = 1,000,000 microseconds)
void moveRight(const int & time) {
	int speed = -25;								// The amount of power set on the wheel(default: -25)
	BP.set_motor_power(PORT_B, speed);				// Right motor
	BP.set_motor_power(PORT_C, -speed);				// Left motor
	usleep(time);									// Sleep for 'time' amount of time(wheels turn during this time)
}

// Move back for 'time' amount of time(time in MICROSECONDS | 1 second = 1,000,000 microseconds)
void moveBack(const int & time) {
    int speed = -25;								// The amount of power set on the wheel(default: -25)
	BP.set_motor_power(PORT_B, speed);				// Right motor
	BP.set_motor_power(PORT_C, speed);				// Left motor
	usleep(time);									// Sleep for 'time' amount of time(wheels turn during this time)
}

// Function to keep turning until a line has been detected
void turnAround(){
    while(!lineDetected){
        moveRight(2);
    }
    cout << "Found line";
    moveStop();

}

//--Eye movement functions
// Turn the eyes left 'lookAngle' degrees
void lookLeft(){
    BP.set_motor_position_relative(PORT_D, lookAngle);
}

// Turn the eyes right 'lookAngle' degrees
void lookRight(){
    BP.set_motor_position_relative(PORT_D, -lookAngle);
}

// Shake head left or right
void shakeHead(){
	BP.set_motor_limits(PORT_D, 50, 0);

	lookLeft();
	usleep(100000);
	lookRight();
	usleep(2000);

	lookRight();
	usleep(100000);
	lookLeft();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);
	lookRight();
	usleep(100000);
	lookLeft();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);
	lookLeft();
	usleep(100000);
	lookRight();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);


	lookLeft();
	usleep(100000);
	lookRight();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);
	lookRight();
	usleep(100000);
	lookLeft();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);
	lookRight();
	usleep(100000);
	lookLeft();
	usleep(2000);

	BP.set_motor_power(PORT_D, 0);
	lookLeft();
	usleep(100000);
	lookRight();
	usleep(20000);


	BP.set_motor_power(PORT_D, 0);
	BP.set_motor_limits(PORT_D, 90, 0);
}

//-- Control functions
// Function that sets the robot in reverse mode and turns it 180 degrees
void reverseBot() {
	cout << "Reversing..." << endl;					// Tell the user the reversing starts
	isReversing = true;								// Set the 'isReversing' boolean to TRUE

	//- Rotating 180 degrees
	moveRight(1000000);								// Move right about a second to make sure the robot has lost the line
	moveStop();										// Stop moving
	while (true) {									// Keep turning right about 50,000 microseconds until the robot found a line
		if (BP.get_sensor(PORT_3, Light3) == 0) {
			if (Light3.reflected < blackLow) {
				moveRight(50000);
			}

			else {
				// If the line has been found -> stop moving and break out of the infinite loop
				moveStop();
				usleep(1000000);
				cout << "end of reversing" << endl;	// Notify user that the reversing function went successfully
				break;
			}
		}
	}
	// Go back to the 'controlGrid' function
	return controlGrid();
}

// Check a crossing
void checkGrid(){
    const unsigned int routesToCheck = 3;			// Amount of routes to check(default: 3) *** STRONGLY RECOMMENDED TO KEEP THIS AT 3 OR LOWER ***
    const int sleepTime = 2;						// Amount of seconds to turn(default: 2)


	int stepsLeft = 0;								// Steps the robot did left
	int stepsRight = 0;								// Steps the robot did right
	int average = 0;								// RGB average measured

    vector<bool> values = {false, false, false};	// Vector containing which paths have been found(index 0 = forward, 1 = left, 2 = right)
    bool driveRequired = false;						// If the robot needs to drive to find a path(always true unless an object has been detected)

	// If the robot is not reversing:
	// Check if an object is present, if not check if a path exists. If so, path is invalid
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
            sleep(1);
            if(!obstacleDetected()){
                if(enableDebug){
                cout << "...clear!" << endl;
                }
                lookRight();                    // Reset eyes
                sleep(1);               // give the eyes time to look
                if(enableDebug){
                cout << "checking if path...";
                }
                // Bring wheels to crossing center and turn left
                moveFwd(1000000);
				moveStop();
				moveLeft(1000000);
				stepsLeft += 1000000;
				usleep(50000);
				while (true) {
					if (BP.get_sensor(PORT_1, Color1) == 0) {
						average = (Color1.reflected_blue + Color1.reflected_green + Color1.reflected_red) / 3;
						if (average > RGBBlackHigh) {
							if (stepsLeft > 3200000) {
								cout << "no path found" << endl;
								moveStop();
								usleep(50000);
								break;
							}
							moveLeft(50000);
							stepsLeft += 50000;
						}
						else {
							cout <<"steps: " << stepsLeft << endl;
							cout << "path found" << endl;
							values[1] = true;
							moveStop();
							usleep(50000);
							break;
						}
					}
				}


                // Turn back to face the middle-line and move back to original position
				moveRight(stepsLeft);
				moveStop();
				stepsRight += stepsLeft;
				stepsLeft += 200000;
				usleep(50000);
				while (true) {
					if (BP.get_sensor(PORT_3, Light3) == 0) {
						if (Light3.reflected < whiteHigh)
						{
							if (stepsRight > stepsLeft) {
								moveLeft(250000);
								moveStop();
								usleep(50000);
								stepsRight = 0;
								stepsLeft = 0;
								break;
							}
							moveRight(50000);
							stepsRight += 50000;
						}
						else {
							moveRight(200000);
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
						moveBack(50000);
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
                sleep(1);

                if(!obstacleDetected()){
                    if(enableDebug){
                    cout << "...clear!" << endl;
                    }
                    lookLeft();         // Reset eyes
                    sleep(1);

                    if(enableDebug){
                    cout << "checking if path...";
                    }

                    // Move wheels to center and turn right
                    moveFwd(1000000);
					moveStop();
					moveRight(1000000);
					stepsRight += 1000000;
					usleep(50000);
					while (true) {
						if (BP.get_sensor(PORT_3, Light3) == 0) {
							if (Light3.reflected < whiteHigh) {
								if (stepsRight >= 2700000) {
									cout << "no path found" << endl;
									moveStop();
									usleep(50000);
									break;
								}
								moveRight(50000);
								stepsRight += 50000;
							}
							else {
								cout << "steps: " << stepsRight << endl;
								cout << "path found" << endl;
								values[2] = true;
								moveStop();
								usleep(50000);
								break;
							}
						}
					}

                    // Turn back to face the middle-line and go back to original position
					moveLeft(stepsRight);
					moveStop();
					stepsLeft += stepsRight;
					stepsRight += 200000;
					usleep(50000);
					while (true) {
						if (BP.get_sensor(PORT_1, Color1) == 0) {
							average = (Color1.reflected_blue + Color1.reflected_red + Color1.reflected_green) / 3;
							if (average > RGBBlackHigh){
								if (stepsLeft > stepsRight) {
									moveRight(250000);
									moveStop();
									usleep(50000);
									//klaas
									break;
								}
								moveLeft(50000);
								stepsLeft+=50000;
							}
							else {
								moveLeft(200000);
								moveStop();
								usleep(50000);
								break;
							}
						}
					}
					while (true) {
						if (!isCrossing()) {
							moveBack(50000);
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
	// Check if there is any valid path. If none, go backwards
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
    }
	// If one or more valid paths, show which ones to the user
	else{
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

	// If bluetooth is disabled
	// 	Keep picking a random direction until a valid one has been found.
	//  Then go to that direction and save it in pathlogger
	if(!useBluetooth){
		// Random choice
		string uinDirection;
	    int randomDirectionChooser = 0;
	    while(true){
	        movement currentMovement;
	        srand((unsigned) time(0));
	        randomDirectionChooser = rand() % 3 + 1;
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
					moveStop();
					moveLeft(1000000);
					while (true) {
						if (BP.get_sensor(PORT_3, Light3) == 0) {
							if (Light3.reflected <= whiteHigh) {
								moveLeft(50000);
							}
							else
							{
								moveStop();
								return;
							}
						}
					}
	            }


	        }else if(uinDirection == "right"){
	            if(values[2]){
					currentMovement.dir = right;
					pathLogger.push_back(currentMovement);
	                moveFwd(1000000);
					moveRight(1000000);
					while (true) {
						if (BP.get_sensor(PORT_3, Light3) == 0) {
							if (Light3.reflected <= whiteHigh) {
								moveRight(50000);
							}
							else {
								moveRight(500000);
								moveStop();
								return;
							}
						}

					}
	            }
	        }else if(uinDirection == "forward"){
	            if(values[0]){
	                currentMovement.dir = forward;
	                pathLogger.push_back(currentMovement);


	                moveFwd(1000000);
	                return;

	            }
	        }else{
	            cout << "Unknown direction" << endl;
	        }

		}
	}
	// If bluetooth is enabled
	// Wait until a valid input is given through bluetooth and go that direction if possible.
	// If not possible, shake head to notify the user that direction is invalid
	else{
		bool foundDirection = false;
		movement currentMovement;
		while(!foundDirection){
		cout << "Waiting for direction..." << endl;
		MessageBox& mb = clientsock->getMessageBox();
		string input;
		while(mb.isRunning()) {
			input = mb.readMessage();  //blokkeert niet
			if(input != ""){
				// input
				cout << "Input: " << input << endl;

				if(input.find("UP") != std::string::npos){
					if(values[0]){
						cout << "Direction > Forward" << endl;

						foundDirection = true;
						currentMovement.dir = forward;
						pathLogger.push_back(currentMovement);


						moveFwd(1500000);
						return;

					}else{
						shakeHead();
					}
				}else if(input.find("LEFT") != std::string::npos){

					if(values[1]){
						cout << "Direction > Left" << endl;
						foundDirection = true;
						currentMovement.dir = left;
						pathLogger.push_back(currentMovement);
						moveFwd(1000000);
						moveLeft(1500000);
						while (true) {
							if (BP.get_sensor(PORT_3, Light3) == 0) {
								if (Light3.reflected >= whiteHigh && Light3.reflected <= blackLow) {
									moveLeft(50000);
								}
								else
								{
									moveStop();
									return;
								}
							}
						}
					}else{
						shakeHead();
					}

				}else if(input.find("RIGHT") != std::string::npos){

					if(values[2]){
						cout << "Direction > Right" << endl;

						foundDirection = true;
						currentMovement.dir = right;
						pathLogger.push_back(currentMovement);
						moveFwd(1000000);
						moveRight(1000000);
						while (true) {
							if (BP.get_sensor(PORT_3, Light3) == 0) {
								if (Light3.reflected < blackLow) {
									moveRight(50000);
								}
								else {
									moveRight(50000);
									moveStop();
									return;
								}
							}

						}
					}else{
						shakeHead();
					}

				}else if(input.find("DOWN") != std::string::npos){
					cout << "Direction > Back" << endl;

					foundDirection = true;
					currentMovement.dir = backwards;
					pathLogger.push_back(currentMovement);
					turnRight();
					sleep(sleepTime);
					turnRight();
					sleep(sleepTime);
				}
			}

			cout.flush();
			}

		}
	}

    }
	// If the robot is reversing:
	//	check what the last index in 'pathLogger' is
	//	if left or right: go opposite direction
	// 	if forward or backward, go still go to that direction
	else{
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

// Wait until the cup is once again in the cup holder
void waitForCup(){
	while(!buttonPressed()){
		cout << "Waiting for the cup" << endl;
	}
	return;

}

//-- Main functions
// Let CliniBot follow a grid
void controlGrid(){
	// Check if the battery is still sufficiently charged, else shutdown
    if (BP.get_voltage_battery() >= 9.0) {
        while(true){
			// If the robot is not yet reversing and no cup is present in the cupholder, reverse
			if(!isReversing){
				if(!buttonPressed()){
				cout << "Button is not pressed" << endl;
				return reverseBot();
				}
			}
			// If already reversing, but there are no more stored choices:
			// The robot is back at starting point.
			// Rotate 180 degrees and wait for a cup to be put back
            if(isReversing){
                if(pathLogger.size() == 0){
					cout << "Back at starting point!" << endl;

					// Move forward a bit to clear off the crossing
					moveFwd(2000000);
					moveStop();

					// Rotate 180 degrees
					const int sleepTime=2;
					turnRight();
					sleep(sleepTime);
					turnRight();
					sleep(sleepTime);

					// Wait until cup is put back
					waitForCup();

					// Robot is no longer reversing
					isReversing = false;
                }
            }

			// Check which direction to drive in
			if (BP.get_sensor(PORT_3, Light3) == 0) {
				if (Light3.reflected >= whiteHigh && Light3.reflected <= blackLow) {
					// Good value: go forward
                    moveBot(Light3.reflected, 30, 30);
				}
				else if (Light3.reflected >= whiteLow && Light3.reflected < whiteHigh) {
					// Lost line: go left
                    moveBot(Light3.reflected, -30, 30);
				}
				else if (Light3.reflected > blackLow) {
					// In black line: might be a crossing. Else -> go right
                    if(isCrossing()){
						// If crossing: Stop moving and check where to go to
                        moveStop();
                        sleep(2);
                        checkGrid();
                    }
					moveBot(Light3.reflected, 30, -30);
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



//-- Dance scripts
// Dance based on colour stored in public variables
void Dansje(){
    BP.set_motor_limits(PORT_B, 90, 0);
    BP.set_motor_limits(PORT_C, 90, 0);

    if(Rood > Groen && Rood > Blauw){

        cout << "=> Rode dans !!" << endl;

        lookLeft();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookLeft();
        usleep(500000);
        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        sleep(1);

        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        lookLeft();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);
        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -420);
        BP.set_motor_position_relative(PORT_C, 420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 420);
        BP.set_motor_position_relative(PORT_C, -420);
        sleep(1);
        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -410);
        BP.set_motor_position_relative(PORT_C, -410);
        usleep(500000);
        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 410);
        BP.set_motor_position_relative(PORT_C, 410);
        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        usleep(750000);
        lookLeft();
        sleep(3);

        }
    else if(Groen > Rood && Groen > Blauw){

        cout << "=> Groene Dans !!" << endl;

        BP.set_motor_position_relative(PORT_B, 360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -360);
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        sleep(1);

        BP.set_motor_position_relative(PORT_C, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_C, 360);
        BP.set_motor_position_relative(PORT_C, 360);
        sleep(1);

        BP.set_motor_position_relative(PORT_C, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookRight();
        usleep(500000);
        lookRight();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, 360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_C, -360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 360);
        BP.set_motor_position_relative(PORT_C, 360);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        sleep(1);

        BP.set_motor_position_relative(PORT_B, 0);
        BP.set_motor_position_relative(PORT_C, 0);

        lookLeft();
        usleep(500000);

        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        sleep(3);

    }

    else if(Blauw > Rood && Blauw > Groen){

        cout << "=> Blauwe dans !!" << endl;

        BP.set_motor_position_relative(PORT_B, -1680);
        BP.set_motor_position_relative(PORT_C, 1680);
        usleep(1000000);
        BP.set_motor_position_relative(PORT_B, 1680);
        BP.set_motor_position_relative(PORT_C, -1680);
        usleep(1000000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookLeft();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookRight();
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, -210);
        BP.set_motor_position_relative(PORT_C, 210);
        usleep(500000);
        BP.set_motor_position_relative(PORT_B, 210);
        BP.set_motor_position_relative(PORT_C, -210);
        lookRight();
        usleep(750000);
        BP.set_motor_position_relative(PORT_B, -840);
        BP.set_motor_position_relative(PORT_C, 840);
        usleep(750000);
        lookLeft();
        sleep(3);
    }
    else{
        cout << "Ongeldige waarde !!!" << endl;
    }

}

// Measure colours for red, green and blue
void MeetKleuren(){

while(true){

    sleep(1);

    if(BP.get_sensor(PORT_1, Color1) == 0){

    Rood = Color1.reflected_red;
    Groen = Color1.reflected_green;
    Blauw = Color1.reflected_blue;

    cout << endl;
    cout << "Kleurwaarde voor Rood = "<< Rood << endl;
    cout << "Kleurwaarde voor Groen = " << Groen << endl;
    cout << "Kleurwaarde voor Blauw = " << Blauw << endl;

    Dansje();

    }

    else{
        cout << "Sensor werkt niet" << endl;
        }
    }
}

// Main function
int main(){
// Initialize the ctrl + c catch
struct sigaction sigIntHandler;
sigIntHandler.sa_handler = eHandler;
sigemptyset(&sigIntHandler.sa_mask);
sigIntHandler.sa_flags = 0;
sigaction(SIGINT, &sigIntHandler, NULL);

// Check the voltage levels and stop program if lower than 9
int cvoltage = BP.get_voltage_battery();
if (cvoltage <= 9.0) {
    cout << "[ERROR] BATTERY CRITICAL!" << endl;
    cout << "Voltage Level: " << cvoltage << endl;
    BP.reset_all();
    exit(0);
}


BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
// Link sensors to ports
BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);
BP.set_sensor_type(PORT_4, SENSOR_TYPE_TOUCH_NXT);

// Set the motor limits
BP.set_motor_limits(PORT_B, 30, 0);		// Right motor(default: 30)
BP.set_motor_limits(PORT_C, 30, 0);		// Left motor(default: 30)
BP.set_motor_limits(PORT_D, 90, 0);		// Eye motor(default: 90)


// Menu
cout << " ___________________________________________________________" << endl;
cout << "|                                                           |" << endl;
cout << "|              Hoe wilt u de robot gebruiken?               |" << endl;
cout << "|                                                           |" << endl;
cout << "| Keuze 1: De robot over een grid laten rijden.             |" << endl;
cout << "| Keuze 2: De robot over een grid laten rijden (Bluetooth). |" << endl;
cout << "| Keuze 3: De robot laten dansen op basis van kleuren.      |" << endl;
cout << "| Keuze 4: Keuzemenu verlaten.                              |" << endl;
cout << "|                                                           |" << endl;
cout << "|           Selecteer uw keuze met 1, 2, 3 of 4.            |" << endl;
cout << "|___________________________________________________________|" << endl;
cout << endl;

// Loop until user gives valid choice
while(true){

string Keuze;
getline(cin,Keuze);

if(Keuze == "1"){
	useBluetooth=false;
	Calibration();
	cout << "Type s and enter to start" << endl;
	string input = "";
	cin >> input;
	controlGrid();
}else if(Keuze == "2"){
	useBluetooth=true;
	Calibration();

	cout << "Waiting for bluetooth device" << endl;
	clientsock = serversock.accept();
	cout << "accepted from " << clientsock->getForeignAddress().getAddress() << endl;
	cout << "Type s and enter to start" << endl;
	string input = "";
	cin >> input;
	controlGrid();
}
else if(Keuze == "4"){
    cout << "...Exiting Keuzemenu..." << endl;
    break;
}else if(Keuze == "3"){
	MeetKleuren();
	break;
}else{
    cout << "Geen geldige keuze! Probeer opnieuw..." << endl;
}
}
BP.reset_all();
}

// Ctrl + c catcher
void eHandler(int s){
    cout << "Exiting..." << endl;		// Notify the user that the program is exiting
	moveStop();							// Stop moving

	// If bluetooth was used, close the sockets
	if (useBluetooth) {
		clientsock->close();
	}

    BP.reset_all();						// Reset the sensors
    exit(0);							// Close program with exit code '0'
}
