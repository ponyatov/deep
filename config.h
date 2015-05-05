// scetch configuration

#ifndef _H_CONFIG_
#define _H_CONFIG

// set SD ring start/end sector
// prepare SD flash card using disk editor
// and ctools/genimage as described in docs
#define SD_RING_IMG_FIRST_HW_SECTOR 7600
#define SD_RING_IMG_SIZE (10*1024L)

#define PIN_BT_READY 7

#define NMEA_MAX_MESSAGE_SZ 88

extern void halt(void);

#define EOL 0x0A

#endif // _H_CONFIG
