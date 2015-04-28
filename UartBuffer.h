#ifndef _H_UartBuffer
#define _H_UartBuffer

#include <Arduino.h>

class UartBuffer {
	int ptr;
	char *buf; int bufsz;
	HardwareSerial &uart; int baud;
	void (*callback)(char channel,char *,int sz);
	char channel;
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
	void poll(void);	// process
};

#endif // _H_UartBuffer
