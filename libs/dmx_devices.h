#ifndef _DMX_DEVICES_H
#define _DMX_DEVICES_H

#include "main.h"


void set_par56(uint16_t addr, uint8_t r,uint8_t g, uint8_t b);
void set_par32(uint16_t addr, uint8_t r,uint8_t g, uint8_t b);
void set_par32uv(uint16_t addr, uint8_t uv);
void set_quadphase(uint16_t addr, int8_t rotation,uint8_t r,uint8_t g, uint8_t b);
void set_fog(uint16_t addr, uint8_t state);


#endif
