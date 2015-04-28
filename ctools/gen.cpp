#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>

#include <iostream>
using namespace std;

#include "emCpp/CRC.h"

CRC7 crc7; // crc7
CRC	 ccc7; // for table generator test

int main() {
	// dump generated CRC table
	ccc7.dumptable();
	// SD SPI commands
	// cmdX
	char cmdX[]={0x40|17,0x00,0x00,0x00,0x00};
	char crc=crc7.crc(cmdX,sizeof(cmdX));
	printf("\ncmd%i crc7:%.2X crc7<<1|1:%.2X\n\n",cmdX[0]&(!0x40),crc,crc<<1|1);
	char ccc=ccc7.crc(cmdX,sizeof(cmdX));
	printf("\ncmd%i ccc7:%.2X ccc7<<1|1:%.2X\n\n",cmdX[0]&(!0x40),ccc,ccc<<1|1);
//	// make test sector
//	for (uint32_t i=1;i<sizeof(buf)+1;i++) buf[i-1]=i;
//	FILE *fh=fopen("test512.bin","wb");
//	fwrite(buf,sizeof(buf),sizeof(buf[0]),fh);
//	fclose(fh);
	return 0;
}
