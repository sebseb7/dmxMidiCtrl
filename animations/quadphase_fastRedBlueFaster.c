#include "main.h"
#include <stdio.h>

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
uint16_t on = 0;

static uint8_t tick(void) 
{
	a++;

	if((a % 3) == 0)
	{
		setCh(4,255);
		setCh(8,255);
	}
	else
	{
		setCh(4,0);
		setCh(8,0);
	}

	if(((a>>1) % 2) == 0)
	{
		setCh(1,35);
		setCh(5,35);
		setCh(16,0);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,0);
		setCh(24,0);
		setCh(234,0);
		setCh(235,0);
		setCh(236,1);
	}
	else
	{
		setCh(1,10);
		setCh(5,10);
		setCh(16,0);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,0);
		setCh(24,0);
		setCh(234,1);
		setCh(235,0);
		setCh(236,0);
	}

	//printf("%i\n",a);
	if((((a+3)>>1) % 2) == 0)
	{
		setCh(2,10);
		setCh(6,10);
	}
	else
	{
		setCh(2,245);
		setCh(6,245);
	}
	setCh(3,0);
	setCh(7,0);

	setCh(19,0);
	setCh(20,0);
	setCh(21,0);

	setCh(25,0);
	setCh(26,0);
	setCh(27,0);

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
	registerAnimation(init,tick,deinit, QUADPHASE,12, 30,1);
}


