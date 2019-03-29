// Includes
#include <iostream>      // Basic IO
#include <unistd.h>      // for usleep and sleep(for linux)
#include <signal.h>      // for catching exit signals
#include <curses.h>      // Library to be able to rea
#include "BrickPi3.h"    // BrickPi3 Header file - used to control the ROBO-MAN
#include <string>        // String variables
#include "BluetoothSocket.h" //Bluetooth
#include <vector>

// Using statements
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

//Sensor definitions
sensor_light_t      Light3; //RGB Light sensor
sensor_color_t      Color1; //Infrared sensor
sensor_ultrasonic_t Ultrasonic2; //Ultrasonic sensor

// Debugging bool
bool enableDebug = false;

// Error handler
void eHandler(int s){
    cout << "Exiting..." << endl;
    BP.reset_all();
    exit(0);
}

//-- Movement functions
// Stop the robot by setting the motor power to '0'
void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // Zet stroom van poort B en C op 0, waardoor de robot stopt.
}

void moveLeft() {
    BP.set_motor_dps(PORT_B, 80);
    BP.set_motor_dps(PORT_C, -80);
}

void moveRight() {
		BP.set_motor_dps(PORT_B, -80);
		BP.set_motor_dps(PORT_C, 80);
}

void moveFwd() {
    BP.set_motor_position_relative(PORT_B, 420);
    BP.set_motor_position_relative(PORT_C, 420);
}

void moveBack() {
    BP.set_motor_position_relative(PORT_B, -420);
    BP.set_motor_position_relative(PORT_C, -420);
}

void moveFwd(const int & time) {
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, 20);
	usleep(time);
}

void moveLeft(const int & time) {
	BP.set_motor_power(PORT_B, 20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
}

void moveRight(const int & time) {
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, 20);
	usleep(time);
}

void moveBack(const int &time) {
	BP.set_motor_power(PORT_B, -20);
	BP.set_motor_power(PORT_C, -20);
	usleep(time);
}

void turnLeft(){
    BP.set_motor_position_relative(PORT_B, 420);
    BP.set_motor_position_relative(PORT_C, -420);
}

void turnRight(){
    BP.set_motor_position_relative(PORT_B, -420);
    BP.set_motor_position_relative(PORT_C, 420);
}

void vvDance(){
    cout << "Het dak moet er af...!" << endl;
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, -360);
    sleep(2);
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    usleep(500000);
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

//- Debugging functions
// Read all data read from the sensors
void testValues(){
    int measurement = 0;
    while(true){
            if (BP.get_sensor(PORT_1, Color1) == 0) {
                measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
                if(enableDebug){
                cout << "Value for RGB: " << measurement;
                }
            }


            if (BP.get_sensor(PORT_3, Light3) == 0) {
                measurement = Light3.reflected;
                if(enableDebug){
                cout << " - Value for IR: " << measurement;
                }
            }
            if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
                if(enableDebug()){
                    cout << " - Value for ultrasonic: " << Ultrasonic2.cm << endl;
                }
            }
            usleep(500000);
    }
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
    while(true){
        cout << "Direction > ";
        cin >> uinDirection;

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
            }else{
                cout << "Impossible to go to this direction" << endl;
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

            }else{
                cout << "Impossible to go to this direction" << endl;
            }
        }else if(uinDirection == "forward"){
            if(values[0]){
                moveFwd(1500000);
                moveFwd();
                sleep(sleepTime);

                return;

            }else{
                cout << "Impossible to go to this direction" << endl;
            }
        }else{
            cout << "Unknown direction" << endl;
        }
    }
}

//- Display functions
// Show the movement controls on-screen
void showControls(){
    cout << "Controls: " << endl << "W - Forwards" << endl << "A - Left" << endl << "S - Backwards" << endl << "D - Right" << endl << endl << "Press enter to continue...";
}

//- Main functions
// Control CliniBot via bluetooth
void controlBluetooth(){
    BluetoothServerSocket serversock(2, 1);  //2 is het channel-number
	cout << "Waiting for a bluetooth device..." << endl;
	while(true) {
		BluetoothSocket* clientsock = serversock.accept();
		cout << "Connected with [" << clientsock->getForeignAddress().getAddress() << "]" << endl;
		MessageBox& mb = clientsock->getMessageBox();

		string input;
		while(mb.isRunning()) {
			input = mb.readMessage();  //blokkeert niet
            if(enableDebug){
            cout << "Executing Action: " << input << endl;
            }
			if(input != ""){
                // input
                if(input.find("UP") != std::string::npos){
                    moveFwd();
                }else if(input.find("LEFT") != std::string::npos){
                    moveLeft();
                }else if(input.find("RIGHT") != std::string::npos){
                    moveRight();
                }else if(input.find("DOWN") != std::string::npos){
                    moveBack();
                }else if(input.find("A") != std::string::npos){
                    vvDance();
                }else if(input.find("FIRE") != std::string::npos){
                    moveStop();
                }
            }

			cout.flush();
			sleep(1);
		}

		clientsock->close();

	}
}

