
#include <SPI.h>

/*
// Mega1280/2560
static const uint8_t SS   = 53;
static const uint8_t MOSI = 51;
static const uint8_t MISO = 50;
static const uint8_t SCK  = 52;
*/

void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128);
/*
  pinMode(SS, OUTPUT);
  digitalWrite(SS,HIGH);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO,  INPUT);
  pinMode(SCK, OUTPUT);
*/
}

void SD_start() {
  digitalWrite(SS, LOW);
}

void SD_stop() {
  digitalWrite(SS,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
//  shiftOut(MOSI,SCK,LSBFIRST,0xA5);
//  shiftOut(MOSI,SCK,LSBFIRST,0x00);
//  shiftOut(MOSI,SCK,LSBFIRST,0xFF);
  SD_start();
  SPI.transfer(0x00);
  SD_stop();
  SPI.transfer(0xFF);
}

