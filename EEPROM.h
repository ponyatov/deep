#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <avr/eeprom.h>

#define EEPTR unsigned char *

uint32_t ee_getuint32(EEPTR addr);
void ee_setuint32(EEPTR addr, uint32_t value);

#endif // EEPROM_H_
