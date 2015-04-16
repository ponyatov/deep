// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#ifndef _H_SDLOW_
#define _H_SDLOW_

#include <Arduino.h>
#include <SPI.h>

class SD_LOW {
    static const uint16_t sectorsz=512;  // sector size, bytes
    uint8_t buffer[sectorsz];            // i/o buffer for data
    
    // \ commands set
    static const uint8_t
    // index    // argument  // response  // data  // Abbreviation   // Description
      cmd0[],   // None(0)   // R1        // no    // GO_IDLE_STATE  // Software reset
      cmd8[],
      cmd9[],
      cmd10[],
      cmd16[],
      cmd17,
      cmd24[],
      cmd41[],
      cmd55[],
      cmd58[];
    // /
  public:
    void begin(void);
    void end(void);
    void on(void);                    // enable card chipselect
    void off(void);                   // disable card
    void read(uint32_t sector);       // sector -> buffer
    void write(uint32_t sector);      // buffer -> sector
    void spi_preinit(void);
    void spi_postinit(void);
};

#endif // _H_SDLOW_

