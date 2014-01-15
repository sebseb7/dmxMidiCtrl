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
uint16_t b;

static uint8_t tick(void) 
{
	b++;

	if(b == 20)
	{
		setCh(1,1);
		setCh(4,255);
	}
	if(b == 22)
	{
		setCh(4,0);
	}
	if(b == 40)
	{
		setCh(1,18);
		setCh(4,255);
	}
	if(b == 42)
	{
		setCh(4,0);
	}
	if(b == 60)
	{
		setCh(1,35);
		setCh(4,255);
	}
	if(b == 62)
	{
		setCh(4,0);
		b=0;
	}

	setCh(2,60);
	setCh(3,0);

	return 1;
}

static void init(void)
{
	a=0;
	b=0;
}


static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, QUADPHASE,80, 10,1);
}