// Control CliniBot with WASD
void controlTerminal(){
    // Show movement controls
    showControls();

    // Curses settings
    initscr();      // Init curses !!! Anything that gets printed after this will be printed weirdly
    cbreak();       // Sets that the code buffers per-key and not per newline
    noecho();       // Don't print the characters entered
    timeout(1750);  // Check once every 1,750ms

    // Control the robot
    while (true){
        int userIn = getch();   // Request a single character from the user
        refresh();              // Refresh the screen

        // Check the input of the user
        if (userIn == 'w'){
            moveFwd();
       }else if(userIn == 'a'){
           moveLeft();
       }else if(userIn == 'd'){
           moveRight();
       }else if(userIn == 's'){
            moveBack();
       }else if(userIn == 'e'){
           break;
       }else if(userIn == 'p'){
            moveStop();
        }
     }
}

// Let CliniBot follow a grid
void controlGrid(){
    int measurement = 0;
    // Check if the battery is still sufficiently charged, else shutdown
    if (BP.get_voltage_battery() >= 9) {
		while (true) {
            //cout << BP.get_sensor(PORT_2, Ultrasonic2) << endl;
            //cout << Ultrasonic2.cm << endl;

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
							moveFwd(100000);
							//rechtdoor
						}
						else if (Light3.reflected > 1800 && Light3.reflected < 2000) {
                            if(enableDebug){
                                cout << "wit" << endl;
                            }
							moveLeft(100000);
							//als ie het wit in gaat
						}
						else if (Light3.reflected > 2200) {
                            if(enableDebug){
                                cout << "zwart" << endl;
                            }
                            if(isCrossing()){
                                moveStop();
                                sleep(1);
                                checkGrid();
                            }

							moveRight(100000);

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

// Debug menu
void debug(){
    cout << "VV DEBUG" << endl;
    cout << "a - turn left 90 degrees" << endl << "t - check if there is a crossing"<< endl << "o - check if there is an obstacle" << endl << "oa - check all obstacles" << endl << "tv - test values" << endl;
    cout << "cr - look left and right" << endl;

    while(true){
        cout << endl << "> ";
        string uin;
        cin >> uin;
        if(uin == "a"){
            turnLeft();
        }else if(uin == "t"){
            cout << "Crossing > ";
            if(isCrossing()){
                cout << "Yes." << endl;
            }else{
                cout << "No." << endl;
            }
            cout << "Line > ";
            if(lineDetected()){
                cout << "YES" << endl;
            }else{
                cout << "NO" << endl;
            }
        }else if(uin == "o"){
            cout << "Route > ";
            if(obstacleDetected()){
                cout << "Blocked." << endl;
            }else{
                cout << "Clear." << endl;
            }
        }else if(uin == "oa"){
            checkGrid();
        }else if(uin == "tv"){
            testValues();
        }else if(uin == "cr"){
            lookLeft();
            cout << "Looking left" << endl;
            sleep(1);
            lookRight();
            cout << "Looking forward" << endl;
            sleep (1);
            lookRight();
            cout << "Looking right" << endl;
            sleep(1);
            lookLeft();
            cout << "Looking forward again" << endl;
        }else{
            return;
        }
    }

}

// Main execution
int main(){
    cout << "[INIT] Starting..." << endl;

    // Initialize the ctrl + c catch
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = eHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Check the voltage levels
    cout << "Checking voltage..." << endl;
    int cvoltage = BP.get_voltage_battery();
    if (cvoltage < 10) {
        cout << "[ERROR] BATTERY CRITICAL!" << endl;
        cout << "Voltage Level: " << cvoltage << endl;
        BP.reset_all();
        exit(0);
    }else{
        cout << "[INFO] Battery levels sufficient" << endl;
        cout << "Voltage Level: " << cvoltage << endl;
    }


    cout << "Setting up sensors..." << endl;

    BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

    BP.set_motor_limits(PORT_B, 30, 0);
    BP.set_motor_limits(PORT_C, 30, 0);
    BP.set_motor_limits(PORT_D, 90, 0);

    cout << "[INIT] Complete!" << endl;

    // Request debugging options
    string doption;
    while(true){
        cout << "Would you like to enable debugging options(y/n) $";
        cin >> doption;
        if(doption == "y"){
            enableDebug = true;
            break;
        }else if(doption == "n"){
            enableDebug = false;
            break;
        }
    }

    // Show menu
    cout << "Enter 'move' to control the robot via this terminal, enter 'bt' to control the robot via bluetooth, enter 'grid' to let the robot follow a grid, or enter 'debug' to enter debug menu." << endl;
    string userChoice;
    cin >> userChoice;
    if(userChoice == "bt"){
        controlBluetooth();
    }else if(userChoice == "debug"){
        debug();
    }else if(userChoice == "move"){
        controlTerminal();
    }else if(userChoice == "grid"){
        controlGrid();
    }

    BP.reset_all();
    cout << "Program Terminated." << endl;
    return 0;

}
