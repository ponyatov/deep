#include "UartBuffer.h"

UartBuffer::UartBuffer(
		void (*_callback)(char channel,char *buf,int sz),
		char _channel,
		int sz,
		HardwareSerial& _uart,
		int _baud) : uart(_uart) {
	callback = _callback; channel=_channel;
	buf = new char[sz]; bufsz=sz; ptr = 0;		// alloc & reset buffer
	baud = _baud; uart.begin(baud);				// init hw serial port
	enabled=true;								// enable by default
}

UartBuffer::~UartBuffer() {
	delete[] buf;
}

void UartBuffer::poll(void) {
	if (uart.available() >= SERIAL_RX_BUFFER_SIZE-2) {
		Serial.print("d:UART "); Serial.print(uart); Serial.println(" overflow");
	}
	if (uart.available()) {
		buf[ptr++] = uart.read();
		if (buf[ptr - 1] == 0x0D) ptr--; // ignore win/dos \r char
		if (buf[ptr - 1] == EOL | ptr >= bufsz) { // EOL | buf full
			if (enabled) callback(channel, buf, ptr);	// callback function
			ptr = 0;						// reset buffer
		}
	}
}

bool UartBuffer::toggle(void) {
	enabled != enabled;
	return enabled;
}
