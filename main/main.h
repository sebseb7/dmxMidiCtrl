#ifndef _MAIN_H
#define _MAIN_H

#include <stdint.h>

#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

//#define KTRL_F1
//#define TOUCH_OSC
#define KORG_CTRL
//#define LAUNCHPAD

//#define WAVECLOCK

#define LIBFTDI
//#define FTD2xx 

enum {
	PAR36 = 1,
	PAR36_UV = 2,
	PAR56 = 3,
	QUADPHASE = 4,
	FOG = 5
};


typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);


void registerAnimation(init_fun init,tick_fun tick,deinit_fun deinit, uint16_t type, uint16_t t, uint16_t duration, uint8_t idle);

void setLedX(uint8_t x, uint8_t r,uint8_t g,uint8_t b);
void setCh(uint8_t chan, uint8_t value);
uint8_t getIn(uint8_t chan);
void invLedX(uint8_t x);
void Delay(uint16_t t);
void getLedX(uint8_t x, uint8_t* red, uint8_t* green, uint8_t* blue);


#endif

