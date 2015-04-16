#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

// SD commands set

uint8_t SD_CMD9 [] = {0xFF,0x40|9 ,0x00,0x00,0x00,0x00,0x57<<1|1};
#define SD_CMD_SEND_CSD SD_CMD9
uint8_t SD_CMD10[] = {0xFF,0x40|10,0x00,0x00,0x00,0x00,0x0D<<1|1};
#define SD_CMD_SEND_CID SD_CMD10

uint8_t SD_CMD17[] = {0xFF,0x40|17,0x00,0x00,0x00,0x00,0xFF};
#define SD_CMD_READ_BLOCK SD_CMD17

uint8_t SD_CMD24[] = {0xFF,0x40|24,0x00,0x00,0x00,0x00,0xFF};
#define SD_CMD_WRITE_BLOCK SD_CMD24

void SD_on(void)  { digitalWrite(SS, LOW); /* SS# */ }
void SD_off(void) { digitalWrite(SS,HIGH); /* SS# */ }

uint8_t SD_CMD_R1(uint8_t *cmd, uint8_t sz) {
  uint8_t ncr;
  uint8_t R1;
  uint8_t buf[0x10]; for (uint8_t i=0;i<sz;i++) buf[i]=cmd[i]; // copy cmd
  SPI.transfer(buf,sz);
  for ( ncr=0,R1=0xFF ; ncr<8&R1==0xFF; ncr++,R1=SPI.transfer(0xFF) );
  return R1;
}

uint8_t SD_CMD_R3(uint8_t *cmd, uint8_t sz, uint32_t *r7) {
  uint8_t r1 = SD_CMD_R1(cmd,sz);
  for (uint8_t i=0;i<sizeof(uint32_t);i++)
    *r7 = ( *r7 << 8 ) | SPI.transfer(0xFF);
  return r1;
}

union SD_OCR {
  uint32_t i;
  struct {
    uint32_t busy:1;
    uint32_t ccs:1;
    uint32_t :6;
    uint32_t VHS:9; // voltage field
  } __attribute__ ((packed)) b;
};

union {
  uint8_t b[sizeof(SD_CMD17)];
  struct {
    uint8_t pad;
    uint8_t cmd;
    uint32_t addr;
    uint8_t crc;
  } __attribute__ ((packed)) f;
} SD_R_CMD,SD_W_CMD;

#define SD_R_TOKEN 0b11111110
#define SD_W_TOKEN 0b11111100

#define SD_SECTOR_SIZE 512

struct {
  uint8_t  R1;
  uint8_t  token;
  uint16_t CRC;
} SD_R_RESULT;

bool SD_R(uint32_t sector,uint8_t *buf) {
  bool Ok=false;
  SD_on();
  SD_R_CMD.f.addr=sector;
  SD_R_RESULT.R1=SD_CMD_R1(SD_R_CMD.b,sizeof(SD_R_CMD.b)); // cmd17 r/1sector
  if (SD_R_RESULT.R1==0x00) {
    for (SD_R_RESULT.token=0xFF;SD_R_RESULT.token==0xFF;SD_R_RESULT.token=SPI.transfer(0xFF)); // wait token
    if (SD_R_RESULT.token==SD_R_TOKEN) {
      for (uint16_t i=0;i<SD_SECTOR_SIZE;i++) buf[i]=SPI.transfer(0xFF); // read data
      SD_R_RESULT.CRC = SPI.transfer(0xFF)<<8|SPI.transfer(0xFF); // read crc
      Ok=true;
    }
  }
  SD_off();
  return Ok;
}

void setup(void) {
  Serial.begin(115200);
  if (SDx.begin()) {
    Serial.println("sector 0x00:");
    SD_LOW::SECTOR sec = SDx.read(0x00);
  }
}

void loop(void) {
}

