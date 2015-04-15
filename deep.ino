
#include <SPI.h>

void SPI_preinit(void) {
  /* mega 1280/2560 @ variants/mega/pins_arduino.h
  static const uint8_t SS   = 53;
  static const uint8_t SCK  = 52;
  static const uint8_t MOSI = 51;
  static const uint8_t MISO = 50;
  */
  pinMode(SS,  OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(MISO, INPUT);
}

void SPI_postinit(void) {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
  // required for SD/MMC
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
}

// SD commands set

uint8_t SD_CMD0[] = {0xFF,0x40|0,0x00,0x00,0x00,0x00,0x4A<<1|1};
#define SD_CMD_GO_IDLE_STATE SD_CMD0

uint8_t SD_CMD8[] = {0xFF,0x40|8,0x00,0x00,0x01,0xAA,0x43<<1|1};
#define SD_CMD_SEND_IF_COND SD_CMD8

uint8_t SD_CMD16_512K[] = {0xFF,0x40|16,0x00,0x00,0x02,0x00,0x0A<<1|1};
#define SD_CMD_SET_BLOCKLEN_512K SD_CMD16_512K

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
    uint32_t vRange:9;
  } __attribute__ ((packed)) b;
};

bool SD_init() {
  // init using grabbed track from LisFiles.ino
  SPI_preinit();
  // init procedure corresponds to @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  // wait 1+ ms on power on
  delay(1);
  // dummy 74+ clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<77;i++) { digitalWrite(MOSI,LOW); digitalWrite(MOSI,HIGH); }
  // SPI hw enable
  SPI_postinit(); SD_on();
  Serial.println("SD init:");
  // cmd0
  if (SD_CMD_R1(SD_CMD0,sizeof(SD_CMD0)) != 0x01) { Serial.println("CMD0 error"); return false; } else {
    Serial.println("CMD0 ok");
    // cmd8
    uint32_t R7=0;
    if (SD_CMD_R7(SD_CMD8,sizeof(SD_CMD8),&R7) != 0x01) { Serial.println("CMD8/R1 error"); return false; } else {
      if (R7!=0x1AA) { Serial.println("CMD8/R7(0x1AA) error: SD ver.2 only supported"); return false; } else {
        Serial.println("CMD8 ok");
        // acmd41 /prefixed with cmd55/
          Serial.print("ACMD41/0x40000000 seq ");
          while (SD_ACMD(SD_ACMD41_40,sizeof(SD_ACMD41_40)) != 0x00) Serial.print(".");
          Serial.println(" Ok");
          // cmd58 check power state
          SD_OCR OCR;
          if (SD_CMD_R3(SD_CMD58,sizeof(SD_CMD58),&OCR.i)!=0x00)  { Serial.println("CMD58/R1 error"); return false; } else {
            // analyze SD info
            if (OCR.b.ccs) { Serial.println("CMD58/CCS error: SDv2 with block address not supported"); return false; } else {
              // cmd16 set block size
              if (SD_CMD_R1(SD_CMD16_512K,sizeof(SD_CMD16_512K))!=0x00) { Serial.println("CMD16/512K block error"); return false; } else {
                // f**g final
                SD_off();
                return true;
              } // cmd16/blocksz
            } // cmd58/ccs=1 block address
          } // cmd58/r1
      } // cmd8/0x1AA SD v.2 only
    } // cmd8/r1
  } // cmd0
}

void setup(void) {
  Serial.begin(115200);
  SD_init();
}

void loop(void) {
}

