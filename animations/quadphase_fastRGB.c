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


		setCh(16,0);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,0);
		setCh(24,0);



	//printf("%i\n",a);

	if((a % 3) ==0)
	{
		setCh(1,1);
		setCh(5,1);

		if(b > 200)
		{
			setCh(16,0);
			setCh(17,255);
			setCh(18,0);
			setCh(22,0);
			setCh(23,0);
			setCh(24,255);
		}
	
	}
	if((a % 3) ==1)
	{
		setCh(1,17);
		setCh(5,17);
		
		if(b > 200)
		{
		setCh(16,0);
		setCh(17,0);
		setCh(18,255);
		setCh(22,255);
		setCh(23,0);
		setCh(24,0);
		}
	}
	if((a % 3) ==2)
	{
		setCh(1,34);
		setCh(5,34);
		
		if(b > 200)
		{
		setCh(16,255);
		setCh(17,0);
		setCh(18,0);
		setCh(22,0);
		setCh(23,255);
		setCh(24,0);
		}
	}
	setCh(2,20);
	setCh(3,0);
	setCh(4,255);
	setCh(6,20);
	setCh(7,0);
	setCh(8,255);
	
	
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
	registerAnimation(init,tick,deinit, QUADPHASE,30, 10,1);
}


