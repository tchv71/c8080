#pragma once

#include <stdint.h>

static const uint8_t TEXT_WIDTH = 64;
static const uint8_t TEXT_HEIGHT = 25;

void FillRectFast(uint8_t* a, uint16_t c, uint8_t l, uint8_t r, uint8_t h);
void DrawText6(uint8_t* addr, uint8_t step, uint8_t n, const char* text);

#define FILLRECTARGS(X0,Y,X1,Y1) (uint8_t*)((((X0)/8)*256) + (Y) + 0x9000), ((X1)+1)/8-(X0/8), (0xFF >> ((uint8_t)(X0) & 7)), (0xFF >> ((uint8_t)((X1)+1) & 7)) ^ 0xFF, (Y1)-(Y)+1

#define VERTLINEARGS(X,Y) (uint8_t*)((((X) / 8) * 256) + (Y) + 0x9000), 0x80 >> (uint8_t)((X) & 7)
#define HORZALINEARGS(X0,Y,X1) (uint8_t*)((((X0)/8)*256) + (Y) + 0x9000), ((X1)+1)/8-(X0/8), (0xFF >> ((uint8_t)(X0) & 7)), (0xFF >> ((uint8_t)((X1)+1) & 7)) ^ 0xFF
#define PRINTARGS(XX,YY) ((uint8_t*)(0x9000)+((YY)*10)+(((XX)*3/4)*256) ), (XX)&3
#define TEXTCOORDS(XX,YY) ((uint8_t*)(0x9000)+((YY)*10)+(((XX)*3/4)*256) )
#define PIXELCOORDS(XX,YY) ((uint8_t*)(0x9000)+(YY)+((XX)*256) )

void graph0(void);
void graph1(void);
void graphXor(void);
//void clrscr10(void* t, uint8_t w, uint8_t h);
void FillRect(uint16_t x, uint8_t y, uint16_t x1, uint8_t y1);
void DrawTextXY(uint8_t x, uint8_t y, const char* text);

