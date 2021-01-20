#include "MKL05Z4.h"                    	/* Device header */
#include "tpm.h"													/* pwm */
#include "sct.h"                          /* SCT2167 header */
#include <stdlib.h>
#include "tetris.h"                    

#define RED 0
#define GREEN 1
#define BLUE 2

#define BUTTON_1 0x01
#define BUTTON_2 0x02
#define BUTTON_3 0x04
#define BUTTON_4 0x08
#define BUTTON_5 0x10
#define BUTTON_6 0x20
#define BUTTON_7 0x40
#define BUTTON_8 0x80

#define FORWARD 2
#define BACKWARD 3
#define LEFT 1
#define RIGHT 0
#define UP 4
#define DOWN 5
#define NONE 7

extern uint8_t Frame_Br[3];								
extern uint8_t Frame[3][8][8];
extern volatile uint8_t Next_Frame;	
extern volatile uint8_t Frame_Time;
extern uint16_t Block[10];
extern uint8_t Direction;
extern volatile uint8_t Keyboard_UART;
extern volatile uint8_t Next_Box;
extern uint8_t Block_Size_x[10];
extern uint8_t Block_Size_y[10];
extern volatile uint8_t Keyboard;

// Block definition:
// 
//  x7  x8  x9
//  x4  x5  x6
//  x1  x2  x3
// Block[x] = 0000 000x9 x8x7x6x5 x4x3x2x1
//
uint16_t Block[10] = {0x0001, 0x001B, 0x01FF, 0x0155, 0x0003, 0x000B, 0x001A, 0x0007, 0x003F, 0x004F};									//declaration of blocks
uint8_t Block_Size_x[10] = {1, 2, 3, 3, 2, 2, 2, 3, 3, 3};																															//and their sizes
uint8_t Block_Size_y[10] = {1, 2, 3, 3, 1, 2, 2, 1, 2, 3};
static uint8_t Block_nr = 4;

void ClearLayer(uint8_t lay, uint8_t Color, uint8_t Time)																																//function for clearing one layer
{
	if (Time > 0)
	 Frame_Time = Time;
	
	for(uint8_t i = 0; i < 8; i++)
		Frame[Color][lay][i] = 0x00;
}

void SetLayer(uint8_t lay, uint8_t Color, uint8_t Time)																																	//function for lighting up one layer
{
	Frame_Time = Time;
	for(uint8_t i = 0; i < 8; i++)
		Frame[Color][lay][i] = 0xFF;
}

void Draw_Block(uint8_t x_pos, uint8_t y_pos, uint8_t lay, uint8_t block_nr, uint8_t Color, uint8_t Time)								//function for drawing blocks
{
	if (Time > 0)
	 Frame_Time = Time;

  if (Block[block_nr] & 0x0001)																																													//we are checking what's the shape of the block wa want to draw
	  Frame[Color][lay][y_pos] |= (uint8_t)(1 << x_pos);																																	
  
	if (Block_Size_x[Block_nr] > 1)																																												//if block size is bigger than 1, we are further checking its shape
   if (Block[block_nr] & 0x0002)
	  Frame[Color][lay][y_pos] |= (uint8_t)(1 << (x_pos + 1));
  
	if (Block_Size_x[Block_nr] > 2)
	 if (Block[block_nr] & 0x0004)
	  Frame[Color][lay][y_pos] |= (uint8_t)(1 << (x_pos + 2));

  if (Block_Size_y[Block_nr] > 1)
	{
	 if (Block[block_nr] & 0x0008)
	  Frame[Color][lay][y_pos+1] |= (uint8_t)(1 << x_pos);

	 if (Block_Size_x[Block_nr] > 1)
    if (Block[block_nr] & 0x0010)
	   Frame[Color][lay][y_pos+1] |= (uint8_t)(1 << (x_pos + 1));
   
	 if (Block_Size_x[Block_nr] > 2)
	  if (Block[block_nr] & 0x0020)
	   Frame[Color][lay][y_pos+1] |= (uint8_t)(1 << (x_pos + 2));
  }
	
  if (Block_Size_y[Block_nr] > 2)
	{
   if (Block[block_nr] & 0x0040)
	  Frame[Color][lay][y_pos+2] |= (uint8_t)(1 << x_pos);
   
	 if (Block_Size_x[Block_nr] > 1)
	  if (Block[block_nr] & 0x0080)
	   Frame[Color][lay][y_pos+2] |= (uint8_t)(1 << (x_pos + 1));
  
	 if (Block_Size_x[Block_nr] > 2) 
	  if (Block[block_nr] & 0x0100)
	   Frame[Color][lay][y_pos+2] |= (uint8_t)(1 << (x_pos + 2));
  }
}

