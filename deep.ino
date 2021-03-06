#include "config.h"

#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx(EOL);	// SD ring with EOL padding

#include "UartBuffer.h"
#include "TimerOne.h"

#include <avr/sleep.h>
void halt(void) {
	DEBUG_UART.println("d:halt"); DEBUG_UART.flush();
	// shutdown
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_bod_disable();
	sleep_enable();
	cli();
	for (;;) sleep_mode();
}
void reset(void) {
	DEBUG_UART.println("d:reset"); DEBUG_UART.flush();
	// soft reset
	void (*rs)(void)=0; rs();
}	

void SendBT(char *buf, int sz) {
	for (int i = 0; i < sz; i++)
		DEBUG_UART.print(buf[i]); // also prints CRs
}
void SendBT(char channel, char *buf, int sz) {
	DEBUG_UART.print(channel); DEBUG_UART.print(':');	// channel id
	SendBT(buf,sz);
}

void SendSD(char channel, char *buf, int sz) {
	SDx.ring_append(&channel,1);
	char colon=':'; SDx.ring_append(&colon,1);
	SDx.ring_append(buf,sz);
}

uint32_t tick_count = 0;
bool volatile tick_SD_flag = false;
void tick(void) { // every sec
	tick_count++;
	if (tick_count % (60 * 10) == 0) // every 10 min: 60 * 10
		tick_SD_flag = true;
}

UartBuffer   gps(UartCallBack,'g',NMEA_MAX_MESSAGE_SZ,Serial1,4800);
UartBuffer sonar(UartCallBack,'s',NMEA_MAX_MESSAGE_SZ,Serial2,4800);
#ifndef DEBUG_UART3
UartBuffer extra(UartCallBack,'x',NMEA_MAX_MESSAGE_SZ,Serial3,9600);
#endif

bool BT_FLAG_PREV, BT_FLAG_NOW = true;

void BT_cmdline(void) {
	if (DEBUG_UART.available()) { // has data in comand line
		switch (DEBUG_UART.read()) {
		case 'c':
			DEBUG_UART.println("d: cmd cold start SD ring");
			SDx.ring_coldstart();
			break;
		case 'r':
			DEBUG_UART.println("d: cmd reset SD ring");
			SDx.ring_reset();
			break;
		case 's':
			DEBUG_UART.println("d: cmd save SD ring ptrs");
			SDx.ring_rwptr_save();
			break;
		case 'l':
			DEBUG_UART.println("d: cmd load SD ring ptrs");
			SDx.ring_rwptr_load();
			break;
		case 'G':
			DEBUG_UART.println("d: cmd toggle GPS channel");
			gps.toggle();
			break;
		case 'S':
			DEBUG_UART.println("d: cmd toggle sonar channel");
			sonar.toggle();
			break;
#ifndef DEBUG_UART3
    case 'X':
      DEBUG_UART.println("d: cmd toggle extra channel");
      extra.toggle();
      break;
#endif
		case 'z':
			reset();
			break;
		}
	}
}

void BT_poll(void) {
	BT_FLAG_PREV = BT_FLAG_NOW;
	BT_FLAG_NOW = digitalRead(PIN_BT_READY);
	if ( BT_FLAG_NOW & !BT_FLAG_PREV) {
		DEBUG_UART.println("d:BT ready");
		SDx.ring_flush();
	}
	if (!BT_FLAG_NOW & BT_FLAG_PREV) {
		DEBUG_UART.println("d:offline");
	}
	BT_cmdline();
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

void setup(void) {
	DEBUG_UART.begin(DEBUG_UART_BAUD);
	
	// setup pin to BT ready signal with pull-up resistor
	pinMode(PIN_BT_READY,INPUT); digitalWrite(PIN_BT_READY,HIGH);

	// start SD (including ring reload from EEPROM)
  while (!SDx.begin()) DEBUG_UART.println("d: SD init retry");
	SDx.ring_reset();
  //SDx.ring_coldstart();

	// start SD ring backuping timer
	Timer1.initialize(1000000L); // default 1 sec
	Timer1.attachInterrupt(tick);	// start ticker
}

char SD_poll_buf[NMEA_MAX_MESSAGE_SZ]; uint16_t SD_poll_ptr=0;
void SD_poll(void) {
	if (BT_FLAG_NOW) { // BT_FLAG
	// call it if has active BT uplink and SD buffered data
	if (SDx.ring_hasData()) {
		SD_poll_buf[SD_poll_ptr++] = SDx.ring_nextchar();
//		SendBT(SDx.ring_read(),SDx.sectorsz);	// sector-block histoiry, produces UART overflows
	}
	// check string ready or buffer full
	if (SD_poll_ptr > sizeof(SD_poll_buf) | SD_poll_buf[SD_poll_ptr - 1] == EOL) {
		if (SD_poll_ptr > 1) { // non empty string (in padded SD sector)
			SD_poll_buf[SD_poll_ptr - 1] = 0x00; // set end of string char
			// replace channel markers
			if (SD_poll_buf[1]==':')
			switch (SD_poll_buf[0]) {
				case 'g': SD_poll_buf[0] = 'G'; break;
				case 's': SD_poll_buf[0] = 'S'; break;
				case 'x': SD_poll_buf[0] = 'X'; break;
			} 
			DEBUG_UART.println(SD_poll_buf);		// send to BT
		}
		SD_poll_ptr = 0;
	}
	} // BT_FLAG
	if (tick_SD_flag) {				// timered SD ring pointers autosave
		tick_SD_flag = false;
		SDx.ring_rwptr_save();
	}
}


void loop(void) {
	// poll BT state & command line
	BT_poll();
	// poll SD ringed history data 
	SD_poll();
	// poll serial channels
	gps.poll();
	sonar.poll();
 #ifndef DEBUG_UART3
  extra.poll();
 #endif
}

