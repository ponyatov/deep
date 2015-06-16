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
	enabled=true;								// enable callback calling
}

UartBuffer::~UartBuffer() {
	delete[] buf;
}

void UartBuffer::poll(void) {
	if (uart.available() >= SERIAL_RX_BUFFER_SIZE-2) {
		DEBUG_UART.print("d:UART "); DEBUG_UART.print(uart); DEBUG_UART.println(" overflow");
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
	enabled = !enabled;
	DEBUG_UART.print("d: channel ");
	DEBUG_UART.print(channel);
	DEBUG_UART.print(" ");
	if (enabled) 
	  DEBUG_UART.println("on");
	else
		DEBUG_UART.println("off");
	return enabled;
}
