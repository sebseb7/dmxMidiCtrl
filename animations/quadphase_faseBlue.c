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

static uint8_t tick(void) 
{
	a++;

	//printf("%i\n",a);

	if(getIn(0) > 80)
	{
		setCh(1,35);
		setCh(5,1);
	}
	else if(getIn(0) > 48)
	{
		setCh(1,1);
		setCh(5,1);
	}
	else
	{
		setCh(1,1);
		setCh(5,35);
	}
	setCh(2,20);
	setCh(3,0);
	setCh(4,255);
	
	setCh(6,20);
	setCh(7,0);
	setCh(8,255);
	
	
	setCh(16,0);
	setCh(17,0);
	setCh(18,0);
	setCh(19,0);
	setCh(20,0);
	setCh(21,0);
	
	setCh(22,0);
	setCh(23,0);
	setCh(24,0);
	setCh(25,0);
	setCh(26,0);
	setCh(27,0);
	
	setCh(234,0);
	setCh(235,0);
	setCh(236,1);

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
	registerAnimation(init,tick,deinit, QUADPHASE,30, 30,1);
}


