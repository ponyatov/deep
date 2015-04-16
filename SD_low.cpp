// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#include "SD_low.h"

// \ commands set: static...
const uint8_t SD_LOW::cmd0[] ={0xFF,0x40|0,0x00,0x00,0x00,0x00,0x4A<<1|1,0xFF};
const uint8_t SD_LOW::cmd8[] ={0xFF,0x40|8,0x00,0x00,0x01,0xAA,0x43<<1|1,0xFF};
// /

uint8_t SD_LOW::cmdR1(const uint8_t *cmd) {
  uint8_t buf[cmdsz]; memcpy(buf,cmd,cmdsz); // copy cmd to tmp buffer
  SPI.transfer(buf,cmdsz); // send command package over spi
  uint8_t Ncr,R1;
  for (  
    Ncr=0, R1= 0xFF;
    Ncr<8, R1==0xFF;
    Ncr++, R1=SPI.transfer(0xFF)
    );
  return R1;
}

uint8_t SD_LOW::cmdR7(const uint8_t *cmd, uint32_t *R7) {
  uint8_t  R1 =cmdR1(cmd);
  for (uint8_t i=0;i<sizeof(R7);i++);
  return R1;
}

bool SD_LOW::begin(void) {
  Serial.print("SD/lowlevel init: ");
  // init using grabbed track from ListFiles.ino
  // init procedure corresponds to @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  spi_preinit();
  // wait 1+ ms on power on
  delay(1);
  // dummy 74+ clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<80;i++) { digitalWrite(MOSI,LOW); digitalWrite(MOSI,HIGH); }
  // SPI hw enable
  spi_postinit(); on();
  // cmd0
  Serial.print("CMD0 ");
  if ( cmdR1(cmd0) != R1_IDLE ) { return error(); } else {
    // cmd8/1aa
    Serial.print("CMD8/1AA "); // error: SD ver.2 only supported
    uint32_t R7=0;
    if ( cmdR7(cmd8,&R7) != R1_IDLE ) { return error(); } else {
      if (R7!=0x1AA) { return error(); } else {
        // done
        //off();
        Serial.println("Ok");
        return true;
      } // cmd8/1aa
    } // cmd8/r1
  } // cmd0
}

bool SD_LOW::error(void) { Serial.println("error"); return false; }

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

