#ifndef _H_UartBuffer
#define _H_UartBuffer

#include <Arduino.h>

class UartBuffer {
	uint8_t ptr;
	char *buf;
	HardwareSerial &uart; uint16_t baud;
public:
	UartBuffer(uint8_t sz, HardwareSerial& uart, uint16_t baud);
	// sz: buffer size
	// uart: Serial, Serial1, Serial2,..
	// baud: uart bps 4800,9600,..
	~UartBuffer();
};

#endif // _H_UartBuffer
