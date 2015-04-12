
#include <SPI.h>

/*
// Mega1280/2560
static const uint8_t SS   = 53;
static const uint8_t MOSI = 51;
static const uint8_t MISO = 50;
static const uint8_t SCK  = 52;
*/

void SPI_preinit() {
  pinMode(SS, OUTPUT); // SS#
  pinMode(MOSI, OUTPUT);
  pinMode(MISO,  INPUT);
  pinMode(SCK, OUTPUT);
//  // trigger pulse
//  digitalWrite(SS,LOW);
}

void SPI_postinit() {  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); // required for SD/MMC
  SPI.setClockDivider(SPI_CLOCK_DIV128);
}

#define SD_CMD0 0x00
#define SD_GO_IDLE_STATE SD_CMD0
// arg:no resp:R1 data: no

struct T_SD_R1 {
  uint8_t pad0:1; // always 0
  uint8_t ParameterErr:1;
  uint8_t AddressErr:1;
  uint8_t EraseSeqErr:1;
  uint8_t CommandCRCErr:1;
  uint8_t IllegalCommand:1;
  uint8_t EraseReset:1;
  uint8_t InIdleState:1;
} __attribute__ ((__packed__));

union { uint8_t byte; T_SD_R1 bit; } SD_R1;

struct SD_R2 {
  
};

struct SD_R3 {
  T_SD_R1 R1;
} __attribute__ ((__packed__)) SD_R3;

struct SD_R7 {
  
};

void SD_init() {
  SPI_preinit();
  // do SPI mode setup @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  Serial.print("\nSD init:");
  // wail >1ms on power on
  delay(2);
  // dummy clock CS=DI=high
  digitalWrite(SS,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<0xFF;i++) { digitalWrite(SCK,LOW); digitalWrite(SCK,HIGH); }
  // SPI enable
  SPI_postinit(); SD_start();
  // cmd0
  while (SPI.transfer(0x00)==0xFF) {
      delay(1);
  char c0[]={0,0,0,0};
  SPI.transfer(c0,sizeof(c0));
  }
  /*
  char c0[]={0,0,0,0};
  SPI.transfer(c0,sizeof(c0));
  SPI.transfer(c0,sizeof(c0));
  SPI.transfer(c0,sizeof(c0));
  SPI.transfer(c0,sizeof(c0));
  */
  /*
  SD_R1.byte=SPI.transfer(SD_CMD0);
  uint8_t cmd0t1=SD_R1.byte;
  SD_R1.byte=SPI.transfer(SD_CMD0);
  uint8_t cmd0t2=SD_R1.byte;
  SD_R1.byte=SPI.transfer(SD_CMD0);
  uint8_t cmd0t3=SD_R1.byte;
  SD_R1.byte=SPI.transfer(SD_CMD0);
  uint8_t cmd0t4=SD_R1.byte;
  Serial.print(" CMD0:");
  Serial.print(cmd0t1);
  Serial.print(cmd0t2);
  Serial.print(cmd0t3);
  Serial.print(cmd0t4);
  */
  SD_error();
  /*
  if (cmd0t1==0xFF & cmd0t2==0xFF) { 
    Serial.print(" \bError: no card");
    SD_error();
  }
  */
  Serial.println("\n");
}

void SD_start() {
  digitalWrite(SS, LOW);
}

void SD_stop() {
  digitalWrite(SS, LOW);
}

void SD_error() {
  SD_stop();
  SPI.end();
  SD_stop();
}

void setup() {
  Serial.begin(115200);
  SD_init();
}

void loop() {
  for (;;) delay(111);
/*
  SD_start();
  SPI.transfer(0x00);
  SD_stop();
  SPI.transfer(0xFF);
*/
}

