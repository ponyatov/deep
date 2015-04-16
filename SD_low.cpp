// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#include "SD_low.h"

// \ commands set: static...
const uint8_t SD_LOW::cmd0[]  ={0xFF,0x40|0 ,0x00,0x00,0x00,0x00,0x4A<<1|1};
const uint8_t SD_LOW::cmd8[]  ={0xFF,0x40|8 ,0x00,0x00,0x01,0xAA,0x43<<1|1};
const uint8_t SD_LOW::cmd16[] ={0xFF,0x40|16,0x00,0x00,0x02,0x00,0x0A<<1|1};
const uint8_t SD_LOW::cmd17[] ={0xFF,0x40|17,0x00,0x00,0x00,0x00,0x00<<1|1};
const uint8_t SD_LOW::cmd24[] ={0xFF,0x40|24,0x00,0x00,0x00,0x00,0x00<<1|1};
const uint8_t SD_LOW::cmd55[] ={0xFF,0x40|55,0x00,0x00,0x00,0x00,0x32<<1|1};
const uint8_t SD_LOW::cmd41[] ={0xFF,0x40|41,0x40,0x00,0x00,0x00,0x72<<1|1};
const uint8_t SD_LOW::cmd58[] ={0xFF,0x40|58,0x00,0x00,0x00,0x00,0x7E<<1|1};
// /

inline uint32_t bswap32(uint32_t op) {
  return 
  ((op&0x000000FF)<<24)|
  ((op&0x0000FF00)<< 8)|
  ((op&0x00FF0000)>> 8)|
  ((op&0xFF000000)>>24);
}

SD_LOW::R1& SD_LOW::cmdR1(const uint8_t *cmd, uint32_t op) {
  CMD buf; memcpy(buf.b,cmd,cmdsz); // copy cmd to tmp buffer
  uint32_t opr = bswap32(op);
  if (buf.f.op != opr) buf.f.op=opr;
  SPI.transfer(buf.b,cmdsz); // send command package over spi
  uint8_t Ncr;
  SD_LOW::R1 R1;
  for (  
    Ncr=0, R1.b =0xFF;
    Ncr<8, R1.b==0xFF;
    Ncr++, R1.b =SPI.transfer(0xFF)
    );
  return R1;
}

SD_LOW::R3& SD_LOW::cmdR3(const uint8_t *cmd, uint32_t op) {
  SD_LOW::R3 R3; R3.r1 =cmdR1(cmd,op);
  for (uint8_t i=0;i<sizeof(uint32_t);i++)
    R3.r3 = (R3.r3 << 8) | SPI.transfer(0xFF);
  return R3;
}

SD_LOW::R7& SD_LOW::cmdR7(const uint8_t *cmd, uint32_t op) {
  SD_LOW::R7 R7; R7.r1 =cmdR1(cmd,op);
  for (uint8_t i=0;i<sizeof(uint32_t);i++)
    R7.r7 = (R7.r7 << 8) | SPI.transfer(0xFF);
  return R7;
}

SD_LOW::R1& SD_LOW::acmd(const uint8_t *cmd, uint32_t op) {
  cmdR1(cmd55,op);
  return cmdR1(cmd);
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
  for (uint8_t i=0;i<80;i++) { digitalWrite(SCK,LOW); digitalWrite(SCK,HIGH); }
  // SPI hw enable
  spi_postinit(); on();
//  // cmd12
//  cmdR1b(cmd12);
  // cmd0
  Serial.print("CMD0 ");
  if ( cmdR1(cmd0).b != R1_IDLE ) { return error(); } else {
    // cmd8/1aa
    Serial.print("CMD8/1AA "); // error: SD ver.2 only supported
    SD_LOW::R7 R7 =cmdR7(cmd8,0x1AA);
    if ( R7.r1.b != R1_IDLE ) { return error(); } else {
      if ( R7.r7 != 0x1AA ) { return error(); } else {
        // acmd41/0x40
        Serial.print("ACMD41/0x40 ");
        while ( acmd(cmd41,0x40000000).b != R1_READY ) Serial.print(". ");
        // cmd58
        Serial.print("CMD58 ");
        SD_LOW::R3 R3=cmdR3(cmd58);
        SD_LOW::OCR OCR; OCR.i=R3.r3;
        if (R3.r1.b != R1_READY) { return error(); } else {
          if (OCR.b.ccs) { Serial.print("SDv2 block address "); return error(); } else {
            // cmd16/512
            Serial.print("CMD16/512 ");
            if (cmdR1(cmd16,512).b != R1_READY) { return error(); } else {
              // done
              off();
              Serial.println("Ok");
              return true;
            } // cmd16/512
          } // cm58/ccs
        } // cmd58/r1
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

SD_LOW::SECTOR& SD_LOW::read(uint32_t sector) {
  buf.addr=sector;
  on();
  buf.r1=cmdR1(cmd17,sector).b;
  if (buf.r1 == R1_READY) {
    // wait data token
    for (
      buf.token =TOKEN_X;
      buf.token==TOKEN_X;
      buf.token=SPI.transfer(0xFF)
    );
    // read data stream
    if (buf.token == TOKEN_R) {
      for (uint16_t i=0;i<sectorsz;i++) buf.b[i]=SPI.transfer(0xFF);
      buf.crc = (SPI.transfer(0xFF)<<8)|SPI.transfer(0xFF);
    } else buf.crc=0xFFFF;
  }
  off();
  return buf;
}

