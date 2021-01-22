#include "MKL05Z4.h"                    	/* Device header */

void Clear_Frame(uint8_t tbl[8][8]);
void ClearLayer(uint8_t lay, uint8_t Color, uint8_t Time);
void SetLayer(uint8_t lay, uint8_t Color, uint8_t Time);
void WaitFrame(void);
void Draw_Block(uint8_t x_pos, uint8_t y_pos, uint8_t lay, uint8_t block_nr, uint8_t Color, uint8_t Time);
void Move_Block(uint8_t x_pos_from, uint8_t y_pos_from, uint8_t lay_from, uint8_t x_pos_to, uint8_t y_pos_to, uint8_t lay_to, uint8_t Color, uint8_t Time);
uint8_t Test_New_Position(uint8_t x_pos_from, uint8_t y_pos_from, uint8_t lay_from, uint8_t x_pos_to, uint8_t y_pos_to, uint8_t lay_to, uint8_t Color_Move, uint8_t Color_XX);
uint8_t Tetris(void);
uint8_t Tetris_End(void);
void Switch_Layer(uint8_t lay, uint8_t Color_from, uint16_t Color_to);
uint8_t Test_Layer_Full(uint8_t lay, uint8_t Color);
void Move_Down(uint8_t lay_start, uint8_t Color);
