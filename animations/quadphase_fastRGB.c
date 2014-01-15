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

	if(b < 100)
	{
		a++;
	}
	else if (b < 200)
	{
		if((b%3)==0)
		{
			a++;
		}
	}
	else if (b < 300)
	{
		if((b%10)==0)
		{
			a++;
		}
	}
	else
	{
		b=0;
	}





	//printf("%i\n",a);

	if((a % 3) ==0)
	{
		setCh(1,1);
	}
	if((a % 3) ==1)
	{
		setCh(1,17);
	}
	if((a % 3) ==2)
	{
		setCh(1,34);
	}
	setCh(2,20);
	setCh(3,0);
	setCh(4,255);

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
	registerAnimation(init,tick,deinit, QUADPHASE,30, 300,1);
}


