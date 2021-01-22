#include "MKL05Z4.h"                    	/* Device header */

#define MAX_LENGTH 50

void Draw_Snake(uint8_t Time, uint8_t Br, uint8_t Color, uint8_t tbl[8][8]);
uint8_t Move_Snake(uint8_t Dir);
void DrawApple(void);
uint8_t EatApple(uint8_t xxx, uint8_t yyy, uint8_t zzz);
void Kaput(void);
void Snake_Appear(void);
void Restart_Procedure(void);
void Clear_Frame(uint8_t tbl[8][8]);							//Clears certain color
uint8_t Snake(void);

