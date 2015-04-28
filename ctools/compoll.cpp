#include <windows.h>
#include <iostream>
#include <assert.h>

using namespace std;

#include <emCpp/UART.h>

UART sp("\\\\.\\COM5",4800);

char buf[]="DD.MM.YYYY HH:MM:SS";

int main() {
	cout << endl << "port: " << sp.SymName << endl << endl;
	while ( sp.send(buf,sizeof(buf)) ) Sleep(1000);
	return 0;
}
