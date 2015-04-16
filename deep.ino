
#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

// SD commands set

uint8_t SD_CMD9 [] = {0xFF,0x40|9 ,0x00,0x00,0x00,0x00,0x57<<1|1};
#define SD_CMD_SEND_CSD SD_CMD9
uint8_t SD_CMD10[] = {0xFF,0x40|10,0x00,0x00,0x00,0x00,0x0D<<1|1};
#define SD_CMD_SEND_CID SD_CMD10

uint8_t SD_CMD16_512K[] = {0xFF,0x40|16,0x00,0x00,0x02,0x00,0x0A<<1|1};
#define SD_CMD_SET_BLOCKLEN_512K SD_CMD16_512K

uint8_t SD_CMD17[] = {0xFF,0x40|17,0x00,0x00,0x00,0x00,0xFF};
#define SD_CMD_READ_BLOCK SD_CMD17

uint8_t SD_CMD24[] = {0xFF,0x40|24,0x00,0x00,0x00,0x00,0xFF};
#define SD_CMD_WRITE_BLOCK SD_CMD24

uint8_t SD_CMD58[] = {0xFF,0x40|58,0x00,0x00,0x00,0x00,0x7E<<1|1};
#define SD_CMD_READ_OCR SD_CMD58

uint8_t SD_CMD55[] = {0xFF,0x40|55,0x00,0x00,0x00,0x00,0x32<<1|1};
#define SD_ACMD_PFX SD_CMD55
uint8_t SD_ACMD41_00[] = {0xFF,0x40|41,0x00,0x00,0x00,0x00,0x72<<1|1};
uint8_t SD_ACMD41_40[] = {0xFF,0x40|41,0x40,0x00,0x00,0x00,0x3B<<1|1};

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

uint8_t SD_ACMD(uint8_t *cmd, uint8_t sz) {
  if (SD_CMD_R1(SD_ACMD_PFX,sizeof(SD_ACMD_PFX)) != 0x01) { Serial.println("ACMD_PFX error"); return 0xFF; } else {
    return SD_CMD_R1(cmd,sz);
  }
}

uint8_t SD_CMD_R7(uint8_t *cmd, uint8_t sz, uint32_t *r7) {
  uint8_t r1 = SD_CMD_R1(cmd,sz);
  for (uint8_t i=0;i<sizeof(uint32_t);i++)
    *r7 = ( *r7 << 8 ) | SPI.transfer(0xFF);
  return r1;
}

inline uint8_t SD_CMD_R3(uint8_t *cmd, uint8_t sz, uint32_t *r3) {
  SD_CMD_R7(cmd,sz,r3);
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

union SD_CID {
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

bool SD_init() {
  Serial.print("SD init: ");
        // acmd41 /prefixed with cmd55/
          Serial.print("ACMD41/0x40000000 seq ");
          while (SD_ACMD(SD_ACMD41_40,sizeof(SD_ACMD41_40)) != 0x00) Serial.print(".");
          // cmd58 check power state
          Serial.print("CMD58 ");
          SD_OCR OCR;
          if (SD_CMD_R3(SD_CMD58,sizeof(SD_CMD58),&OCR.i)!=0x00)  { Serial.println("CMD58/R1 error"); return false; } else {
            // analyze SD info
            if (OCR.b.ccs) { Serial.println("CMD58/CCS error: SDv2 with block address not supported"); return false; } else {
              // cmd16 set block size
              Serial.print("CMD16/512K ");
              if (SD_CMD_R1(SD_CMD16_512K,sizeof(SD_CMD16_512K))!=0x00) { Serial.println("CMD16/512K block error"); return false; } else {
                /*
                // get extra card info
                uint8_t cidcsd[0x20];
                Serial.print("\nCID: ");
                if (SD_CMD_R1(SD_CMD_SEND_CID,sizeof(SD_CMD_SEND_CID))==0x00) {
                  for (uint8_t cid=0;cid<sizeof(cidcsd);cid++) cidcsd[cid]=SPI.transfer(0xFF);
                  for (uint8_t cid=0;cid<sizeof(cidcsd);cid++) Serial.print(cidcsd[cid],HEX);
                }
                */
                SD_off();
                Serial.println("Ok");
                memcpy(SD_R_CMD.b,SD_CMD17,sizeof(SD_CMD17));  // \ fill memcmd buffers
                memcpy(SD_W_CMD.b,SD_CMD24,sizeof(SD_CMD24));  // /
                return true;
              } // cmd16/blocksz
            } // cmd58/ccs=1 block address
          } // cmd58/r1
}

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

uint8_t SD_IO_BUF[SD_SECTOR_SIZE];

void setup(void) {
  Serial.begin(115200);
  SDx.begin();
  SD_init();
  Serial.print("Read #00: "); Serial.println(SD_R(0x00000000,SD_IO_BUF));
  Serial.print("CRC: "); Serial.println(SD_R_RESULT.CRC,HEX);
  Serial.print("[00]: "); Serial.println(SD_IO_BUF[0x00],HEX);
  Serial.print("[FF]: "); Serial.println(SD_IO_BUF[0xFF],HEX);
  Serial.print("[512]: "); Serial.println(SD_IO_BUF[512],HEX);
}

void loop(void) {
}

