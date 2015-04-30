// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#include "SD_low.h"

SD_LOW::SD_LOW() {
	ring.wpadchar=0;
}

// \ commands set: static...
const uint8_t SD_LOW::cmd0[]  ={0xFF,0x40|0 ,0x00,0x00,0x00,0x00,0x4A<<1|1};
const uint8_t SD_LOW::cmd8[]  ={0xFF,0x40|8 ,0x00,0x00,0x01,0xAA,0x43<<1|1};
const uint8_t SD_LOW::cmd12[] ={0xFF,0x40|12,0x00,0x00,0x00,0x00,0x30<<1|1};
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
  uint32_t opr = bswap32(op);          // reverse operand bytes
  if (buf.f.op != opr) buf.f.op=opr;  // update operand
  SPI.transfer(buf.b,cmdsz); // send command package over spi
  uint8_t Ncr;
  SD_LOW::R1 R1;
  for (  
    Ncr=0 , R1.b =0xFF;
    Ncr<8 & R1.b==0xFF;
    Ncr++ , R1.b =SPI.transfer(0xFF)
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
        while ( acmd(cmd41,0x40000000).b != R1_READY );
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

bool SD_LOW::error(void) { off(); Serial.println("error"); return false; }

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

bool SD_LOW::write(uint32_t sector, char *buf) {
	bool ok = false;
	on();
	if (cmdR1(cmd24, sector * sectorsz).b == R1_READY) {	// block_write cmd
		SPI.transfer(0xFF);                                      // >=1 byte pad
		SPI.transfer(TOKEN_RW_SINGLE);  						// token
		for (uint16_t i = 0; i < sectorsz; i++) SPI.transfer(buf[i]);  // data
		SPI.transfer(0xFF); SPI.transfer(0xFF); 				// dummy crc
		ok = (SPI.transfer(0xFF) & RESP_MASQ) == RESP_ACCEPTED; // check resp
		while (SPI.transfer(0xFF) == SD_BUSY); 				// wait busy state
	} else { // R1 error, pad 0x100 byte clock
		for (uint16_t i = 0; i < 0x100; i++) SPI.transfer(0xFF);
	}
	off();
	return ok;
}

bool SD_LOW::write(uint32_t sector) {
	buf.sector = sector;
	buf.ok = write(sector, buf.b);
	return buf.ok;
}

bool SD_LOW::read(uint32_t sector, char *buf) {
  bool ok=false;
  on();
  if (cmdR1(cmd17,sector*sectorsz).b == R1_READY) {		// read sector cmd17
	uint8_t token;										// wait data token
	for (token = TOKEN_X; token == TOKEN_X; token = SPI.transfer(0xFF));
    // read data stream
    if (token == TOKEN_RW_SINGLE) {
      for (uint16_t i=0;i<sectorsz;i++) buf[i]=SPI.transfer(0xFF);	// data
      SPI.transfer(0xFF); SPI.transfer(0xFF);			// skip crc
	  ok = true;
    }
  } else {
	for (uint16_t i = 0; i < 0x100; i++) SPI.transfer(0xFF);
  }
  off();
  return ok;
}

bool SD_LOW::read(uint32_t sector) {
	buf.sector = sector;
	buf.ok = read(sector, buf.b);
	return buf.ok;
}

void SD_LOW::dump(void) {
  Serial.print("\nsector:"); Serial.print(buf.sector);
  Serial.print(" ok:"); Serial.print(buf.ok); 
  for (int i=0;i<sectorsz;i++) {
    if (i%0x10==0) {
      Serial.print("\n");
      Serial.print((i>>12)&0xF,HEX); 
      Serial.print((i>> 8)&0xF,HEX); 
      Serial.print((i>> 4)&0xF,HEX); 
      Serial.print((i    )&0xF,HEX); 
      Serial.print(": "); 
    }
      Serial.print(" ");
      Serial.print((buf.b[i]>> 4)&0xF,HEX); 
      Serial.print((buf.b[i]    )&0xF,HEX); 
  }
  Serial.print("\n");
}

const uint8_t SD_LOW::crc7_table[] = {
0x00,0x09,0x12,0x1B,0x24,0x2D,0x36,0x3F,0x48,0x41,0x5A,0x53,0x6C,0x65,0x7E,0x77,
0x19,0x10,0x0B,0x02,0x3D,0x34,0x2F,0x26,0x51,0x58,0x43,0x4A,0x75,0x7C,0x67,0x6E,
0x32,0x3B,0x20,0x29,0x16,0x1F,0x04,0x0D,0x7A,0x73,0x68,0x61,0x5E,0x57,0x4C,0x45,
0x2B,0x22,0x39,0x30,0x0F,0x06,0x1D,0x14,0x63,0x6A,0x71,0x78,0x47,0x4E,0x55,0x5C,
0x64,0x6D,0x76,0x7F,0x40,0x49,0x52,0x5B,0x2C,0x25,0x3E,0x37,0x08,0x01,0x1A,0x13,
0x7D,0x74,0x6F,0x66,0x59,0x50,0x4B,0x42,0x35,0x3C,0x27,0x2E,0x11,0x18,0x03,0x0A,
0x56,0x5F,0x44,0x4D,0x72,0x7B,0x60,0x69,0x1E,0x17,0x0C,0x05,0x3A,0x33,0x28,0x21,
0x4F,0x46,0x5D,0x54,0x6B,0x62,0x79,0x70,0x07,0x0E,0x15,0x1C,0x23,0x2A,0x31,0x38,
0x41,0x48,0x53,0x5A,0x65,0x6C,0x77,0x7E,0x09,0x00,0x1B,0x12,0x2D,0x24,0x3F,0x36,
0x58,0x51,0x4A,0x43,0x7C,0x75,0x6E,0x67,0x10,0x19,0x02,0x0B,0x34,0x3D,0x26,0x2F,
0x73,0x7A,0x61,0x68,0x57,0x5E,0x45,0x4C,0x3B,0x32,0x29,0x20,0x1F,0x16,0x0D,0x04,
0x6A,0x63,0x78,0x71,0x4E,0x47,0x5C,0x55,0x22,0x2B,0x30,0x39,0x06,0x0F,0x14,0x1D,
0x25,0x2C,0x37,0x3E,0x01,0x08,0x13,0x1A,0x6D,0x64,0x7F,0x76,0x49,0x40,0x5B,0x52,
0x3C,0x35,0x2E,0x27,0x18,0x11,0x0A,0x03,0x74,0x7D,0x66,0x6F,0x50,0x59,0x42,0x4B,
0x17,0x1E,0x05,0x0C,0x33,0x3A,0x21,0x28,0x5F,0x56,0x4D,0x44,0x7B,0x72,0x69,0x60,
0x0E,0x07,0x1C,0x15,0x2A,0x23,0x38,0x31,0x46,0x4F,0x54,0x5D,0x62,0x6B,0x70,0x79
};

void SD_LOW::ring_flush(void) {
	// endpadding to end of sector
	int padding = sectorsz-ring.wptr;
	memset(&ring.wbuf[ring.wptr],ring.wpadchar,padding);
	// write data
	write(ring.w, ring.wbuf);
	ring_raiseptr(ring.w);
	// reset wptr
	ring.wptr = 0;
}

void SD_LOW::ring_append(char *buf, int sz) {
	for (uint8_t i = 0; i < sz; i++) {		// append data
		ring.wbuf[ring.wptr++] = buf[i]; 	// append next char
		// flush buffer @ end of buffer
		if (ring.wptr >= sizeof(ring.wbuf))
			ring_flush();
	}
}

void SD_LOW::ring_raiseptr(uint32_t &sector) {
	sector++;
	if (sector>ring.end) sector=ring.start;
}

bool SD_LOW::ring_hasData(void) {
	return ring.r != ring.w;
}

char *SD_LOW::ring_poll(void) {
	if (ring_hasData()) {
		read(ring.r, ring.rbuf);
		ring_raiseptr(ring.r);
	}
	return ring.rbuf;
}

//EEMEM
uint32_t SD_LOW::er = SD_RING_IMG_FIRST_HW_SECTOR;
//EEMEM
uint32_t SD_LOW::ew = SD_RING_IMG_FIRST_HW_SECTOR;

void SD_LOW::ring_rwptr_load(void) {
	ring.r = er;
	ring.w = ew;
}

void SD_LOW::ring_rwptr_save(void) {
	if (er != ring.r) er = ring.r;
	if (ew != ring.w) ew = ring.w;
}

void SD_LOW::ring_coldstart(void) {
	ring.r = ring.start;
	ring.w = ring.start;
	ring_rwptr_save();
}

void SD_LOW::ring_reset(void) {
	ring_rwptr_load();
}
