#define RING_IMG_FIRST_HW_SECTOR 7600

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

#include "UartBuffer.h"

void CallBack(char channel, char *buf, int sz) {
//	uint8_t c[]={channel};
//	SDx.ring_append(c,sizeof(c));
//	SDx.ring_append(buf, sz);
	Serial.print(channel); Serial.print(':');
	for (int i=0;i<sz;i++) Serial.print(buf[i]);
	Serial.println();
}

UartBuffer   gps(CallBack,'g',88,Serial1,4800);
UartBuffer sonar(CallBack,'s',88,Serial2,4800);

void setup(void) {
	Serial.begin(115200);

	// set SD ring start/end sector 
	// prepare SD flash card using disk editor 
	// and ctools/genimage as described in docs
	SDx.ring.start = RING_IMG_FIRST_HW_SECTOR;
	// + 1 megabyte for ring buffer
	SDx.ring.end = SDx.ring.start + Meg1 / SD_LOW::sectorsz; 
	// clean ring buffer
	SDx.ring_clean();

	// start SD
	if (SDx.begin()) {
		/*
		SDx.read(SDx.ring.start); SDx.dump();
		memset(SDx.buf.b, 0xFF, SD_LOW::sectorsz);
		SDx.write(SDx.ring.start); SDx.dump();
		*/
	} // SD.begin ok
}

void loop(void) {
	gps.poll();
	sonar.poll();
}
