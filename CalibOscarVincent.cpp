int blackHigh = 0;
int blackLow = 1000000;
int whiteHigh = 0;
int whiteLow = 10000000;
int RGBBlackLow = 1000000;
int RGBBlackHigh = 0;
int RGBWhiteLow = 1000000;
int RGBWhiteHigh = 0;

void Calibration() {
	int time = 4000;
	int average;
	char input;
	while (true) {
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