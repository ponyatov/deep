#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial
uint8_t CRCTable[256];

uint8_t crc7_table[] = {
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

void GenerateCRCTable()
{
    int i, j;
 
    // generate a table value for all 256 possible byte values
    for (i = 0; i < 256; i++)
    {
        CRCTable[i] = (i & 0x80) ? i ^ CRCPoly : i;
        for (j = 1; j < 8; j++)
        {
            CRCTable[i] <<= 1;
            if (CRCTable[i] & 0x80)
                CRCTable[i] ^= CRCPoly;
        }
    }
}

uint8_t crc_add(uint8_t *table, uint8_t crc0, uint8_t nextbyte) {
  return table[(crc0<<1)^nextbyte];
}

uint8_t crc_byte(uint8_t *table, uint8_t *buf, size_t len) {
	uint8_t CRC=0;
	for (int i=0; i<len;i++) {
		CRC=crc_add(table,CRC,buf[i]);
	}
	return CRC;
}

uint8_t crc7(uint8_t *buf, size_t len) {
	return crc_byte(crc7_table,buf,len);
}


int main() {
	/*
	GenerateCRCTable();
	for (int i=0;i<sizeof(CRCTable);i++) {
		if (i%8==0) printf("\n");
		printf("0x%.2X,",CRCTable[i]);
	}
	printf("\n");
	*/
	// SD SPI commands
	// cmdX
	uint8_t cmdX[]={0x40|16,0x00,0x00,0x02,0x00};
	uint8_t cmdX_crc7=crc7(cmdX,sizeof(cmdX));
	printf("\ncmdX crc7:%.2X crc7<<1|1:%.2X\n\n",cmdX_crc7,cmdX_crc7<<1|1);
	return 0;
}

