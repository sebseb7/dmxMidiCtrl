#include "main.h"
#include <stdio.h>

#include "dmx_devices.h"

// ch1 : 1-16 red
// 		 17-33 green
// 		 34-50 blue


// ch2 : 0-9 no rot
//       10-120 clockwise
//       121-134 no
//       135-245 counter
//       246-249 no 
//       250-255 music

uint16_t a;

static uint8_t tick(void) 
{
	a++;

	set_quadphase(ADDR_QF1, -38,255,0,0);
	set_quadphase(ADDR_QF2, -38,255,0,0);

	set_par56(ADDR_LED1, 255,0,0);
	set_par56(ADDR_LED2, 255,0,0);
	set_par56(ADDR_LED3, 0,0,0);
	set_par56(ADDR_LED4, 255,0,0);

	return 1;
}

static void init(void)
{
	a=0;
}


static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, QUADPHASE,70, 10,1);
}


