
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

// SD cmd set from @ http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
struct S_SD_CMD {
  uint8_t cmd;  // command # with msb bit padding 01 = 0x40
  uint8_t arg3;   // argument bytes
  uint8_t arg2;   // argument bytes
  uint8_t arg1;   // argument bytes
  uint8_t arg0;   // argument bytes
  uint8_t crc7;    // crc with lsb end bit set
} __attribute__ ((__packed__));

// SD R1 command answer
struct S_SD_R1 {
  uint8_t pad:1, // 0 pad bit
  ParameterError:1,
  AddressError:1,
  EraseSeqError:1,
  CommandCRCError:1,
  IllegalCommand:1,
  EraseReset:1,
  InIdleState:1;
}  __attribute__ ((__packed__));

#define SD_R1_IDLE 0x01

union U_SD_R1 { S_SD_R1 r; uint8_t b; };
//union U_SD_R7 { S_SD_R7 r; uint8_t b[4]; };

S_SD_CMD SD_CMD0[]={0x40|0,0,0,0,0,0x4A<<1|1};
#define SD_CMD_GO_IDLE_STATE SD_CMD0

S_SD_CMD SD_CMD8[]={0x40|8,0,0,0x01,0xAA,0x43<<1|1};
#define SD_CMD_SEND_IF_COND SD_CMD8

S_SD_CMD SD_ACMD41_40[]={0x40|41,0x40,0x00,0x00,0x00,0x35<<1|1};
#define SD_CMD_APP_SEND_OP_COND SD_ACMD41_40

S_SD_CMD SD_ACMD41_00[]={0x40|41,0x00,0x00,0x00,0x00,0x72<<1|1};
#define SD_CMD_APP_SEND_OP_COND SD_ACMD41_00


void SD_init(void) {
  U_SD_R1 R1;
//  U_SD_R7 R7;
  uint8_t ncr;
  // init procedure @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  Serial.println("SD init:");
  SPI_preinit();
  // wait on power on
  delay(1);
  // dummy clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<77;i++) { digitalWrite(SCK,LOW); digitalWrite(SCK,HIGH); }
  // SPI hw enable
  SPI_postinit(); SD_on();
  // CMD0
  SPI.transfer(SD_CMD0,sizeof(SD_CMD0));
  for (ncr=0;ncr<=8;ncr++) { R1.b=SPI.transfer(0xFF); if (R1.b != 0xFF) break; }
  SPI.transfer(0xFF); SPI.transfer(0xFF); 
  if (R1.b!=SD_R1_IDLE) { Serial.println("CMD0 error"); } else {
    Serial.println("CMD0 ok");
    // CMD8
    SPI.transfer(SD_CMD8,sizeof(SD_CMD8));
    for (ncr=0;ncr<=8;ncr++) { R1.b=SPI.transfer(0xFF); if (R1.b != 0xFF) break; }
    uint32_t r7=0; for(uint8_t i=0;i<sizeof(r7);i++) r7=(r7<<8)|SPI.transfer(0xFF);
    SPI.transfer(0xFF); SPI.transfer(0xFF); 
    Serial.print("CMD8(0x1AA) R1:"); Serial.print(R1.b);
    if (R1.b!=SD_R1_IDLE) { Serial.println("CMD8/R1 error"); } else {
      Serial.print(" R7:");  Serial.print(r7,HEX); Serial.print(" ");
      if (r7!=0x1AA) { Serial.println("CMD8/0x1AA error: only SDv2 supported"); } else {
        Serial.println("Ok, SDv2 detected");
        // ACMD41_40
        Serial.print("ACMD41(0x40000000):");
        SPI.transfer(SD_ACMD41_40,sizeof(SD_ACMD41_40));
        for (ncr=0;ncr<=8;ncr++) { R1.b=SPI.transfer(0xFF); if (R1.b != 0xFF) break; }
        SPI.transfer(0xFF); SPI.transfer(0xFF); 
        // ACMD41_00
        Serial.print("ACMD41(0x00000000):");
        SPI.transfer(SD_ACMD41_00,sizeof(SD_ACMD41_00));
        for (ncr=0;ncr<=8;ncr++) { R1.b=SPI.transfer(0xFF); if (R1.b != 0xFF) break; }
        SPI.transfer(0xFF); SPI.transfer(0xFF); 
      }
    } // CMD8 R1 part ok
  } // CMD0 ok
}

void SD_on(void) { digitalWrite(SS, LOW); /* SS# */ }

void setup(void) {
  Serial.begin(115200);
  SD_init();
}

void loop(void) {
}

