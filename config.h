// scetch configuration

#ifndef _H_CONFIG_
#define _H_CONFIG

// set SD ring start/end sector
// prepare SD flash card using disk editor
// and ctools/genimage as described in docs
#define SD_RING_IMG_FIRST_HW_SECTOR 7600

//#include "../emCpp/const.h"
#define Kb 1024
#define Mb (1024L*Kb)
#define Gb (1024L*Mb)
// //

#define SD_RING_IMG_SIZE (( 512*Mb )-(SD_RING_IMG_FIRST_HW_SECTOR*Kb))

#define PIN_BT_READY 7

#define NMEA_MAX_MESSAGE_SZ 88

//extern void halt(void);
extern void reset(void);

#define EOL 0x0A

//#define DEBUG_UART3

#ifdef DEBUG_UART3
  #define DEBUG_UART Serial3
#else
  #define DEBUG_UART Serial
#endif

#define DEBUG_UART_BAUD 115200

#endif // _H_CONFIG
