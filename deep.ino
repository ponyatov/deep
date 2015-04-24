#include <SPI.h>
#include "SD_low.h"
SD_LOW SDx;

// 1 megabyte
#define Meg1 1024L*1024

void setup(void) {
  // set SD ring start/end sector 
  // prepare SD flash card using disk editor and ctools/genimage as described in docs
  SDx.ring.start = 7600;
  SDx.ring.end   = SDx.ring.start + Meg1/SD_LOW::sectorsz; // + 1 megabyte for ring buffer

  Serial.begin(115200);
  memset(SDx.buf.b,0xFF,SD_LOW::sectorsz);
  if (SDx.begin()) {
    //if (SDx.read(0x00000000L)) SDx.dump(); else Serial.println("read error");
    Serial.println("test write first ring.img sector");
    Serial.println( SDx.write(SDx.ring.start) ); // write first 512 bytes of F:\ring.img
    SDx.dump();
    Serial.println("done");
  } // SD.begin ok
}

void loop(void) {
}

