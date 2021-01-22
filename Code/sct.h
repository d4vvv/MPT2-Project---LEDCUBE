#include "MKL05Z4.h"

#define SET_CLK FPTB->PSOR |= (1 << 0)
#define CLR_CLK FPTB->PCOR |= (1 << 0)

#define SET_LE FPTB->PSOR |= (1 << 7)
#define CLR_LE FPTB->PCOR |= (1 << 7)

#define SET_SDI1 FPTB->PSOR |= (1 << 3)
#define CLR_SDI1 FPTB->PCOR |= (1 << 3)

#define SET_SDI2 FPTB->PSOR |= (1 << 4)
#define CLR_SDI2 FPTB->PCOR |= (1 << 4)

#define SET_SDI3 FPTB->PSOR |= (1 << 5)
#define CLR_SDI3 FPTB->PCOR |= (1 << 5)

void Send8BytesT(uint8_t *TableR, uint8_t *TableG, uint8_t *TableB );
