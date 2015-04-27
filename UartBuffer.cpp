#include "UartBuffer.h"

UartBuffer::UartBuffer(
		uint8_t sz,
		HardwareSerial& _uart,
		uint16_t _baud) : uart(_uart) {
	ptr = 0;
	buf = new char(sz);
	baud = _baud; uart.begin(baud);
}

UartBuffer::~UartBuffer() {
	delete[] buf;
}
