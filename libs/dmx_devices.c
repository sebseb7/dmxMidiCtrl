#include "dmx_devices.h"



void set_par56(uint16_t addr, uint8_t r,uint8_t g, uint8_t b)
{
	setCh(addr,r);
	setCh(addr+1,g);
	setCh(addr+2,b);
	if(addr < 100)
	{
		setCh(addr+3,0);
		setCh(addr+4,0);
		setCh(addr+5,0);
	}
}

void set_par32(uint16_t addr  __attribute__((__unused__)), uint8_t r  __attribute__((__unused__)),uint8_t g  __attribute__((__unused__)), uint8_t b  __attribute__((__unused__)))
{
}

void set_par32uv(uint16_t addr  __attribute__((__unused__)), uint8_t uv  __attribute__((__unused__)))
{
}


void set_quadphase(uint16_t addr, int8_t rotation,uint8_t r,uint8_t g, uint8_t b)
{
	
	if(r>0)
	{
		setCh(addr,1);
	}
	else if(g>0)
	{
		setCh(addr,20);
	}
	else if(b>0)
	{
		setCh(addr,40);
	}

	setCh(addr+1,rotation+128);


	setCh(addr+2,0); //strobe

	if((r>0)||(b>0)||(g>0))
	{
		setCh(addr+3,255); //enable
	}
	else
	{
		setCh(addr+3,0); //enable
	}
	
}

void set_fog(uint16_t addr  __attribute__((__unused__)), uint8_t state  __attribute__((__unused__)))
{
}

