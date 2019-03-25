
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

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

//Stop
void stop(void)
{
	 BP.set_motor_power(PORT_B, 0);
	 BP.set_motor_power(PORT_C, 0);
}

//Move Forward
void fwd(void)
{
	 BP.set_motor_power(PORT_B, 20);
	 BP.set_motor_power(PORT_C, 20);
	 usleep(500000);
	 BP.set_motor_power(PORT_B, 40);
	 BP.set_motor_power(PORT_C, 40);
	 usleep(500000);
	 BP.set_motor_power(PORT_B, 50);
	 BP.set_motor_power(PORT_C, 50);
	 sleep(2);
	 stop();

}
//Move Left
void left(void)
{
	 BP.set_motor_position_relative(PORT_B, 270);
	 BP.set_motor_position_relative(PORT_C, -270);
}
//Move Right
void right(void)
{
	 BP.set_motor_position_relative(PORT_B, -270);
	 BP.set_motor_position_relative(PORT_C, 270);
}
//Move backward
void back(void)
{
	 BP.set_motor_dps(PORT_B, -360);
	 BP.set_motor_dps(PORT_C, -360);
	 sleep(3);
	 stop();
}

int main()
{
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C
  	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
  	BP.set_motor_limits(PORT_B, 60, 0);
	BP.set_motor_limits(PORT_C, 60, 0);
  	char inp;

	while(true)
	{
		cout << "Press f (forward), b (backward), l(left), r (right), s (stop): " << endl;
		cin >> inp;	//Take input from the terminal
		//Move the bot
		if(inp=='f') {
			  	fwd();
			}

		else if (inp=='b') {
			  	back();
			}

		else if (inp=='l'){
				left();
			}
		else if (inp=='r'){
				right();
			}
		else if (inp=='s'){
				stop();
			}




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