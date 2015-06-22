// low-level SD/SPI library
// (c) Dmitry Ponyatov <dponyatov@gmail.com>
// part of https://github.com/ponyatov/deep project

#include "SD_low.h"

SD_LOW::SD_LOW(char _wpadchar) {
	ring.wpadchar = _wpadchar;
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
	CMD buf;
	memcpy(buf.b, cmd, cmdsz); 				// copy cmd to tmp buffer
	uint32_t opr = bswap32(op);          	// reverse operand bytes
	if (buf.f.op != opr)
		buf.f.op = opr;  					// update operand
	SPI.transfer(buf.b, cmdsz); 			// send command package over spi
	uint8_t Ncr;
	SD_LOW::R1 R1;
	for (Ncr = 0, R1.b = 0xFF;
		 Ncr < 8 & R1.b == 0xFF;
		 Ncr++, R1.b = SPI.transfer(0xFF) );
	return R1;
}

SD_LOW::R3& SD_LOW::cmdR3(const uint8_t *cmd, uint32_t op) {
	SD_LOW::R3 R3;
	R3.r1 = cmdR1(cmd, op);
	for (uint8_t i = 0; i < sizeof(uint32_t); i++)
		R3.r3 = (R3.r3 << 8) | SPI.transfer(0xFF);
	return R3;
}

SD_LOW::R7& SD_LOW::cmdR7(const uint8_t *cmd, uint32_t op) {
	SD_LOW::R7 R7;
	R7.r1 = cmdR1(cmd, op);
	for (uint8_t i = 0; i < sizeof(uint32_t); i++)
		R7.r7 = (R7.r7 << 8) | SPI.transfer(0xFF);
	return R7;
}

SD_LOW::R1& SD_LOW::acmd(const uint8_t *cmd, uint32_t op) {
	cmdR1(cmd55, op);
	return cmdR1(cmd);
}

bool SD_LOW::begin(void) {
  DEBUG_UART.print("d: SD/lowlevel init: ");
  // init using grabbed track from ListFiles.ino
  // init procedure corresponds to @ http://elm-chan.org/docs/mmc/gx1/sdinit.png
  spi_preinit();
  // wait 1+ ms on power on
  delay(1);
  // dummy 74+ clock CS=DI=high
  digitalWrite(SPI_CHIP_SELECT,HIGH); digitalWrite(MOSI,HIGH);
  for (uint8_t i=0;i<80;i++) { digitalWrite(SCK,LOW); digitalWrite(SCK,HIGH); }
  // SPI hw enable
  spi_postinit(); on();
//  // cmd12
//  cmdR1b(cmd12);
  // cmd0
  DEBUG_UART.print("CMD0 ");
  if ( cmdR1(cmd0).b != R1_IDLE ) { return error(); } else {
    // cmd8/1aa
    DEBUG_UART.print("CMD8/1AA "); // error: SD ver.2 only supported
    SD_LOW::R7 R7 =cmdR7(cmd8,0x1AA);
    if ( R7.r1.b != R1_IDLE ) { return error(); } else {
      if ( R7.r7 != 0x1AA ) { return error(); } else {
        // acmd41/hcs
        DEBUG_UART.print("ACMD41/HCS "); // HCS bit 30 up
        int cmd41_limit=0x666;
        while ( (--cmd41_limit>0) & (acmd(cmd41,0x40000000).b != R1_READY) ) delay(1);
        if (!cmd41_limit) {
            DEBUG_UART.print("error: CMD41 timeout ");
            return error();
        }
        // cmd58
        DEBUG_UART.print("CMD58/CCS ");
        SD_LOW::R3 R3=cmdR3(cmd58);
        SD_LOW::OCR OCR; OCR.i=R3.r3;
        if (R3.r1.b != R1_READY) { return error(); } else {
          if (OCR.b.ccs) { DEBUG_UART.print("SDHC/SDXC block address "); return error(); } else {
            // cmd16/512
            DEBUG_UART.print("CMD16/512 ");
            if (cmdR1(cmd16,512).b != R1_READY) { return error(); } else {
              // done
              off();
              DEBUG_UART.println("Ok");
              return true;
            } // cmd16/512
          } // cm58/ccs
        } // cmd58/r1
      } // cmd8/1aa
    } // cmd8/r1
  } // cmd0
}

bool SD_LOW::error(void) { off(); DEBUG_UART.println("error"); return false; }

