#include "MKL05Z4.h"
#include "sct.h"

void Send8BytesT(uint8_t *TableB, uint8_t *TableG, uint8_t *TableR )
{
	for (uint8_t xx = 0; xx < 8; xx++)
	{
	 CLR_CLK;																								//The bits are written to the register on the rising edge of the clocck, so we have to clear it at the beginning
	 (TableR[xx] & 0x80) ? (SET_SDI1) : (CLR_SDI1);					
	 (TableG[xx] & 0x80) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x80) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x40) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x40) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x40) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x20) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x20) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x20) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x10) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x10) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x10) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x08) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x08) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x08) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;
	
	 CLR_CLK;
	 (TableR[xx] & 0x04) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x04) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x04) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x02) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x02) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x02) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;

	 CLR_CLK;
	 (TableR[xx] & 0x01) ? (SET_SDI1) : (CLR_SDI1);
	 (TableG[xx] & 0x01) ? (SET_SDI2) : (CLR_SDI2);
	 (TableB[xx] & 0x01) ? (SET_SDI3) : (CLR_SDI3);
	 SET_CLK;
	}

	SET_LE;
	CLR_CLK;
	CLR_LE;
}
