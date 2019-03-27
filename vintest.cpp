// Includes
#include <iostream>      // Basic IO
#include <unistd.h>      // for usleep and sleep(for linux)
#include <signal.h>      // for catching exit signals
#include <curses.h>      // Library to be able to rea
#include "BrickPi3.h"    // BrickPi3 Header file - used to control the ROBO-MAN
#include <string>        // String variables
#include "BluetoothSocket.h" //Bluetooth
// Using statements
using std::string;
using std::cin;
using std::cout;
using std::endl;


void exit_signal_handler(int signo); // The exit handler definition. Used to catch 'ctrl+c' to terminate execution

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

// Stop the robot by setting the motor power to '0'
void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // Zet stroom van poort B en C op 0, waardoor de robot stopt.

    cout << " Stopped - ";
}


void moveFwd() {
	BP.set_motor_dps(PORT_B, 180);
	BP.set_motor_dps(PORT_C, 180);
	// Draai de motor op port B en C 360 graden
	cout << " Forward - ";

}

void moveLeft() {

		BP.set_motor_dps(PORT_B, 80);
		BP.set_motor_dps(PORT_C, -80);
		/*BP.set_motor_position_relative(PORT_B, 45);
		BP.set_motor_position_relative(PORT_C, -45);*/

	cout << " Left - ";

}

void moveRight() {
		BP.set_motor_dps(PORT_B, -80);
		BP.set_motor_dps(PORT_C, 80);
		/*BP.set_motor_position_relative(PORT_B, -45);
		BP.set_motor_position_relative(PORT_C, 45);*/

	cout << " Right - ";

}

void moveBack() {
	BP.set_motor_dps(PORT_B, -360);
	BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden

	cout << " Back - ";

}
void turnLeft(){
    BP.set_motor_position_relative(PORT_B, 405);
    BP.set_motor_position_relative(PORT_C, -405);
    //should be 90 degrees
}

bool isCrossing(){
    // check if other sensor is black
    sensor_color_t      Color1;
    //sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light3;

    int measurement = 0;
    bool s1 = false;
    bool s2 = false;
    if (BP.get_sensor(PORT_2, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(measurement > 150 && measurement < 240){
            s1 = true;
        }

    }
    if (BP.get_sensor(PORT_2, Light3) == 0) {
        measurement = Light3.reflected;
        if(measurement >2300){
            s2 = true;
        }

    }
    if(s1 && s2){
        return true
    }else{
        return false;
    }



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
    sleep(10);
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    usleep(500000);
}


// Show the movement controls on-screen
void showControls(){
    cout << "Controls: " << endl << "W - Forwards" << endl << "A - Left" << endl << "S - Backwards" << endl << "D - Right" << endl << endl << "Press enter to continue...";
    string cinput;
    cin >> cinput;

}
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
            cout << "Executing Action: " << input << endl;
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
                // cout << endl << input << endl;
            }

			//doe andere dingen.
			cout.flush();
			sleep(1); // wacht 500 ms
		}

		clientsock->close();

	}
}

void controlTerminal(){
    // Show movement controls
    showControls();

    // Curses settings
    initscr();      // Init curses !!! Anything that gets printed after this will be printed weirdly
    cbreak();       // Sets that the code buffers per-key and not per newline
    noecho();       // Don't print the characters entered
    timeout(1750);  // Check once every 1,750ms
    signal(SIGINT, exit_signal_handler);    // register the exit function for Ctrl+C

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

void debug(){
    cout << "VV DEBUG" << endl;
    cout << "a - turn left 90 degrees" << endl << "t - check if there is a crossing";
    while(true){
        cout << endl << "> ";
        string uin;
        cin >> uin;
        if(uin == "a"){
            turnLeft();
        }else if(uin == "t"){
            cout << "Crossing > ";
            if(isCrossing()){
                cout << "true" << endl;
            }else{
                cout << "false" << endl;
            }
        }else{
            return;
        }
    }

}
// Main execution
int main()
{
    cout << "Enter 'move' to control the robot via this terminal, enter 'bt' to control the robot via bluetooth or enter 'crossing' to navigate over a grid." << endl;
    string userChoice;
    cin >> userChoice;
    if(userChoice == "bt"){
        controlBluetooth();
    }else if(userChoice == "crossing"){
        debug();
    }else if(userChoice == "move"){
        controlTerminal();
    }

    cout << "Program Terminated." << endl;
    return 0;

}


// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
