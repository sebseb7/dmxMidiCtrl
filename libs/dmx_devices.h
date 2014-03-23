#ifndef _DMX_DEVICES_H
#define _DMX_DEVICES_H

#include "main.h"


void set_par56(uint16_t addr, uint8_t r,uint8_t g, uint8_t b);
void set_par32(uint16_t addr, uint8_t r,uint8_t g, uint8_t b);
void set_par32uv(uint16_t addr, uint8_t uv);
void set_quadphase(uint16_t addr, int8_t rotation,uint8_t r,uint8_t g, uint8_t b);
void set_fog(uint16_t addr, uint8_t state);


#define ADDR_QF1 1 //left
#define ADDR_QF2 5 //right
#define ADDR_LED1 16 // left 56'er
#define ADDR_LED2 22 // right 56'er
#define ADDR_LED3 28 // bar 56'er
//#define ADDR_LED4 34 // chill 56'er
#define ADDR_LED5 40 //DJ Spot
#define ADDR_LED6 46 //2nd Spot
#define ADDR_LED7 52 //uv 1
#define ADDR_LED8 58 //uv 2
#define ADDR_FOG  64


#define ADDR_LED4 234


#endif
