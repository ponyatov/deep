#define RING_IMG_FIRST_HW_SECTOR 7600
#define RING_IMG_SIZE (10*1024L)
#define PIN_BT_READY 7

#include <avr/sleep.h>

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

#include "UartBuffer.h"

void SendBT(char channel, char *buf, int sz) {
	Serial.print(channel); Serial.print(':');
	for (int i=0;i<sz;i++) Serial.print(buf[i]); // also prints CR
}

void SendSD(char channel, char *buf, int sz) {
	SDx.ring_append(&channel,1);
	char colon=':'; SDx.ring_append(&colon,1);
	SDx.ring_append(buf,sz);
}

bool BT_FLAG_PREV,BT_FLAG_NOW=true;

void BT_poll(void) {
	BT_FLAG_PREV = BT_FLAG_NOW;
	BT_FLAG_NOW = digitalRead(PIN_BT_READY);
	if (BT_FLAG_NOW) {
		// flush SD ring on BT switch on
		if (!BT_FLAG_PREV) {
			Serial.println("BT ready");
			SDx.ring_flush();
		}
	} else {
		// on swith off BT
		if ( BT_FLAG_PREV) {
			Serial.println("offline");
		}
	}
}

void UartCallBack(char channel, char *buf, int sz) {
	if (BT_FLAG_NOW) {
		// online
		SendBT(channel, buf, sz);
	} else {
		// offline
		SendSD(channel, buf, sz);
	}
}

UartBuffer   gps(UartCallBack,'g',88,Serial1,4800);
UartBuffer sonar(UartCallBack,'s',88,Serial2,4800);
UartBuffer extra(UartCallBack,'x',88,Serial3,4800);

void setup(void) {
	Serial.begin(115200);
	
	// setup pin to BT ready signal with pull-up resistor
	pinMode(PIN_BT_READY,INPUT); digitalWrite(PIN_BT_READY,HIGH);

	// set SD ring start/end sector 
	// prepare SD flash card using disk editor 
	// and ctools/genimage as described in docs
	SDx.ring.start = RING_IMG_FIRST_HW_SECTOR;
	// + 1 megabyte for ring buffer
	SDx.ring.end = SDx.ring.start + RING_IMG_SIZE / SD_LOW::sectorsz;
	// set sector padding
	SDx.ring.wpadchar = 0x0A; // EOL
	// reset logger
	SDx.ring_reset();

	// start SD
	if (!SDx.begin()) {
		Serial.flush();
		// shutdown
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		//sleep_bod_disable();
		sleep_enable();
		cli();
		for (;;) sleep_mode();
	} // SD.begin ok
}

char hisbuf[88];
int hisptr=0;
void SD_poll(void) {
	// if has active BT uplink and SD buffered data
	if (BT_FLAG_NOW & SDx.ring_hasData()) {
		hisbuf[hisptr++] = SDx.ring_poll();
		if (
				hisbuf[hisptr - 1] == 0x0A |	/* EOL */
				hisptr > sizeof(hisbuf) |		/* buf end */
				!SDx.ring_hasData()				/* SD ring empty */
		) {
			SendBT('h', hisbuf, hisptr); hisptr = 0;
		}
	}
}

void loop(void) {
	// poll BT state
	BT_poll();
	// poll History data 
	SD_poll();
	// poll serial channels
	gps.poll();
	sonar.poll();
	extra.poll();
}
