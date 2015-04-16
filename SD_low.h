// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#ifndef _H_SDLOW_
#define _H_SDLOW_

#include <Arduino.h>
#include <SPI.h>

class SD_LOW {
    static const uint16_t sectorsz =512; // sector size, bytes
  public:
    struct SECTOR {                      // i/o buffer for data
    uint32_t addr;
    uint8_t b[sectorsz];
    } buf;
    
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

  union CID {
    uint8_t b[128/8];
    struct {
      uint32_t MID:8;      // Manufacturer ID
      uint32_t OID:16;     // OEM/Application ID
      char     PNM[40/8];  // Product name
      uint32_t PRV:8;      // Product revision
      uint32_t PSN:32;     // Product serial number
      uint32_t :4;     
      uint32_t MDT:12;     // Manufacturing date
      uint32_t CRC:7;
      uint32_t :1;
    } __attribute__ ((packed)) f; 
  };
  
    bool begin(void);
    void end(void);
    void on(void);                      // enable card chipselect
    void off(void);                     // disable card
    SECTOR& read(uint32_t sector);     // sector -> buffer
    void write(uint32_t sector);        // buffer -> sector
    void spi_preinit(void);
    void spi_postinit(void);
    R1& cmdR1(const uint8_t *cmd, uint32_t op=0); // send command R1 resp
    R3& cmdR3(const uint8_t *cmd, uint32_t op=0); // send command R3 resp
    R7& cmdR7(const uint8_t *cmd, uint32_t op=0); // send command R7 resp
    R1& acmd(const uint8_t *cmd, uint32_t op=0);  // send cmd55 cmdxx R1 resp
    bool error(void);                   // log error
};

#endif // _H_SDLOW_

