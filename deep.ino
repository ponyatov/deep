
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
struct T_SD_CMD {
  uint8_t cmd;  // command # with msb bit padding 01 = 0x40
  uint8_t arg3;   // argument bytes
  uint8_t arg2;   // argument bytes
  uint8_t arg1;   // argument bytes
  uint8_t arg0;   // argument bytes
  uint8_t crc7;    // crc with lsb end bit set
} __attribute__ ((__packed__));

T_SD_CMD SD_CMD0[]={0x40|0,1,2,3,4,1|0b1010010};
#define SD_CMD_GO_IDLE_STATE SD_CMD0

void SD_init(void) {
  // init procedure @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  SPI_preinit();
  // wait on power on
  delay(1);
  // dummy clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (int i=0;i<77;i++) { digitalWrite(SCK,LOW); digitalWrite(SCK,HIGH); }
  // SPI hw enable
  SPI_postinit(); SD_on();
  // CMD0
  SPI.transfer(SD_CMD0,sizeof(SD_CMD0));
  for (int j=0;j<9;j++) SPI.transfer(0xFF);
}

void SD_on(void) { digitalWrite(SS, LOW); /* SS# */ }

void setup(void) {
  Serial.begin(115200);
  Serial.println(sizeof(SD_CMD0));
  SD_init();
}

void loop(void) {
}

