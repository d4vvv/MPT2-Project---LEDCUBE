#include "MKL05Z4.h"                    	/* Device header */
#include "tpm.h"													/* pwm */
#include "sct.h"                          /* SCT2167 header */
#include "demo.h"

#define RED 0
#define GREEN 1
#define BLUE 2

void Clear_Frame(uint8_t tbl[8][8]);

extern uint8_t Frame_Br[3];								
extern uint8_t Frame[3][8][8];
extern volatile uint8_t Next_Frame;	
extern volatile uint8_t Frame_Time;

// x1,y1,z1 - lower left corner
// x2,y2,z2 - upper right corner
// x - number of the diode in row
// y - number of the row of certain layer
// z - number of the layer
void Rectangle(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8], uint8_t x1, uint8_t y1, uint8_t z1, uint8_t x2, uint8_t y2, uint8_t z2)
{
 Frame_Time = Time;
 Frame_Br[Color] = Br;	

 for(uint8_t i = x1; i <= x2; i++)
  {
   uint8_t	aa = (uint8_t)(1 << i);
   tbl[z1][y1] |= aa;
   tbl[z1][y2] |= aa;
   tbl[z2][y1] |= aa;
   tbl[z2][y2] |= aa;
  }
 
 uint8_t aa = (uint8_t)(1 << x1) | (uint8_t)(1 << x2);

 for(uint8_t i = y1; i <= y2; i++)
  {
   tbl[z1][i] |= aa;
   tbl[z2][i] |= aa;
  }
 for(uint8_t i = z1; i <= z2; i++)
  {
   tbl[i][y1] |= aa;
   tbl[i][y2] |= aa;
  }
 while (Next_Frame == 0){}
 Next_Frame = 0;
}	

void Demo_Contour(uint8_t Color, uint8_t Time, uint8_t Br, uint8_t tbl[8][8], uint8_t rotations)
{	
	Frame_Time = Time;
	Frame_Br[Color] = Br;
	
	Clear_Frame(Frame[0]);
	Clear_Frame(Frame[1]);
	Clear_Frame(Frame[2]);
	
	for (uint8_t x1 = 0; x1 < 8; x1++)
	  {
     tbl[0][0] |= (uint8_t)(1 << x1);
	   tbl[0][7] |= (uint8_t)(1 << (7-x1));
     tbl[0][x1] |= 0x80;
     tbl[0][7-x1] |= 0x01;

     tbl[7][0] |= (uint8_t)(1 << x1);
	   tbl[7][7] |= (uint8_t)(1 << (7-x1));
     tbl[7][x1] |= 0x80;
     tbl[7][7-x1] |= 0x01;

		 tbl[x1][0] |= 0x81;	
		 tbl[x1][7] |= 0x81;	
			
		 while (Next_Frame == 0){}
		 Next_Frame = 0;
		}	
	
	for (uint8_t rot = 0; rot < rotations; rot++)
	{	
	for (uint8_t x1 = 0; x1 < 7; x1++)
   { 
		for (uint8_t x2 = 1; x2 < 7; x2++)
	  {
     tbl[x2][0] = ((uint8_t)tbl[x2][0]) * 2;		
		 tbl[x2][7] = ((uint8_t)tbl[x2][7]) / 2;

 		 tbl[x2][x1] &= 0x7F;	
 		 tbl[x2][x1+1] |= 0x80;	

 	   tbl[x2][7-x1] &= 0xFE;	
		 tbl[x2][6-x1] |= 0x01;	
		}	
		while (Next_Frame == 0){}
		Next_Frame = 0;
	 }
 }	

	for (uint8_t rot = 0; rot < rotations; rot++)
	{	
	for (uint8_t x1 = 0; x1 < 7; x1++)
   { 
		for (uint8_t x2 = 1; x2 < 7; x2++)
	  {
     tbl[x2][0] = ((uint8_t)tbl[x2][0]) / 2;		
		 tbl[x2][7] = ((uint8_t)tbl[x2][7]) * 2;

 		 tbl[x2][x1] &= 0xFE;	
 		 tbl[x2][x1+1] |= 0x01;	

 	   tbl[x2][7-x1] &= 0x7F;	
		 tbl[x2][6-x1] |= 0x80;	
		}	
		while (Next_Frame == 0){}
		Next_Frame = 0;
	 }
 }	

 	for (uint8_t rot = 0; rot < rotations; rot++)
	{	
	for (uint8_t x1 = 1; x1 < 8; x1++)
   { 
    tbl[x1][0] = 0xFF; 
    tbl[x1-1][0] = 0x81; 

    tbl[7-x1][7] = 0xFF; 
    tbl[8-x1][7] = 0x81; 
		 
		tbl[0][7-x1] = 0xFF;
		tbl[0][8-x1] = 0x81;
		 
		tbl[7][x1] = 0xFF;
		tbl[7][x1-1] = 0x81;

		while (Next_Frame == 0){}
		Next_Frame = 0;
	 }
 }	

 for (uint8_t rot = 0; rot < rotations; rot++)
	{	
	for (uint8_t x1 = 1; x1 < 8; x1++)
   { 
    tbl[7-x1][0] = 0xFF; 
    tbl[8-x1][0] = 0x81; 

    tbl[x1][7] = 0xFF; 
    tbl[x1-1][7] = 0x81; 
		 
		tbl[0][x1] = 0xFF;
		tbl[0][x1-1] = 0x81;
		 
		tbl[7][7-x1] = 0xFF;
		tbl[7][8-x1] = 0x81;

		while (Next_Frame == 0){}
		Next_Frame = 0;
	 }
 }
 Clear_Frame(tbl);
}

