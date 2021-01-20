#include "MKL05Z4.h"                    	/* Device header */
#include "tpm.h"													/* pwm */
#include "sct.h"                          /* SCT2167 header */
#include <stdio.h>
#include <stdlib.h>
#include "uart0.h"
#include "tetris.h"
#include "snake.h"
#include "demo.h"

#define Layer1 5	                        /* Defintion of the layers of the cube, PTA5 - PTA12 */
#define Layer2 6
#define Layer3 7
#define Layer4 8
#define Layer5 9
#define Layer6 10
#define Layer7 11
#define Layer8 12

#define RED 0
#define GREEN 1
#define BLUE 2

#define FORWARD 2
#define BACKWARD 3
#define LEFT 1
#define RIGHT 0
#define UP 4
#define DOWN 5
#define NONE 7

#define CLK_Pin 0                         /* Definition of Clock, PTB0 */
#define LE_Pin 7                          /* Definition of LE, PTB7 */
#define SDI1_Pin 3                        /* Definition of Serial Data Input 1, PTB3 */
#define SDI2_Pin 4                        /* Definition of Serial Data Input 2, PTB4 */
#define SDI3_Pin 5                        /* Definition of Serial Data Input 3, PTB5 */

#define BUTTON_1 0x01
#define BUTTON_2 0x02
#define BUTTON_3 0x04
#define BUTTON_4 0x08
#define BUTTON_5 0x10
#define BUTTON_6 0x20
#define BUTTON_7 0x40
#define BUTTON_8 0x80

extern uint8_t Layer;															//Current layer number                   
extern uint8_t Frame_Br[3];												//Color Brightness (Color brightness in one frame: 0-RED 1-GREEN 2-BLUE)
extern uint8_t Frame[3][8][8];										//Frame [COLOR][LAYER][ROW]

extern volatile uint8_t Frame_Time;								//Time of the frame being displayed  (n*16ms - there are 8 layers and each of them is displayed for 2ms - Systick Configuration)
extern volatile uint8_t Current_Frame_Time; 			//Time of the current frame		
extern volatile uint8_t Next_Frame;								//Control variable for determining weather next frame should be generated
extern uint8_t snake[50][3];											//"Body" of the snake
extern volatile uint8_t Keyboard;				  				//Variable for storing information about currently ressed key (1 - the button is pressed)
extern volatile uint8_t Keyboard_UART;				  	//Variable for storing information about currently ressed key (1 - the button is pressed)
extern volatile uint8_t UART_RX;
extern volatile uint8_t UART_Restart;
extern uint8_t Direction;
extern volatile uint8_t Next_Box;
extern uint8_t Move_Status;
extern uint8_t Snake_Display_Time;

void SysTick_Handler(void);												

// Declaration of necessery variables  
uint8_t Frame_Br[3] = {30,20,20};					   	
uint8_t Layer = 0;			

uint8_t Direction = RIGHT;

static uint8_t Game_Switch = 0; // == 0 -> Snake; == 1 -> Tetrix
volatile uint8_t Keyboard;
volatile uint8_t Keyboard_UART;

volatile uint8_t Frame_Time = 20;						// The amount of time the current frame should light (1 => ~16ms; 1s => 62)
volatile uint8_t Current_Frame_Time = 0;		// The amount of time the current frame is already lighting
volatile uint8_t Next_Frame = 0;						// ==1 -> switch to next frame
volatile uint8_t Next_Box = 0;						  // ==1 -> switch to next box
volatile uint8_t UART_RX = 0;
volatile uint8_t UART_Restart = 0;

uint8_t snake[MAX_LENGTH][3] = {{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0},
																{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0},
																{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0},
																{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0},
																{255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}, {255,0,0}};


/*static const uint8_t Frame_X[3][8][8] = {						// That's the way to hardcode some frame
{
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
},{
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
},{
{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
}
};*/

