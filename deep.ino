#include "config.h"

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx(EOL);	// SD ring with EOL padding

#include "UartBuffer.h"
#include "TimerOne.h"

#include <avr/sleep.h>
void halt(void) {
	Serial.println(); Serial.println("halt"); Serial.flush();
	// shutdown
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_bod_disable();
	sleep_enable();
	cli();
	for (;;) sleep_mode();
}

void SendBT(char *buf, int sz) {
	for (int i = 0; i < sz; i++)
		Serial.print(buf[i]); // also prints CRs
}
void SendBT(char channel, char *buf, int sz) {
	Serial.print(channel); Serial.print(':');	// channel id
	SendBT(buf,sz);
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
	if ( BT_FLAG_NOW & !BT_FLAG_PREV) {
		Serial.println(); Serial.println("BT ready");
		SDx.ring_flush();
	}
	if (!BT_FLAG_NOW &  BT_FLAG_PREV) {
		Serial.println(); Serial.println("offline");
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

UartBuffer   gps(UartCallBack,'g',NMEA_MAX_MESSAGE_SZ,Serial1,4800);
UartBuffer sonar(UartCallBack,'s',NMEA_MAX_MESSAGE_SZ,Serial2,4800);
UartBuffer extra(UartCallBack,'x',NMEA_MAX_MESSAGE_SZ,Serial3,4800);

//unsigned int tick_count = 0;
//bool volatile tick_flag = false;
//void tick(void) { // every sec
//	tick_count++;
//	if (tick_count % (1 * 10) == 0) // 10 min
//		tick_flag = true;
//}
//void tick_poll(void) {
//	if (tick_flag) {
//		tick_flag=false;
//	}
//}
//
//////		SDx.ring_rwptr_save();
////		Serial.println();
////		 Serial.println(tick_count);
////		Serial.println();
////		Serial.println("r/w=");
////		Serial.println(SDx.ring.r);
////		Serial.println(SDx.ring.w);
////		Serial.println();
////		Serial.println();
////	}
////}

void setup(void) {
	Serial.begin(115200);
	
	// setup pin to BT ready signal with pull-up resistor
	pinMode(PIN_BT_READY,INPUT); digitalWrite(PIN_BT_READY,HIGH);

	// start SD (including ring reload from EEPROM)
	if (!SDx.begin()) halt();
	SDx.ring_coldstart();
	
//	// start SD ring backuping timer
//	Timer1.initialize(1000000L); // default 1 sec
////	Timer1.attachInterrupt(tick);	// start ticker
}

void SD_poll(void) {
	// if has active BT uplink and SD buffered data
	if (BT_FLAG_NOW & SDx.ring_hasData())
		SendBT(SDx.ring_read(), SDx.sectorsz);
}

void loop(void) {
//	// timer poll
//	tick_poll();
	// poll BT state
	BT_poll();
	// poll SD ringed history data 
	SD_poll();
	// poll serial channels
	gps.poll();
	sonar.poll();
	extra.poll();
}
