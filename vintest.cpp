// Includes
#include <iostream>      // Basic IO
#include <unistd.h>      // for usleep and sleep(for linux)
#include <signal.h>      // for catching exit signals
#include <curses.h>      // Library to be able to rea
#include "BrickPi3.h"    // BrickPi3 Header file - used to control the ROBO-MAN
#include <string>        // String variables

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

    cout << "I AM NOT MOVING" << endl;
}

// Move wheel on port 'B' en 'C' 360 degrees forwards
void moveFwd(){
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    // Draai de motor op port B en C 360 graden
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
   // sleep(2);
>>>>>>> Stashed changes
=======
   // sleep(2);
>>>>>>> Stashed changes
    cout << "I AM MOVING FORWARD" << endl;

}

// Move wheel on port 'B' forwards by 270 degrees and move wheel on port 'C' backwards by 270 degrees
void moveLeft(){
    BP.set_motor_position_relative(PORT_B, 270);
    BP.set_motor_position_relative(PORT_C, -270);
    // Draai het wiel op port B 270 graden en de wiel op port C -270 graden
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
    //sleep(2);
>>>>>>> Stashed changes
=======
    //sleep(2);
>>>>>>> Stashed changes

    cout << "I AM MOVING LEFT" << endl;

}

// Move wheel on port 'C' forwards by 270 degrees and move wheel on port 'B' backwards by 270 degrees
void moveRight(){
    BP.set_motor_position_relative(PORT_B, -270);
    BP.set_motor_position_relative(PORT_C, 270);
    // Draai het wiel op port B -270 graden en de wiel op port C 270 graden
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
    //sleep(2);
>>>>>>> Stashed changes
=======
    //sleep(2);
>>>>>>> Stashed changes

    cout << "I AM MOVING RIGHT" << endl;

}

// Move the wheels on port 'B' en 'C' backwards 360 degrees
void moveBack(){
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    // Draai de motor op port B en C -360 graden
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
    //sleep(2);
>>>>>>> Stashed changes
=======
    //sleep(2);
>>>>>>> Stashed changes

    cout << "I AM MOVING BACKWARDS" << endl;

}

// Show the movement controls on-screen
void showControls(){
    cout << "Controls: " << endl << "W - Forwards" << endl << "A - Left" << endl << "S - Backwards" << endl << "D - Right" << endl << endl << "Press enter to continue...";
    string cinput;
    cin >> cinput;

}

// Main execution
int main()
{
    // Show movement controls
    showControls();
<<<<<<< HEAD
    initscr();
    cbreak();
    noecho();
<<<<<<< Updated upstream
<<<<<<< Updated upstream
    timeout(1750);
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    while (true){
=======
    timeout(1250);
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    while (true){
	//sleep(1);
>>>>>>> Stashed changes
=======
    timeout(1250);
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    while (true){
	//sleep(1);
>>>>>>> Stashed changes
        int userIn = getch();
        refresh();
        //cout << "Hold 'w' to move forward!" << endl;
=======

    // Curses settings
    initscr();      // Init curses !!! Anything that gets printed after this will be printed weirdly
    cbreak();       // Sets that the code buffers per-key and not per newline
    noecho();       // Don't print the characters entered
    timeout(1750);  // Check once every 1,750ms
    signal(SIGINT, exit_signal_handler);    // register the exit function for Ctrl+C

    while (true){
        int userIn = getch();   // Request a single character from the user
        refresh();              // Refresh the screen
>>>>>>> c9fce54e604f9ea0b21990b30fff0e006df8a86b

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
