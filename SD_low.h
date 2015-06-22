// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

  /* mega 1280/2560 @ variants/mega/pins_arduino.h
  static const uint8_t SS   = 53;
  static const uint8_t SCK  = 52;
  static const uint8_t MOSI = 51;
  static const uint8_t MISO = 50;
  */


#ifndef _H_SDLOW_
#define _H_SDLOW_

#include "config.h"
#include <Arduino.h>
#include <SPI.h>

#define SPI_CHIP_SELECT SS
//#define SPI_CHIP_SELECT 53	/* MEGA */	
//#define SPI_CHIP_SELECT  4

#include "config.h"

class SD_LOW {

	void spi_0x100pad(void);
  public:

	SD_LOW(char wpadchar=EOL);

    static const uint16_t sectorsz =512; // sector size, bytes
//    struct SECTOR {                      // main i/o buffer for data
//    uint32_t sector;
//    char b[sectorsz];
//    bool ok;
//    } buf;
//
    static const uint8_t SD_BUSY=0;
    // \ data response codes
    static const uint8_t RESP_MASQ     =0b00011111;
    static const uint8_t RESP_ACCEPTED =0b00000101;
    static const uint8_t RESP_BADCRC   =0b00001011;
    static const uint8_t RESP_WERR     =0b00001101;
    // /

    // \ r/w tokens (data block begin markers)
    static const uint8_t TOKEN_X 		 =0b11111111;
    static const uint8_t TOKEN_RW_SINGLE =0b11111110;
//    static const uint8_t TOKEN_MULTI =0b11111100;
    // /

    // \ commands set
    static const uint8_t cmdsz =7;       // SD command size, bytes

    union CMD {
      uint8_t b[cmdsz];
      struct {
        uint32_t pad:10; // 0xFF 0b00......
        uint32_t index:6; // command num
        uint32_t op;    // operand
        uint32_t crc7:7;  // crc
        uint32_t one:1;  // terminator bit =1
      } __attribute__ ((packed)) f;;
    };

    static const uint8_t
    // index      // argument  // response  // data  // Abbreviation   // Description
      cmd0[],     // None(0)   // R1        // no    // GO_IDLE_STATE  // Software reset
      cmd8[],     // None(0)   // R1        // no    // SEND_OP_COND   // Initiate initialization process
      cmd9[],
      cmd10[],
      cmd12[],    // None(0)    // R1b      // no    // STOP_TRANSMISSION
      cmd16[],    // Block length[31:0] // R1 // no  // SET_BLOCKLEN   // Change R/W block size
      cmd17[],
      cmd24[],
      cmd41[],
      cmd55[],
      cmd58[];
    // /

    // \ R1 return status
    static const uint8_t R1_READY =0x00;
    static const uint8_t R1_IDLE  =0x01;
    // /

    union R1 {
      uint8_t b;
    };
    struct R3 { R1 r1; int32_t r3; };
    struct R7 { R1 r1; int32_t r7; };

  union OCR {
    uint32_t i;
    struct {
      uint32_t busy:1;
      uint32_t ccs:1;
      uint32_t :6;
      uint32_t VHS:9; // voltage field
    } __attribute__ ((packed)) b;
  };

//  union CID {
//    uint8_t b[128/8];
//    struct {
//      uint32_t MID:8;      // Manufacturer ID
//      uint32_t OID:16;     // OEM/Application ID
//      char     PNM[40/8];  // Product name
//      uint32_t PRV:8;      // Product revision
//      uint32_t PSN:32;     // Product serial number
//      uint32_t :4;
//      uint32_t MDT:12;     // Manufacturing date
//      uint32_t CRC:7;
//      uint32_t :1;
//    } __attribute__ ((packed)) f;
//  };

    bool begin(void);
//    void end(void);
    void on(void);                      		// enable card chipselect
    void off(void);                     		// disable card
	bool  read(uint32_t sector, char *); 		// read  random buffer[sectorsz]
	bool write(uint32_t sector, char *); 		// write random buffer[sectorsz]
    void spi_preinit(void);
    void spi_postinit(void);
    R1& cmdR1(const uint8_t *cmd, uint32_t op=0); // send command R1 resp
    R3& cmdR3(const uint8_t *cmd, uint32_t op=0); // send command R3 resp
    R7& cmdR7(const uint8_t *cmd, uint32_t op=0); // send command R7 resp
    R1&  acmd(const uint8_t *cmd, uint32_t op=0); // send cmd55 cmdxx R1 resp
    bool error(void);                   // log error
//    void dump(void);                    // dump current sector

    // data buffering using circular ring

    static EEMEM uint32_t er,ew; // SD ring pointers in EEPROM

    struct {
		uint32_t start = SD_RING_IMG_FIRST_HW_SECTOR;
		uint32_t end = SD_RING_IMG_FIRST_HW_SECTOR + SD_RING_IMG_SIZE/sectorsz;
		uint32_t r, w; // working ring pointers
      char rbuf[sectorsz]; // reading buffer separated from main SD_LOW::buf
      uint16_t rptr;
      char wbuf[sectorsz]; // writing buffer separated from main SD_LOW::buf
      uint16_t wptr;
      char wpadchar=EOL;	// padding symbol to end of unfilled sector
   	  	  	  	  	  	 	// 0x0A = EOL for text ring, 0x00 for bin ring
    } ring;

    void ring_coldstart(void);		// full ring buffer reset
    void ring_reset(void);			// clear ring buffer
    void ring_rwptr_load(void);		// load r/wfrom EEPROM
    void ring_rwptr_save(void);		// save r/w to EEPROM
    void ring_append(char *,uint16_t);	// append data to ring buffer
    void ring_flush(void);			// flush ring to SD
    void ring_incptr(uint32_t&);	// increment pointer ringically
    bool ring_hasData(void);		// return flag data ready in SD buffer
    char *ring_read(void);			// read next sector from SD ring
    char ring_nextchar(void);		// get one char from SD ring
    void ring_nextrsector(void);
};

#endif // _H_SDLOW_
