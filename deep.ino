#define RING_IMG_FIRST_HW_SECTOR 7600
#define PIN_BT_READY 7

#include <avr/sleep.h>

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

#include "UartBuffer.h"

void CallBack(char channel, char *buf, int sz) {
	Serial.print(channel); Serial.print(':');
	for (int i=0;i<sz;i++) Serial.print(buf[i]); // also prints CR
	if (digitalRead(PIN_BT_READY)) {
		// online
		Serial.println("BTR");
	} else {
		// offline
		Serial.println("offline");
	}
}

UartBuffer   gps(CallBack,'g',88,Serial1,4800);
UartBuffer sonar(CallBack,'s',88,Serial2,4800);
UartBuffer extra(CallBack,'x',88,Serial3,4800);

void setup(void) {
	Serial.begin(115200);
	
	// setup pin to BT ready signal with pull-up resistor
	pinMode(PIN_BT_READY,INPUT); digitalWrite(PIN_BT_READY,HIGH);

	// set SD ring start/end sector 
	// prepare SD flash card using disk editor 
	// and ctools/genimage as described in docs
	SDx.ring.start = RING_IMG_FIRST_HW_SECTOR;
	// + 1 megabyte for ring buffer
	SDx.ring.end = SDx.ring.start + Meg1 / SD_LOW::sectorsz; 
	// clean ring buffer
	SDx.ring_clean();

	// start SD
	if (!SDx.begin()) {
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		//sleep_bod_disable();
		sleep_enable();
		cli();
		for (;;) sleep_mode();
	} // SD.begin ok
}

void loop(void) {
	gps.poll();
	sonar.poll();
	extra.poll();
}
