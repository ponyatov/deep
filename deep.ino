#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

// 1 megabyte
#define Meg1 1024L*1024

// ring buffer pointers
uint32_t ring_r,ring_w;

// gps buffer
uint16_t gps_ptr;
uint8_t gps_buf[SD_LOW::sectorsz];

// depth buffer


void setup(void) {
	// set SD ring start/end sector 
	// prepare SD flash card using disk editor 
	// and ctools/genimage as described in docs
	SDx.ring.start = 7600;
	// clean ring buffer
	ring_r=SDx.ring.start;
	ring_w=SDx.ring.start;
	// + 1 megabyte for ring buffer
	SDx.ring.end = SDx.ring.start + Meg1 / SD_LOW::sectorsz; 

	Serial.begin(115200); // uart0
	
	// init gps
	Serial1.begin(4800); // gps channel
	gps_ptr=0;
	
	// init depth
	Serial2.begin(4800); // depth channel
	
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
}