void SD_LOW::spi_preinit(void) {
  pinMode(SPI_CHIP_SELECT,  OUTPUT); // chipselect /inverted/
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

void SD_LOW::on (void)  { digitalWrite(SPI_CHIP_SELECT, LOW); /* SS# */ noInterrupts(); }
void SD_LOW::off(void)  { digitalWrite(SPI_CHIP_SELECT,HIGH); /* SS# */ interrupts(); }

bool SD_LOW::write(uint32_t sector, char *buf) {
	bool ok = false;
	on();
	if (cmdR1(cmd24, sector * sectorsz).b == R1_READY) {	// block_write cmd
		SPI.transfer(0xFF);                                 // >=1 byte pad
		SPI.transfer(TOKEN_RW_SINGLE);  					// token
		for (uint16_t i = 0; i < sectorsz; i++)				// data
			SPI.transfer(buf[i]);
		SPI.transfer(0xFF); SPI.transfer(0xFF); 			// dummy crc
		ok = ((SPI.transfer(0xFF) & RESP_MASQ) == RESP_ACCEPTED); // check resp
		while (SPI.transfer(0xFF) == SD_BUSY); 				// wait busy state
	} else spi_0x100pad();
	off();
	return ok;
}

void SD_LOW::spi_0x100pad() {
	for (uint16_t i = 0; i < 0x100; i++)
		SPI.transfer(0xFF); // clock pad
}

bool SD_LOW::read(uint32_t sector, char *buf) {
  bool ok=false;
  on();
  if (cmdR1(cmd17, sector * sectorsz).b == R1_READY) {	// read sector cmd17
		uint8_t token = TOKEN_X;
		while (token == TOKEN_X) token = SPI.transfer(0xFF); // wait data token
		// read data stream
		if (token == TOKEN_RW_SINGLE) {
			for (uint16_t i = 0; i < sectorsz; i++) {			// data
				buf[i] = SPI.transfer(0xFF);
//        delay(1);
			}
			SPI.transfer(0xFF); SPI.transfer(0xFF);			// skip crc
			ok = true;
		} else spi_0x100pad();
  } else spi_0x100pad();
  off();
  return ok;
}

void SD_LOW::ring_flush(void) {
	// endpadding to end of sector
	uint16_t padding = sectorsz - ring.wptr;
	memset(&ring.wbuf[ring.wptr], ring.wpadchar, padding);
	// write data
	if (write(ring.w, ring.wbuf)) {
		ring_incptr(ring.w);
	} else {
		DEBUG_UART.println();
		DEBUG_UART.print("d: sector ");
		DEBUG_UART.print(ring.w);
		DEBUG_UART.println(" sector SD write error");
		reset();
	};
	// reset wptr
	ring.wptr = 0;
}

void SD_LOW::ring_append(char *buf, uint16_t sz) {
	for (uint16_t i = 0; i < sz; i++) {		// append data
		ring.wbuf[ring.wptr++] = buf[i]; 	// append next char
		// flush buffer @ end of buffer
		if (ring.wptr >= sizeof(ring.wbuf))
			ring_flush();
	}
}

void SD_LOW::ring_incptr(uint32_t &sector) {
	sector++;
	if (sector > ring.end) sector = ring.start;
}

bool SD_LOW::ring_hasData(void) {
	return (ring.r != ring.w) | ring.rptr < sizeof(ring.rbuf);
}

void SD_LOW::ring_nextrsector(void) {
	if (!read(ring.r, ring.rbuf)) {
		DEBUG_UART.println();
		DEBUG_UART.print("d: sector ");
		DEBUG_UART.print(ring.r);
		DEBUG_UART.println(" sector SD read error");
		reset();
	} else
		ring_incptr(ring.r);
}

char *SD_LOW::ring_read(void) {
	if (!ring_hasData()) {
		DEBUG_UART.println();
		DEBUG_UART.println("SD ring empty");
		reset();
	}
	ring_nextrsector();
	return ring.rbuf;
}

char SD_LOW::ring_nextchar(void) {
	char R;
	if (ring.rptr < sizeof(ring.rbuf)) {	// has buffered data in rbuf
		R = ring.rbuf[ring.rptr++];
	} else {								// rbuf empty
		if (ring.r != ring.w) {				// has data on SD
			ring_nextrsector();				// load sector
			ring.rptr=0;					// reset rptr to data begin
			R = ring_nextchar();			// recursive ring_nextchar
		} else
			R = ring.wpadchar;
	}
	return R;
}

#include "EEPROM.h"

EEMEM uint32_t SD_LOW::er = SD_RING_IMG_FIRST_HW_SECTOR;
EEMEM uint32_t SD_LOW::ew = SD_RING_IMG_FIRST_HW_SECTOR;

void SD_LOW::ring_rwptr_load(void) {
	DEBUG_UART.print("d: SD ring r/w load");
	ring.r = ee_getuint32((EEPTR) &er);
	ring.w = ee_getuint32((EEPTR) &ew);
	DEBUG_UART.print(" r= "); DEBUG_UART.print(ring.r);
	DEBUG_UART.print(" w= "); DEBUG_UART.println(ring.w);
	DEBUG_UART.flush();
}

void SD_LOW::ring_rwptr_save(void) {
	DEBUG_UART.print("d: SD ring r/w save");
	DEBUG_UART.print(" r= "); DEBUG_UART.print(ring.r);
	DEBUG_UART.print(" w= "); DEBUG_UART.println(ring.w);
	DEBUG_UART.flush();
	//	if (er != ring.r) er = ring.r;
	if (ee_getuint32((EEPTR) &er) != ring.r)
		ee_setuint32((EEPTR) &er, ring.r);
	//	if (ew != ring.w) ew = ring.w;
	if (ee_getuint32((EEPTR) &ew) != ring.w)
		ee_setuint32((EEPTR) &ew, ring.w);
}

void SD_LOW::ring_coldstart(void) {
	ring.r = ring.start; ring.w = ring.start;
	ring.rptr=sizeof(ring.rbuf); ring.wptr=0;
	ring_rwptr_save();
}

void SD_LOW::ring_reset(void) {
	ring_rwptr_load();
}
