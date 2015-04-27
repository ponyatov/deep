#include "UartBuffer.h"

UartBuffer::UartBuffer(
		void (*_callback)(char channel,char *buf,int sz),
		char _channel,
		int sz,
		HardwareSerial& _uart,
		int _baud) : uart(_uart) {
	callback = _callback; channel=_channel;
	ptr = 0;
	buf = new char[sz];
	baud = _baud; uart.begin(baud);
}

UartBuffer::~UartBuffer() {
	delete[] buf;
}

void UartBuffer::poll(void) {
	if (uart.available()) {
		buf[ptr]=uart.read();
		if (buf[ptr]==0x0A) { // EOL detected
			callback(channel,buf,ptr);
			ptr=0;
		} else ptr++;
	}
}
