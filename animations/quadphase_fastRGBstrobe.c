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

	if(b == 30)
	{
		setCh(1,1);
		setCh(4,255);
		setCh(5,1);
		setCh(8,255);
	}
	if(b == 32)
	{
		setCh(4,0);
		setCh(8,0);
	}
	if(b == 40)
	{
		setCh(16,255);
		setCh(17,0);
		setCh(18,0);
		setCh(22,255);
		setCh(23,0);
		setCh(24,0);
	}
	if(b == 41)
	{
		setCh(16,0);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,0);
		setCh(24,0);
	}
	if(b == 50)
	{
		setCh(16,255);
		setCh(17,0);
		setCh(18,0);
		setCh(22,255);
		setCh(23,0);
		setCh(24,0);
	}
	if(b == 51)
	{
		setCh(16,0);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,0);
		setCh(24,0);
	}
	if(b == 60)
	{
		setCh(1,18);
		setCh(4,255);
		setCh(5,18);
		setCh(8,255);
	}
	if(b == 62)
	{
		setCh(4,0);
		setCh(8,0);
	}
	if(b == 90)
	{
		setCh(1,35);
		setCh(4,255);
		setCh(5,35);
		setCh(8,255);
	}
	if(b == 92)
	{
		setCh(4,0);
		setCh(8,0);
		b=0;
	}

	setCh(2,60);
	setCh(3,0);
	setCh(6,60);
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
	b=0;
}


static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, QUADPHASE,80, 10,1);
}


