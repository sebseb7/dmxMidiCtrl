#include "main.h"
#include <stdio.h>
#include "dmx_devices.h"

uint16_t a;

static uint8_t tick(void) 
{
	a++;

	if(((a % 80) == 0)||((a % 80) == 10))
	{
		set_quadphase(ADDR_QF1, -38,255,0,0);
		set_quadphase(ADDR_QF2, -38,255,0,0);

		set_par56(ADDR_LED1, 255,0,0);
		set_par56(ADDR_LED2, 255,0,0);
		set_par56(ADDR_LED3, 0,0,0);
		set_par56(ADDR_LED4, 255,0,0);
	}
	if(((a % 80) == 1)||((a % 80) == 11))
	{
		set_quadphase(ADDR_QF1, -38,0,0,0);
		set_quadphase(ADDR_QF2, -38,0,0,0);

		set_par56(ADDR_LED1, 0,0,0);
		set_par56(ADDR_LED2, 0,0,0);
		set_par56(ADDR_LED3, 0,0,0);
		set_par56(ADDR_LED4, 0,0,0);
	}
	if(((a % 80) == 40)||((a % 80) == 50))
	{
		set_quadphase(ADDR_QF1, -38,0,0,255);
		set_quadphase(ADDR_QF2, -38,0,0,255);

		set_par56(ADDR_LED1, 0,0,255);
		set_par56(ADDR_LED2, 0,0,255);
		set_par56(ADDR_LED3, 0,0,0);
		set_par56(ADDR_LED4, 0,0,255);
	}
	if(((a % 80) == 41)||((a % 80) == 51))
	{
		set_quadphase(ADDR_QF1, -38,0,0,0);
		set_quadphase(ADDR_QF2, -38,0,0,0);

		set_par56(ADDR_LED1, 0,0,0);
		set_par56(ADDR_LED2, 0,0,0);
		set_par56(ADDR_LED3, 0,0,0);
		set_par56(ADDR_LED4, 0,0,0);
	}


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


