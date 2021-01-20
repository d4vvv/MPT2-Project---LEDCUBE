#include "MKL05Z4.h"
#include "tpm.h"

void TPM0_Init_PWM(void) {
		
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;	// Enable TPM0 mask in SCGC6 register
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  // Choose MCGFLLCLK clock source

	PORTB->PCR[11] = PORT_PCR_MUX(2);		// TPM0_CH0
	PORTB->PCR[10] = PORT_PCR_MUX(2);		// TPM0_CH1
	PORTB->PCR[6]  = PORT_PCR_MUX(2);		// TPM0_CH3

	TPM0->SC |= (TPM_SC_PS(7) | TPM_SC_CMOD(1)); 	// Set prescaler to 128, select the internal input clock source

	TPM0->MOD = 65; 										// Set modulo value to 65, the value chosen so we get period = ~0,2ms
	
	TPM0->SC &= ~TPM_SC_CPWMS_MASK; 		/* up counting */
	TPM0->CONTROLS[0].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK); /* clear Output on match, set Output on reload */ 
	TPM0->CONTROLS[1].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK); 
	TPM0->CONTROLS[3].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK); 

	TPM0->CONTROLS[0].CnV = 0; 				// For CH0 -> according to the documentation this is reset value, so it is not necesserily
	TPM0->CONTROLS[1].CnV = 0; 				// For CH1 -> according to the documentation this is reset value, so it is not necesserily
	TPM0->CONTROLS[3].CnV = 0; 				// For_CH3 -> according to the documentation this is reset value, so it is not necesserily
}
