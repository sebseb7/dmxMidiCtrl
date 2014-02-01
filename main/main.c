#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#include "main.h"
#include <string.h>
#include<sys/time.h>

#include "ftd2xx.h"
#include "keyboard.h"

static unsigned char ch[512];
static unsigned char in[512];

static int running = 1;

void intHandler(int dummy) 
{
	running = 0*dummy;
}

#define MAX_ANIMATIONS 200

int animationcount = 0;

struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	uint16_t type;
	int duration;
	uint32_t timing;
	uint32_t idle;
} animations[MAX_ANIMATIONS];


void setCh(uint8_t chan, uint8_t value)
{
	ch[chan] = value;
}

void setIn(uint8_t chan, uint8_t value)
{
	in[chan] = value;
}
uint8_t getIn(uint8_t chan)
{
	return in[chan];
}

void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t type,uint16_t t, uint16_t count, uint8_t idle)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = t*count;
	animations[animationcount].type = type;
	animations[animationcount].idle = idle;
	animations[animationcount].timing = 1000000/t;

	animationcount++;

}


int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {


//	FILE *input;
	keyboard_init();

/*	input = popen ("../traktorMidiClock/traktorMidiClock", "r");
	if (!input)
	{
		fprintf (stderr,
				"incorrect parameters or too many files.\n");
		return EXIT_FAILURE;
	}
*/

	FT_HANDLE ftHandle; 

	FT_STATUS ftStatus; 

	DWORD BytesWritten; 


	printf("start\n");


	ftStatus = FT_Open(0, &ftHandle); 

	if(ftStatus != FT_OK) { 

		// FT_Open failed 

		printf("open failed\n");
		//return; 

	} 
	printf("open ok\n");

	if((ftStatus = FT_SetBaudRate(ftHandle, 250000)) != FT_OK) {
		printf("Error FT_SetBaudRate(%d)\n", (int)ftStatus);
		return 1;
	}


	if((ftStatus = FT_SetDataCharacteristics(ftHandle, 
					FT_BITS_8,
					FT_STOP_BITS_2,
					FT_PARITY_NONE)) != FT_OK) {
		printf("Error FT_SetDataCharacteristics)\n");
		return 1;
	}



	for(uint8_t i = 32;i <= 39;i++)
	{
		keyboard_send(176,i,0);
		keyboard_send(176,i+16,0);
		keyboard_send(176,i+32,0);
	}
	keyboard_send(176,32,127);

	srand(time(NULL));

	signal(SIGINT, intHandler);

	int current_animation = 0;
	int new_animation = 0;



	ch[0]=0;

	animations[current_animation].init_fp();

	int tick_count = 0;

	unsigned long long t1, t2;

	struct timeval tv;

	uint16_t beat = 0;

	while(running) {

		gettimeofday(&tv,NULL);
		t1 = tv.tv_usec;


		KeyboardEvent e;
		while(keyboard_poll(&e)) 
		{
			if((e.type == 176)&&(e.y == 127)&&(e.x>=32)&&(e.x<40)&&(e.x < 32+animationcount))
			{
				new_animation = e.x-32;
			}
			if((e.type == 176)&&(e.y == 127)&&(e.x>=48)&&(e.x<56)&&(e.x < 48+animationcount-8))
			{
				new_animation = e.x-48+8;
			}
			if((e.type == 176)&&(e.y == 127)&&(e.x>=64)&&(e.x<72)&&(e.x < 64+animationcount-16))
			{
				new_animation = e.x-64+16;
			}
			printf("%d %d %d\n", e.x, e.y, e.type);
		}
		//setIn(0,e.y);

		animations[current_animation].tick_fp();
		tick_count++;
		beat = 0;


		if((ftStatus = FT_SetBreakOn(ftHandle)) != FT_OK) {
			printf("Error FT_SetBreakon\n");
			return 1;
		}
		usleep(100);
		if((ftStatus = FT_SetBreakOff(ftHandle)) != FT_OK) {
			printf("Error FT_SetBreakOff\n");
			return 1;
		}
		unsigned char c=0;

		if((ftStatus = FT_Write(ftHandle, &c, 0, &BytesWritten)) != FT_OK) {
			printf("Error FT_Write\n");
			return 1;
		}
		usleep(10);

		if((ftStatus = FT_Write(ftHandle, ch, 32, &BytesWritten)) != FT_OK) {
			printf("Error FT_Write\n");
			return 1;
		}
		usleep(2000);


		gettimeofday(&tv,NULL);
		t2 = tv.tv_usec;

		int32_t diff = t2-t1;
		if(diff < 0 )
		{
			diff+=1000000;
		}
		diff = animations[current_animation].timing-diff;

		if(diff > 0)
		{
			usleep(diff);
		}






		if((tick_count == animations[current_animation].duration)||(new_animation != current_animation))
		{
			animations[current_animation].deinit_fp();

			if(current_animation < 8)
			{
				keyboard_send(176,32+current_animation,0);
			}
			else if(current_animation < 16)
			{
				keyboard_send(176,48+(current_animation-8),0);
			}

			if(new_animation != current_animation)
			{
				current_animation= new_animation;
			}
			else
			{
				current_animation++;
				if(current_animation == animationcount)
				{
					current_animation = 0;
				}
				new_animation= current_animation;
			}
			if(current_animation < 8)
			{
				keyboard_send(176,32+current_animation,127);
			}
			else if(current_animation < 16)
			{
				keyboard_send(176,48+(current_animation-8),127);
			}
			tick_count=0;

			animations[current_animation].init_fp();


		}
	}
	printf("exiting\n");
	FT_Close(ftHandle); 

	return 0;
}