void Move_Block(uint8_t x_pos_from, uint8_t y_pos_from, uint8_t lay_from, uint8_t x_pos_to, uint8_t y_pos_to, uint8_t lay_to, uint8_t Color, uint8_t Time)
{
	uint16_t Block_Tmp = 0;																																													//function for movement of the block
	
  if ((Frame[Color][lay_from][y_pos_from]) & (uint8_t)(0x01 << x_pos_from))																				//checking what figure we want to move
		Block_Tmp = ((uint16_t)(0x01 << 0));

  if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color][lay_from][y_pos_from]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp |= ((uint16_t)(0x01 << 1));

  if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color][lay_from][y_pos_from]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp |= ((uint16_t)(0x01 << 2));

  if (Block_Size_y[Block_nr] > 1)
	{ 
	if ((Frame[Color][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << x_pos_from))
		Block_Tmp |= ((uint16_t)(0x01 << 3));

  if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp |= ((uint16_t)(0x01 << 4));

  if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp |= ((uint16_t)(0x01 << 5));
  }
	
	if (Block_Size_y[Block_nr] > 2)
	{
  if ((Frame[Color][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << x_pos_from))
		Block_Tmp |= ((uint16_t)(0x01 << 6));
  
	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp |= ((uint16_t)(0x01 << 7));
  
	if (Block_Size_x[Block_nr] > 2)
	if ((Frame[Color][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp |= ((uint16_t)(0x01 << 8));
 }
	
	ClearLayer(lay_from, Color, Time);																																					//clearing the shape
	
  if (Block_Tmp & 0x0001)																																											//moving the shape to the new position
	  Frame[Color][lay_to][y_pos_to] |= (uint8_t)(1 << x_pos_to);

	if (Block_Size_x[Block_nr] > 1)
  if (Block_Tmp & 0x0002)
	  Frame[Color][lay_to][y_pos_to] |= (uint8_t)(1 << (x_pos_to + 1));

	if (Block_Size_x[Block_nr] > 2)
  if (Block_Tmp & 0x0004)
	  Frame[Color][lay_to][y_pos_to] |= (uint8_t)(1 << (x_pos_to + 2));

  if (Block_Size_y[Block_nr] > 1)
	{
	if (Block_Tmp & 0x0008)
	  Frame[Color][lay_to][y_pos_to+1] |= (uint8_t)(1 << x_pos_to);

	if (Block_Size_x[Block_nr] > 1)
  if (Block_Tmp & 0x0010)
	  Frame[Color][lay_to][y_pos_to+1] |= (uint8_t)(1 << (x_pos_to + 1));

	if (Block_Size_x[Block_nr] > 2)
  if (Block_Tmp & 0x0020)
	  Frame[Color][lay_to][y_pos_to+1] |= (uint8_t)(1 << (x_pos_to + 2));
  }
	
	if (Block_Size_y[Block_nr] > 2)
	{
  if (Block_Tmp & 0x0040)
	  Frame[Color][lay_to][y_pos_to+2] |= (uint8_t)(1 << x_pos_to);

	if (Block_Size_x[Block_nr] > 1)
  if (Block_Tmp & 0x0080)
	  Frame[Color][lay_to][y_pos_to+2] |= (uint8_t)(1 << (x_pos_to + 1));

	if (Block_Size_x[Block_nr] > 2)
  if (Block_Tmp & 0x0100)
	  Frame[Color][lay_to][y_pos_to+2] |= (uint8_t)(1 << (x_pos_to + 2));
  }
}	

// == 0 -> next position is free
// == 1 -> next position is busy
uint8_t Test_New_Position(uint8_t x_pos_from, uint8_t y_pos_from, uint8_t lay_from, uint8_t x_pos_to, uint8_t y_pos_to, uint8_t lay_to, uint8_t Color_Move, uint8_t Color_XX)
{
	uint16_t Block_Tmp_from = 0;																																//function determining if the move is possible
	uint16_t Block_Tmp_to = 0;
	
	if (lay_from == 0)
		return 1;
	
// Check block "to" position
  if ((Frame[Color_XX][lay_to][y_pos_to]) & (uint8_t)(0x01 << x_pos_to))
		Block_Tmp_to = ((uint16_t)(0x01 << 0));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_XX][lay_to][y_pos_to]) & (uint8_t)(0x01 << (x_pos_to+1)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 1));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_XX][lay_to][y_pos_to]) & (uint8_t)(0x01 << (x_pos_to+2)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 2));

	if (Block_Size_y[Block_nr] > 1)
  {
  if ((Frame[Color_XX][lay_to][y_pos_to+1]) & (uint8_t)(0x01 << x_pos_to))
		Block_Tmp_to |= ((uint16_t)(0x01 << 3));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_XX][lay_to][y_pos_to+1]) & (uint8_t)(0x01 << (x_pos_to+1)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 4));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_XX][lay_to][y_pos_to+1]) & (uint8_t)(0x01 << (x_pos_to+2)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 5));
  }
	
	if (Block_Size_y[Block_nr] > 2)
  {
  if ((Frame[Color_XX][lay_to][y_pos_to+2]) & (uint8_t)(0x01 << x_pos_to))
		Block_Tmp_to |= ((uint16_t)(0x01 << 6));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_XX][lay_to][y_pos_to+2]) & (uint8_t)(0x01 << (x_pos_to+1)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 7));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_XX][lay_to][y_pos_to+2]) & (uint8_t)(0x01 << (x_pos_to+2)))
		Block_Tmp_to |= ((uint16_t)(0x01 << 8));
  }
	
