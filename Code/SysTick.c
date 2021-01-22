#include "MKL05Z4.h"                    	/* Device header */
#include "tpm.h"													/* pwm */
#include "sct.h"                          /* SCT2167 header */

#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 3 
#define BUTTON_4 4 
#define BUTTON_5 5 
#define BUTTON_6 6
#define BUTTON_7 7

#define PTB1 1
#define PTB2 2
#define PTB8 8
#define PTB9 9
#define PTB13 13
#define PTA0 0

void SysTick_Handler(void);

extern uint8_t Layer;                             
extern uint8_t Frame_Br[3];								
extern uint8_t Frame[3][8][8];

extern volatile uint8_t Frame_Time;					
extern volatile uint8_t Current_Frame_Time; 
extern volatile uint8_t Current_Frame;			
extern volatile uint8_t Next_Frame;				  
extern volatile uint8_t Keyboard;
extern volatile uint8_t Next_Box;
static uint8_t Key_Count = 0;

void SysTick_Handler(void) {
  if (++Key_Count > 5)						//Handling pressing of the keys
	{
	 PTB->PCOR |= (1 << PTB13);
	 PTA->PSOR |= (1 << PTA0);
	 ((PTB->PDIR & (1<<PTB1) )) ? (Keyboard &= ~(1 << BUTTON_0)) : (Keyboard |= (1 << BUTTON_0));  
//	 ((PTB->PDIR & (1<<PTB2) )) ? (Keyboard &= ~(1 << BUTTON_1)) : (Keyboard |= (1 << BUTTON_1));
	 ((PTB->PDIR & (1<<PTB8) )) ? (Keyboard &= ~(1 << BUTTON_2)) : (Keyboard |= (1 << BUTTON_2));  
	 ((PTB->PDIR & (1<<PTB9) )) ? (Keyboard &= ~(1 << BUTTON_3)) : (Keyboard |= (1 << BUTTON_3));  
		
	 PTB->PSOR |= (1 << PTB13);
	 PTA->PCOR |= (1 << PTA0);			//Making dummy action in order to get some needed time
	 PTA->PCOR |= (1 << PTA0);			
	 ((PTB->PDIR & (1<<PTB1) )) ? (Keyboard &= ~(1 << BUTTON_4)) : (Keyboard |= (1 << BUTTON_4));  
//	 ((PTB->PDIR & (1<<PTB2) )) ? (Keyboard &= ~(1 << BUTTON_5)) : (Keyboard |= (1 << BUTTON_5));  
	 ((PTB->PDIR & (1<<PTB8) )) ? (Keyboard &= ~(1 << BUTTON_6)) : (Keyboard |= (1 << BUTTON_6));  
	 ((PTB->PDIR & (1<<PTB9) )) ? (Keyboard &= ~(1 << BUTTON_7)) : (Keyboard |= (1 << BUTTON_7));  
		
	 PTB->PSOR |= (1 << PTB13);
	 PTA->PSOR |= (1 << PTA0);
	 Key_Count = 0;
	}
  
	Layer++;
	if (Layer >= 8)
	{
		Current_Frame_Time++;
		Layer =0;
		Next_Box = 1;
		if (Current_Frame_Time >= Frame_Time)
			{
			 Current_Frame_Time = 0;
			 Next_Frame = 1;
			}
	}
	
	FPTA->PCOR |= 0x1FE0;
	
	Send8BytesT(Frame[2][Layer],Frame[1][Layer],Frame[0][Layer]);
	
	TPM0->CONTROLS[0].CnV = Frame_Br[2];           /* Blue LEDs brigthness */
	TPM0->CONTROLS[1].CnV = Frame_Br[1];           /* Green LEDs brigthness */
	TPM0->CONTROLS[3].CnV = Frame_Br[0];           /* Red LEDs brigthness */
	
	FPTA->PSOR |= (1 << (Layer + 5));
}
