
#include <avr/sleep.h>

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

#include "config.h"

#include "UartBuffer.h"

#include "TimerOne.h"

void halt(void) {
	Serial.flush();
	// shutdown
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_bod_disable();
	sleep_enable();
	cli();
	for (;;) sleep_mode();
}

void SendBT(char channel, char *buf, int sz) {
	if (channel != 'h') {
		Serial.print(channel); Serial.print(':');
	}
	for (int i = 0; i < sz; i++)
		Serial.print(buf[i]); // also prints CR
}

void SendSD(char channel, char *buf, int sz) {
	SDx.ring_append(&channel,1);
	char colon=':'; SDx.ring_append(&colon,1);
	SDx.ring_append(buf,sz);
}

bool BT_FLAG_PREV, BT_FLAG_NOW = true;

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
		if (BT_FLAG_PREV) {
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

unsigned int tick_count = 0;
void tick(void) { // every sec
	tick_count++;
	if (tick_count % (1 * 10) == 0) // 10 min
		SDx.ring_rwptr_save();
}

void setup(void) {
	Serial.begin(115200);
	
	// setup pin to BT ready signal with pull-up resistor
	pinMode(PIN_BT_READY,INPUT); digitalWrite(PIN_BT_READY,HIGH);

	// start SD (including ring reload from EEPROM)
	if (!SDx.begin()) halt();
	SDx.ring_reset();
	
	// start SD ring backuping timer
	Timer1.initialize(1000000L); // default 1 sec
	Timer1.attachInterrupt(tick);	// start ticker
}

void SD_poll(void) {
	// if has active BT uplink and SD buffered data
	if (BT_FLAG_NOW & SDx.ring_hasData())
		SendBT('h', SDx.ring_poll(), SDx.sectorsz);
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
