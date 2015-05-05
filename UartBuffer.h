#ifndef _H_UartBuffer
#define _H_UartBuffer

#include "config.h"

#include <Arduino.h>

class UartBuffer {
	char *buf; uint8_t bufsz; uint8_t ptr;			// buf collecting eoled line
	HardwareSerial &uart; uint16_t baud;			// serial port config
	void (*callback)(char channel,char *,int sz);	// will be called on eol/buf
	char channel;									// channel id
public:
	UartBuffer(
			void (*_callback)(char channel,char *,int),
			char channel,
			int sz,
			HardwareSerial& uart,
			int baud);
	// callback: function will be called on eol (end of line)
	// sz: buffer size
	// uart: Serial, Serial1, Serial2,..
	// baud: uart bps 4800,9600,..
	~UartBuffer();
	void poll(void);		// process
};

#endif // _H_UartBuffer
