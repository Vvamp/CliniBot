
//Commands:
//	w-Move forward
//	a-Move left
//	d-Move right
//	s-Move back
//	x-Stop

#include "BrickPi3.h"   // for BrickPi3
#include <iostream>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <curses.h>    // only getting 1 char from input

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);


void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    cout << "Motor B and C stopped." << endl;
}
void moveFwd(){
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    sleep(10);
    moveStop();
}

void moveLeft(){
    BP.set_motor_position_relative(PORT_B, 270);
    BP.set_motor_position_relative(PORT_C, -270);
    sleep(2);
    moveStop();

}
void checkChar(int ichar){
               
		if(ichar == KEY_UP){
                      moveFwd();
                }else if(ichar == KEY_LEFT){
						moveLeft();
		}else{
			int ichar2 = getch();
			checkChar(ichar2);
		}


}
int main()
{
        signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

        BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.

        BP.set_motor_limits(PORT_B, 60, 0);
        BP.set_motor_limits(PORT_C, 60, 0);
	//moveFwd();
 	while(true)
        {
            cout << "Enter a key: " << endl;
            int ichar = getch();

	checkChar(ichar);
	}

        return 0;
}


// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}