uint8_t Frame[3][8][8] = {															// Initialization of one frame // e.g -> [0][1][4] - red led on fourth row of the bottom+1 layer
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},              
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}};

int main(void) {
	
  SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK;       /* Enable Clock to Port A and B */ 
	
	PORTA->GPCLR  = (((1 << Layer1) | (1 << Layer2) | (1 << Layer3) | (1 << Layer4) | (1 << Layer5) | (1 << Layer6) | (1 << Layer7) | (1 << Layer8))<<16) | PORT_PCR_MUX(1);
	FPTA->PDDR   |=   (1 << Layer1) | (1 << Layer2) | (1 << Layer3) | (1 << Layer4) | (1 << Layer5) | (1 << Layer6) | (1 << Layer7) | (1 << Layer8) ;       

	PORTB->GPCLR  = (((1 << CLK_Pin) | (1 << SDI1_Pin) | (1 << SDI2_Pin) | (1 << SDI3_Pin) | (1 << LE_Pin))<<16) | PORT_PCR_MUX(1);
	FPTB->PDDR   |=   (1 << CLK_Pin) | (1 << SDI1_Pin) | (1 << SDI2_Pin) | (1 << SDI3_Pin) | (1 << LE_Pin); 

	PORTB->PCR[1] = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_MUX(1);	
//	PORTB->PCR[2] = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_MUX(1);	
	PORTB->PCR[8] = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_MUX(1);	
	PORTB->PCR[9] = PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_MUX(1);	
	
	PORTB->PCR[13] = PORT_PCR_MUX(1);	
	PORTA->PCR[0] = PORT_PCR_MUX(1);	
	
  FPTB->PDDR |= (1 << 13);
	FPTA->PDDR |= (1 << 0);
	
	PTB->PSOR |= (1 << 13);
	PTA->PSOR |= (1 << 0);
	
	SysTick_Config(SystemCoreClock / 500);	// Interrupt every ~2ms

	TPM0_Init_PWM();
	
	UART0_Init();
	
	while(1){	

		Demo3_Color(15);

		Demo2_Color(30, 1, GREEN, Frame[GREEN]);																													//signalizing that the game is about to begin
    Demo2_Color(30, 3, BLUE, Frame[BLUE]);
		Demo2_Color(30, 5, RED, Frame[RED]);

    if(Keyboard == (BUTTON_3))																																				//checking physical keyboard for game switch key press
		{  
		  Direction = NONE;
			Game_Switch = 1;
		}
		else
		{
		  Direction = RIGHT;
			Game_Switch = 0;
		}

    if (Game_Switch == 0)																																							//beginning of the snake game
		{
		 Clear_Frame(Frame[GREEN]);
		 Snake_Appear();
		}
			
	while(1)																																														//checking and verifying input from the user
	{
		switch(Keyboard_UART)
		{
			case BUTTON_3:
				if(Direction != FORWARD)
				{
					Direction = BACKWARD;
				}
				break;
			case BUTTON_7:
				if(Direction != BACKWARD)
				{
					Direction = FORWARD;
				}
				break;
			case BUTTON_4:
				if(Direction != UP)
				{
					Direction = DOWN;
				}
				break;
			case BUTTON_5:
				if(Direction != RIGHT)
				{
					Direction = LEFT;
				}
				break;
			case BUTTON_6:
				if(Direction != LEFT)
				{
					Direction = RIGHT;
				}
				break;
			case BUTTON_8:
				if(Direction != DOWN)
				{
					Direction = UP;
				}
				break;
		}
		Keyboard_UART = 0;
		
		if (Game_Switch == 0)
		{
		  Move_Status = Move_Snake(Direction);																														//Checking if any game over condition happened
		  if ((Move_Status == 1)||(Move_Status == 2))
		   {
			  Kaput();
				break;
		   }
		  Draw_Snake(Snake_Display_Time, 40, GREEN, Frame[GREEN]);
	  }

		if (Game_Switch == 1)
		{
     Tetris();
		 break;
		}
	}
}
}

