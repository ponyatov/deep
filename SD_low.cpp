// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#include "SD_low.h"

// \ commands set: static...
const uint8_t SD_LOW::cmd0[]={0xFF,0x40|0,0x00,0x00,0x00,0x00,0x4A<<1|1};
// /

void SD_LOW::begin(void) {
  Serial.print("SD/lowlevel init: ");
  // init using grabbed track from ListFiles.ino
  // init procedure corresponds to @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  spi_preinit();
  // wait 1+ ms on power on
  delay(1);
  // dummy 74+ clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<77;i++) { digitalWrite(MOSI,LOW); digitalWrite(MOSI,HIGH); }
  // SPI hw enable
  spi_postinit(); on();
  // done
  Serial.println("");
}

void SD_LOW::spi_preinit(void) {
  /* mega 1280/2560 @ variants/mega/pins_arduino.h
  static const uint8_t SS   = 53;
  static const uint8_t SCK  = 52;
  static const uint8_t MOSI = 51;
  static const uint8_t MISO = 50;
  */
  pinMode(SS,  OUTPUT); // chipselect /inverted/
  pinMode(SCK, OUTPUT); // clock
  pinMode(MOSI,OUTPUT); // output line
  pinMode(MISO, INPUT); // input line
}

void SD_LOW::spi_postinit(void) {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  // required for SD/MMC
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
}

void SD_LOW::on (void)  { digitalWrite(SS, LOW); /* SS# */ }
void SD_LOW::off(void)  { digitalWrite(SS,HIGH); /* SS# */ }

