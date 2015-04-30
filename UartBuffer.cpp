#include "UartBuffer.h"

UartBuffer::UartBuffer(
		void (*_callback)(char channel,char *buf,int sz),
		char _channel,
		int sz,
		HardwareSerial& _uart,
		int _baud) : uart(_uart) {
	callback = _callback; channel=_channel;
	ptr = 0;
	buf = new char[sz]; bufsz=sz;
	baud = _baud; uart.begin(baud);
}

UartBuffer::~UartBuffer() {
	delete[] buf;
}

extern void halt(void);
#include <USBAPI.h>

void UartBuffer::poll(void) {
	if (uart.available() >= SERIAL_RX_BUFFER_SIZE - 0x10) {
		Serial.println();
		Serial.print(uart);
		Serial.println(" overflow\n");
		halt();
	}
	if (uart.available()) {
		buf[ptr++] = uart.read();
		if (buf[ptr - 1] == 0x0D)
			ptr--; // ignore win/dos \r char
		if (buf[ptr - 1] == 0x0A | ptr >= bufsz) { // EOL | buf full
			callback(channel, buf, ptr);
			ptr = 0;
		}
	}
}
