#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

// 1 megabyte
#define Meg1 1024L*1024

void setup(void) {
	// set SD ring start/end sector 
	// prepare SD flash card using disk editor 
	// and ctools/genimage as described in docs
	SDx.ring.start = 7600;
	// + 1 megabyte for ring buffer
	SDx.ring.end = SDx.ring.start + Meg1 / SD_LOW::sectorsz; 

	Serial.begin(115200);
	if (SDx.begin()) {
		SDx.read(SDx.ring.start); SDx.dump();
		memset(SDx.buf.b, 0xFF, SD_LOW::sectorsz);
		SDx.write(SDx.ring.start); SDx.dump();
	} // SD.begin ok
}

void loop(void) {
}