void Clear_Frame(uint8_t tbl[8][8])
{
	for (uint8_t x = 0; x < 8; x++)
	 for (uint8_t y = 0; y < 8; y++)
			tbl[x][y] = 0;
}

void Demo_Color(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8])
{
	Frame_Time = Time;
	Frame_Br[Color] = Br;
	
	Clear_Frame(tbl);
	
	for (uint8_t x1 = 0; x1 < 8; x1++)
	 for (uint8_t x2 = 0; x2 < 8; x2++)
	  {
	   Clear_Frame(tbl);
		 for (uint8_t x3 = 0; x3 < 8; x3++)
		   { 
			  tbl[x1][x2] = (uint8_t)(1 << x3);
		    while (Next_Frame == 0){}
    	  Next_Frame = 0;
			 }	
		 }	
  Clear_Frame(tbl);
}

void Demo1_Color(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8])
{
	Frame_Time = Time;
	Frame_Br[Color] = Br;
	
	Clear_Frame(tbl);
	
	for (uint8_t x1 = 0; x1 < 8; x1++)
	 for (uint8_t x2 = 0; x2 < 8; x2++)
	  {
	   Clear_Frame(tbl);
     tbl[x1][x2] = 0xff;
	   while (Next_Frame == 0){}
		 Next_Frame = 0;
		 }	
  Clear_Frame(tbl);
}

void Demo2_Color(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8])
{
	Frame_Time = Time;
	Frame_Br[Color] = Br;
	
	Clear_Frame(tbl);
	
	for (uint8_t x1 = 0; x1 < 8; x1++)
   for (uint8_t x2 = 0; x2 < 8; x2++)
	  {
     tbl[x1][x2] |= 0xff;
	  }	
	while (Next_Frame == 0){}
	Next_Frame = 0;
  Clear_Frame(tbl);
}

void Demo3_Color(uint8_t Time)
{
	Frame_Time = Time;
	while (Next_Frame == 0){}
	Next_Frame = 0;
}

void Demo_XXXX(uint8_t Time, uint8_t Color, uint8_t Br)
{
 Frame_Time = Time;
 Frame_Br[Color] = Br;
	
 for(uint8_t i = 0; i < 8; i++)
  {
   Frame[Color][i][i] = (uint8_t)(1 << i);
	 while (Next_Frame == 0){}
   Next_Frame = 0;
	}		
}

