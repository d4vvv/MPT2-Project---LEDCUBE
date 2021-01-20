#include "MKL05Z4.h"                    	/* Device header */
#include "snake.h"
#include <stdlib.h>
#include "demo.h"

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

static uint8_t Apple_X = 2;
static uint8_t Apple_Y = 2;
static uint8_t Apple_Z = 2;
static uint8_t Current_Snake_Length;
static uint8_t Restart_Game = 1;

extern uint8_t Move_Status;
extern uint8_t Snake_Display_Time;

extern uint8_t Layer;															//Current layer number                   
extern uint8_t Frame_Br[3];												//Color Brightness (Color brightness in one frame: 0-RED 1-GREEN 2-BLUE)
extern uint8_t Frame[3][8][8];										//Frame [COLOR][LAYER][ROW]
extern uint8_t snake[MAX_LENGTH][3];
extern volatile uint8_t Keyboard;				  				//Variable for storing information about currently ressed key (1 - the button is pressed)
extern volatile uint8_t Keyboard_UART;				  	//Variable for storing information about currently ressed key (1 - the button is pressed)
extern uint8_t Direction;
extern volatile uint8_t UART_Restart;

extern volatile uint8_t Frame_Time;								//Time of the frame being displayed  (n*16ms - there are 8 layers and each of them is displayed for 2ms - Systick Configuration)
extern volatile uint8_t Current_Frame_Time; 			//Time of the current frame				
extern volatile uint8_t Next_Frame;								//Control variable for determining weather next frame should be generated

uint8_t Move_Status;
uint8_t Snake_Display_Time = 50;

void Draw_Snake(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8])
{
 Frame_Time = Time;
 Frame_Br[Color] = Br;	

 Clear_Frame(Frame[Color]);
	
 for(uint8_t i=0; i<MAX_LENGTH; i++)
		{
		 if (snake[i][0] != 255)
			 tbl[snake[i][2]][snake[i][1]] |= (uint8_t)(1 << snake[i][0]);
		}
 while (Next_Frame == 0){}
 Next_Frame = 0;

}

// Directions
// Dir == 0 -> move right
// Dir == 1 -> move left
// Dir == 2 -> move forward
// Dir == 3 -> move backward
// Dir == 4 -> move up
// Dir == 5 -> move down
// Result 
// == 0 -> OK
// == 1 -> Outside the cube
// == 2 -> Struck into itself

uint8_t Move_Snake(uint8_t Dir)							//Function handling movement of the snake
{
 uint8_t ptr = 0;
 uint8_t result_ok = 0;
 uint8_t xxx = 0;	
 uint8_t yyy = 0;	
 uint8_t zzz = 0;	
	
 uint8_t xxx2 = 0;	
 uint8_t yyy2 = 0;	
 uint8_t zzz2 = 0;	
	
 for(ptr = 0; ptr < MAX_LENGTH; ptr++)
	if (snake[ptr][0] == 255)
		break;
 
 xxx = snake[ptr-1][0];
 yyy = snake[ptr-1][1];
 zzz = snake[ptr-1][2];
 
 xxx2 = snake[ptr-1][0];
 yyy2 = snake[ptr-1][1];
 zzz2 = snake[ptr-1][2];
	
 switch(Dir)
 {
	 case 0: xxx += 1;
					 break;
	 
	 case 1: xxx -= 1;
					 break;

	 case 2: yyy += 1;
					 break;
	 
	 case 3: yyy -= 1;
					 break;

	 case 4: zzz += 1;
					 break;
	 
	 case 5: zzz -= 1;
					 break;
 }

 if ((xxx == 8) || (yyy == 8) || (zzz == 8) || (xxx == 255) || (yyy == 255) || (zzz == 255))
 {
  result_ok = 1;
 }
 else	 
 {
	for (uint8_t ptr1 = 0; ptr1 < MAX_LENGTH; ptr1++)
	  {
		 if ((snake[ptr1][0] == xxx) && 	(snake[ptr1][1] == yyy) &&(snake[ptr1][2] == zzz))
		 {
			result_ok = 2;
			break; 
		 } 
		}
		
	if ((result_ok == 0))
	{
 	 snake[ptr][0] = xxx; 
	 snake[ptr][1] = yyy; 
	 snake[ptr][2] = zzz; 
	 Current_Snake_Length = ptr;
	}
 }
 
 if ((ptr > 1) && (result_ok == 0) && (EatApple(xxx2,yyy2,zzz2) == 0))
  for(ptr = 0; ptr < MAX_LENGTH; ptr++)
	 {
    snake[ptr][0] = snake[ptr+1][0];
    snake[ptr][1] = snake[ptr+1][1];
    snake[ptr][2] = snake[ptr+1][2];
    if (snake[ptr+1][0] == 255)
	 	 break;
	 }
	
 return result_ok;
}	

void DrawApple(void)					//Creating "apples" for the snake to "eat"
{
	uint8_t checc = 0;
	
	srand((uint16_t)TPM0->CNT);
	
	while(checc == 0)
		{
		uint8_t xr = (uint8_t)rand()%8;
		uint8_t yr = (uint8_t)rand()%8;
		uint8_t zr = (uint8_t)rand()%8;
		for (uint8_t ptr1 = 0; ptr1 < MAX_LENGTH; ptr1++)
			{
				if((snake[ptr1][0] == xr) && 	(snake[ptr1][1] == yr) &&(snake[ptr1][2] == zr))
				{
					checc = 1;
				}
			}
		if(checc == 1)
			{
				checc = 0;
			}
		else
			{
				Apple_X = xr;
				Apple_Y = yr;
				Apple_Z = zr;
				checc = 1;
			}
		}
	Clear_Frame(Frame[RED]);
	Frame[RED][Apple_Z][Apple_Y] = (uint8_t)(1<<Apple_X);
}

