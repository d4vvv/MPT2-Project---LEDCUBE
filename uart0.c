#include "uart0.h"

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

extern volatile uint8_t UART_RX;
extern uint8_t Frame[3][8][8];
extern volatile uint8_t UART_Restart;
extern volatile uint8_t Keyboard_UART;	

void UART0_IRQHandler(void);

void UART0_Init(void)
{
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;											// Enable clock for UART0
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(MCGFLLCLK);						// MCGFLLCLK=41943040Hz (CLOCK_SETUP=0)
	PORTB->PCR[2] = PORT_PCR_MUX(2);												// PTB2=RX_D
	
	UART0->C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK );		// Disable TX & RX
	UART0->BDH = ((273 & 0xFF00) >> 8);
	UART0->BDL = (273 & 0xFF);															// For CLOCK_SETUP=0 BR=9600	BDL=273
	UART0->C4 &= ~UART0_C4_OSR_MASK;
	UART0->C4 |= UART0_C4_OSR(15);													// For CLOCK_SETUP=0 BR=9600	OSR=15
	UART0->C5 |= UART0_C5_BOTHEDGE_MASK;										// Both edges sampling
	UART0->C2 |= UART0_C2_RIE_MASK;													// Enable IRQ from RX
	UART0->C2 |= UART0_C2_RE_MASK;													// Enable RX
	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_ClearPendingIRQ(UART0_IRQn);
}

void UART0_IRQHandler()
{
	if(UART0->S1 & UART0_S1_RDRF_MASK)
	{
		UART_RX=UART0->D;																			// Read data s flagi RDRF

		switch(UART_RX)																				// Setting proper values after certain key press		
		{
			case '2': 
        Keyboard_UART = BUTTON_4;
				break;   
			case '8': 
        Keyboard_UART = BUTTON_8;
				break;   
			case 'e': 
        Keyboard_UART = BUTTON_6;
				break;   
			case 's': 
        Keyboard_UART = BUTTON_3;
				break;   
			case 'q': 
        Keyboard_UART = BUTTON_5;
				break;   
			case 'w': 
        Keyboard_UART = BUTTON_7;
				break;   
			case 'R': 
        UART_Restart = 1;
				break;   
			default: 
				break;   
		}
		
	NVIC_EnableIRQ(UART0_IRQn);
	}
}
