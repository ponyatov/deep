#include "EEPROM.h"

uint32_t ee_getuint32(EEPTR addr) {
	return
		eeprom_read_byte(addr + 3) << 24 |
		eeprom_read_byte(addr + 2) << 16 |
		eeprom_read_byte(addr + 1) <<  8 |
		eeprom_read_byte(addr + 0);
}

void ee_setuint32(EEPTR addr, uint32_t value) {
	eeprom_write_byte(addr + 3,(value >> 24)&0xFF);
	eeprom_write_byte(addr + 2,(value >> 16)&0xFF);
	eeprom_write_byte(addr + 1,(value >>  8)&0xFF);
	eeprom_write_byte(addr + 0,(value >>  0)&0xFF);
}