// Check block "from" position
  if ((Frame[Color_Move][lay_from][y_pos_from]) & (uint8_t)(0x01 << x_pos_from))
		Block_Tmp_from = ((uint16_t)(0x01 << 0));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_Move][lay_from][y_pos_from]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 1));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_Move][lay_from][y_pos_from]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 2));

 	if (Block_Size_y[Block_nr] > 1)
  {
	if ((Frame[Color_Move][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << x_pos_from))
		Block_Tmp_from |= ((uint16_t)(0x01 << 3));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_Move][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 4));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_Move][lay_from][y_pos_from+1]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 5));
  }
	
  if (Block_Size_y[Block_nr] > 2)
  {
  if ((Frame[Color_Move][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << x_pos_from))
		Block_Tmp_from |= ((uint16_t)(0x01 << 6));

	if (Block_Size_x[Block_nr] > 1)
  if ((Frame[Color_Move][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << (x_pos_from+1)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 7));

	if (Block_Size_x[Block_nr] > 2)
  if ((Frame[Color_Move][lay_from][y_pos_from+2]) & (uint8_t)(0x01 << (x_pos_from+2)))
		Block_Tmp_from |= ((uint16_t)(0x01 << 8));
  }
	
	
	if ((Block_Tmp_to & Block_Tmp_from) == 0)
		return 0;
	 else
		return 1;
}


void WaitFrame(void)
{
 while (Next_Frame == 0){}
 Next_Frame = 0;
}


uint8_t Tetris(void)																																				//function for the main operation of the game
{
  uint8_t pos_x = 2;
  uint8_t pos_y = 2;
  uint8_t pos_z = 7;

  uint8_t new_pos_x = 0;
  uint8_t new_pos_y = 0;
  uint8_t new_pos_z = 0;
	
	uint8_t New_Block = 1;																																		//if New_Block = 1, new block is generated

  Clear_Frame(Frame[RED]);
  Clear_Frame(Frame[GREEN]);
  Clear_Frame(Frame[BLUE]);

	Frame_Br[GREEN] = 45;
	Frame_Br[BLUE] = 45;
	Frame_Br[RED] = 60;
	
  WaitFrame();

  while(1)
	{
	  if(New_Block == 1)
		{ 
		 pos_x = 2;
     pos_y = 2;
     pos_z = 7;

  	 srand((uint16_t)TPM0->CNT);
		 Block_nr = (uint8_t)rand()%10;  

		 Draw_Block(pos_x, pos_y, pos_z, Block_nr, GREEN, 100);

		 Keyboard_UART = 0;
	   while (Next_Frame == 0)																																	//for vertical movement
			{
			 if (Keyboard_UART != 0)																																//if any input, do something
			 {	 
			 switch(Keyboard_UART)																																	//depending on the key pressed, we move the block horizontally
		   {
			  case BUTTON_3:  // Back
				  if (pos_y >= 1)	
				    pos_y--;
   	      ClearLayer(pos_z,GREEN,0);
			    Draw_Block(pos_x, pos_y, pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_7:  // Forw
					if (pos_y <= (7 - Block_Size_y[Block_nr])) 
				    pos_y++;
   	      ClearLayer(pos_z,GREEN,0);
			    Draw_Block(pos_x, pos_y, pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_5:  // Left
					if (pos_x >= 1)
				    pos_x--;
   	      ClearLayer(pos_z,GREEN,0);
			    Draw_Block(pos_x, pos_y, pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_6:  // Right
					if (pos_x <= (7 - Block_Size_x[Block_nr]))
				    pos_x++;
   	      ClearLayer(pos_z,GREEN,0);
			    Draw_Block(pos_x, pos_y, pos_z, Block_nr, GREEN, 0);
				  break;
			  default:
				  break;
		   }
       Keyboard_UART = 0;																																								//Reseting the input
		  }
		 }
		 Next_Frame = 0;
		 New_Block = 0;
		}

		 new_pos_x = pos_x;	
		 new_pos_y = pos_y;
		 if (pos_z > 0)																																											//if not ground level, we are moving down
		  new_pos_z = pos_z - 1;
		
    if (Test_New_Position(pos_x, pos_y, pos_z, new_pos_x, new_pos_y, new_pos_z, GREEN, BLUE) == 0)			//if sapace below us is free, we are moving the block down
		{
  		Move_Block(pos_x, pos_y, pos_z, new_pos_x, new_pos_y, new_pos_z, GREEN, 100);
	  }
		else																																																//if space below us is occupied, we are changin the color of the block
		{
			Switch_Layer(pos_z, GREEN, BLUE);
			if (Tetris_End() == 0)																																						//checking whether game over condition occured
			{
			 SetLayer(7, RED, 100);
			 while(Keyboard == 0) {}
			 return 0;
			}
			if (Test_Layer_Full(pos_z, BLUE) == 1)																														//if layer is full, we clear it
			{	
			  ClearLayer(pos_z,BLUE, 0);																																			
			  Move_Down(pos_z, BLUE);																																					//and move all the levels above the clear level down
			}
			New_Block = 1;																																										//letting the program know that it should generate new block
			new_pos_z = 7;
		}
		Keyboard_UART = 0;
		while (Next_Frame == 0)																																							//overall pretty much same as above but for the layers 6-0
			{
	     if (Keyboard_UART != 0)
    	 {	
			 switch(Keyboard_UART)
		   {
			  case BUTTON_3:  // Back
					if (new_pos_y >= 1)
				    new_pos_y--;
  				ClearLayer(new_pos_z,GREEN,0);
	  		  Draw_Block(new_pos_x, new_pos_y, new_pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_7:  // Forw
					if (new_pos_y <= (7 - Block_Size_y[Block_nr]))
				    new_pos_y++;
  				ClearLayer(new_pos_z,GREEN,0);
	  		  Draw_Block(new_pos_x, new_pos_y, new_pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_5:  // Left
					if (new_pos_x >=1)
				    new_pos_x--;
  				ClearLayer(new_pos_z,GREEN,0);
	  		  Draw_Block(new_pos_x, new_pos_y, new_pos_z, Block_nr, GREEN, 0);
				  break;
			  case BUTTON_6:  // Right
					if (new_pos_x<= (7 - Block_Size_x[Block_nr]))
				    new_pos_x++;
  				ClearLayer(new_pos_z,GREEN,0);
	  		  Draw_Block(new_pos_x, new_pos_y, new_pos_z, Block_nr, GREEN, 0);
				  break;
			  default:
				  break;
		   }
		  Keyboard_UART = 0;
		  }
			}
      Next_Frame = 0;
		
		pos_x = new_pos_x;
		pos_y = new_pos_y;
		pos_z = new_pos_z;
	}	
}	

// == 0 -> you lose
// == 1 -> not end
uint8_t Tetris_End(void)																																							//checking if after movement the ceiling level is free
{																																																			//if no, it's the end of the game
	uint8_t test = 1;
	
	for(uint8_t i = 0; i < 8; i++)
	 if (Frame[BLUE][7][i] != 0x00)
		 test = 0;
	
	return test;
}

void Switch_Layer(uint8_t lay, uint8_t Color_from, uint16_t Color_to)																	//function for switching color of one layer to another color
{
	for(uint16_t i = 0; i < 8; i++)
	 Frame[Color_to][lay][i] |= Frame[Color_from][lay][i];
	
	ClearLayer(lay, Color_from, 0);
	
}


// == 0 -> layer not full
// == 1 -> layer full
uint8_t Test_Layer_Full(uint8_t lay, uint8_t Color)																											//checking if the layer is full
{																																																				
 uint8_t test = 1;
 
 for(uint8_t i = 0; i < 8; i++)
	if (Frame[Color][lay][i] != 0xff)
		test = 0;

 return test;
}

void Move_Down(uint8_t lay_start, uint8_t Color)																												//function for moving down
{
	for(uint8_t i = lay_start; i < 7; i++)
	 for(uint8_t j = 0; j < 8; j++)
	  Frame[Color][i][j] = Frame[Color][i+1][j];
}