uint8_t EatApple(uint8_t xxx, uint8_t yyy, uint8_t zzz)						//Handling "eating" of an apple
{
	uint8_t result;
	if((xxx == Apple_X)&&(yyy == Apple_Y)&&(zzz == Apple_Z))
	{
		DrawApple();
		if(Current_Snake_Length%10 == 0)
		{
			Snake_Display_Time = Snake_Display_Time - 6;
		}
		if(Current_Snake_Length == MAX_LENGTH - 1)  //Victory check
		{
			while(Restart_Game == 0)
			{
				Demo_Contour(BLUE,5,15,Frame[BLUE],2);
				if(UART_Restart == 1)
					{
						UART_Restart = 0;
						Restart_Procedure();
					}
			}
			Restart_Game = 0;
		}
		result = 1;
	}
	else
	{
		result = 0;
	}
	return result;
}

void Kaput(void)												//Handling GameOver situation
{
	Clear_Frame(Frame[GREEN]);
	Draw_Snake(25,40,BLUE,Frame[BLUE]);
	uint8_t ptr;
	Restart_Game = 0;
	
	for(ptr = 0; ptr < MAX_LENGTH; ptr++)
	 {
    if (snake[ptr+1][0] == 255)
	 	 break;
	 }
	
	 for(uint8_t ptr2 = 0; ptr2 < ptr; ptr2++)
	 {
		for(uint8_t ptr3 = 0; ptr3 < ptr-ptr2; ptr3++)
		 {
			snake[ptr3][0] = snake[ptr3+1][0];
			snake[ptr3][1] = snake[ptr3+1][1];
			snake[ptr3][2] = snake[ptr3+1][2];
		 }
		 Clear_Frame(Frame[BLUE]);
		 Draw_Snake(5,40,BLUE,Frame[BLUE]);
		 
	 }
	 Clear_Frame(Frame[BLUE]);
	 Clear_Frame(Frame[RED]);
	 
   Frame_Time = 150;
	 Frame_Br[BLUE] = 50;
	 uint8_t count = 0;
	 
	 for(uint8_t ii = 0; ii < 8; ii++)
	  for(uint8_t jj = 0; jj < 8; jj++)
	   {
      if (++count > (Current_Snake_Length -5))
        break;				
			Frame[BLUE][ii][0] |= (uint8_t)(1 << jj);
		 }
		while (Next_Frame == 0){}
		Next_Frame = 0;   
	 
	 while(Restart_Game == 0)																	//Waiting for user input to restart game
	 {
		 Frame_Br[BLUE] = 50;
		 uint8_t test = 1;
		 Demo_Contour(BLUE,4,15,Frame[BLUE],2);
  	 if((UART_Restart == 1) || (Keyboard == BUTTON_4))			 
		 {
			UART_Restart = 0;
  		Restart_Procedure();
      test = 0;
      return;
		 }
		 
		 if (test)
		 Demo_Contour(RED,4,15,Frame[RED],2);
  	 if((UART_Restart == 1) || (Keyboard == BUTTON_4))
		 {
			UART_Restart = 0;
			Restart_Procedure();
      test = 0;
      return;
		 }

 		 if (test)
		 Demo_Contour(GREEN,4,15,Frame[GREEN],2);
  	 if((UART_Restart == 1) || (Keyboard == BUTTON_4))
		 {
			UART_Restart = 0;
			Restart_Procedure();
      return;
		 }
	 }
	 Restart_Game = 0;
}

void Snake_Appear(void)												//Function making the snake appear with an animation
{
		snake[0][0] = 0; snake[0][1] = 0; snake[0][2] = 0;
		Draw_Snake(12, 1, BLUE, Frame[BLUE]);
		snake[1][0] = 0; snake[1][1] = 0; snake[1][2] = 1;
		Draw_Snake(12, 9, BLUE, Frame[BLUE]);
		snake[2][0] = 0; snake[2][1] = 0; snake[2][2] = 2;
		Draw_Snake(12, 17, BLUE, Frame[BLUE]);
		snake[3][0] = 1; snake[3][1] = 0; snake[3][2] = 2;
		Draw_Snake(12, 25, BLUE, Frame[BLUE]);
		snake[4][0] = 1; snake[4][1] = 1; snake[4][2] = 2;
		Draw_Snake(12, 30, BLUE, Frame[BLUE]);
		Clear_Frame(Frame[BLUE]);
		Frame_Br[RED] = 0;
    DrawApple();
		for(uint8_t step = 5; step <=65; step = step +5)
		{
			Draw_Snake(10, step, GREEN, Frame[GREEN]);
			Frame_Br[RED] = step;
		}
}

void Restart_Procedure(void)									//Function making restart of the game possible
{
			 Restart_Game = 1;
			 Move_Status = 0;
			 Direction = RIGHT;
		   Keyboard_UART = 0; 
			 Snake_Display_Time = 50;
			 snake[0][0] = 0; snake[0][1] = 0; snake[0][2] = 0;
			 snake[1][0] = 0; snake[1][1] = 0; snake[1][2] = 1;
			 snake[2][0] = 0; snake[2][1] = 0; snake[2][2] = 2;
			 snake[3][0] = 1; snake[3][1] = 0; snake[3][2] = 2;
			 snake[4][0] = 1; snake[4][1] = 1; snake[4][2] = 2;
			 for(uint8_t cnt = 0; cnt < MAX_LENGTH; cnt++)
					{
						snake[cnt][0] = 255;
						snake[cnt][1] = 0;
						snake[cnt][2] = 0;
					}
}
