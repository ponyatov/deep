#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

void setup(void) {
  Serial.begin(115200);
  memset(SDx.buf.b,0xFF,SD_LOW::sectorsz);
  if (SDx.begin()) {
    //if (SDx.read(0x00000000L)) SDx.dump(); else Serial.println("read error");
    SDx.write(-1L);
    SDx.dump();
  } // SD.begin ok
}

void loop(void) {
}

