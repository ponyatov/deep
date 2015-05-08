#include "EEPROM.h"

uint32_t ee_getuint32(EEPTR addr) {
	return
		eeprom_read_byte(addr + 0) << 24 |
		eeprom_read_byte(addr + 1) << 16 |
		eeprom_read_byte(addr + 2) <<  8 |
		eeprom_read_byte(addr + 3);
}

void ee_setuint32(EEPTR addr, uint32_t value) {
	eeprom_write_byte(addr + 0,value << 24);
	eeprom_write_byte(addr + 1,value << 16);
	eeprom_write_byte(addr + 2,value <<  8);
	eeprom_write_byte(addr + 3,value	  );
}
