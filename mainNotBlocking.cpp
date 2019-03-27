#include "BluetoothSocket.h"
#include <unistd.h>
using namespace std;

int main() {
	BluetoothServerSocket serversock(2, 1);  //2 is het channel-number
	cout << "listening" << endl;
	while(true) {
		BluetoothSocket* clientsock = serversock.accept();
		cout << "accepted from " << clientsock->getForeignAddress().getAddress() << endl;
		MessageBox& mb = clientsock->getMessageBox();
			
		string input;
		while(mb.isRunning()) {
			input = mb.readMessage();  //blokkeert niet
			if(input != "") cout << endl << input << endl;
			//doe andere dingen.
			cout << ".";
			cout.flush();
			usleep(500000); // wacht 500 ms
		}	
		
		clientsock->close();
			
	}
}
	