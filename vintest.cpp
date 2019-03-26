#include <iostream>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <curses.h>    // only getting 1 char from input
#include "BrickPi3.h"   // for BrickPi3
#include <string>

using std::string;
using std::cin;
using std::cout;
using std::endl;

void exit_signal_handler(int signo);
BrickPi3 BP;

void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // Zet stroom van poort B en C op 0, waardoor de robot stopt.

    cout << "I AM NOT MOVING" << endl;
}

void moveFwd(){
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    // Draai de motor op port B en C 360 graden
    cout << "I AM MOVING FORWARD" << endl;

}

void moveLeft(){
    BP.set_motor_position_relative(PORT_B, 270);
    BP.set_motor_position_relative(PORT_C, -270);
    // Draai het wiel op port B 270 graden en de wiel op port C -270 graden

    cout << "I AM MOVING LEFT" << endl;

}

void moveRight(){
    BP.set_motor_position_relative(PORT_B, -270);
    BP.set_motor_position_relative(PORT_C, 270);
    // Draai het wiel op port B -270 graden en de wiel op port C 270 graden

    cout << "I AM MOVING RIGHT" << endl;

}

void moveBack(){
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    // Draai de motor op port B en C -360 graden

    cout << "I AM MOVING BACKWARDS" << endl;

}

void showControls(){
    cout << "Controls: " << endl << "W - Forwards" << endl << "A - Left" << endl << "S - Backwards" << endl << "D - Right" << endl << endl << "Press enter to continue...";
    string cinput;
    cin >> cinput;
    
}
int main()
{
    showControls();
    initscr();
    cbreak();
    noecho();
    timeout(1750);
    signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
    while (true){
        sleep(1);
        int userIn = getch();
        refresh();
        //cout << "Hold 'w' to move forward!" << endl;

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
        }else{
           moveStop();
       }
     }


    cout << "Program Ended" << endl;
    return 0;

}


// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
