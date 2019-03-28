#include <iostream>
#include <string>
using namespace std;
using std::getline;

int main(){

cout << " _______________________________________________" << endl;
cout << "|                                               |" << endl;
cout << "|        Hoe wilt u de robot gebruiken?         |" << endl;
cout << "|                                               |" << endl;
cout << "| Keuze 1: De robot besturen met 'WASD'.        |" << endl;
cout << "| Keuze 2: De robot besturen via Bluetooth.     |" << endl;
cout << "| Keuze 3: De robot over een lijn laten rijden. |" << endl;
cout << "| Keuze 4: De robot over een grid laten rijden. |" << endl;
cout << "|                                               |" << endl;
cout << "|     Selecteer uw keuze met 1, 2, 3 of 4.      |" << endl;
cout << "|_______________________________________________|" << endl;
cout << endl;

while(true){
string Keuze;
getline(cin,Keuze);

if(Keuze == "1"){
    cout << "WASD programma" << endl;
}
else if(Keuze == "2"){
    cout << "Bluetooth programma" << endl;
}
else if(Keuze == "3"){
    cout << "Lijn rijden" << endl;
}
else if(Keuze == "4"){
    cout << "Grid rijden" << endl;
}
else{
    cout << "Geen geldige keuze! Probeer opnieuw..." << endl;
}
}

}
