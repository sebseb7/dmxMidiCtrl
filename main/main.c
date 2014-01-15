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
#define BUF_SIZE 0x10
#define MAX_DEVICES		5

static unsigned char ch[255];

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



	if((ftStatus = FT_SetDataCharacteristics(ftHandle, 
					FT_BITS_8,
					FT_STOP_BITS_2,
					FT_PARITY_NONE)) != FT_OK) {
		printf("Error FT_SetDataCharacteristics)\n");
		return 1;
	}



	srand(time(NULL));

	signal(SIGINT, intHandler);
	char l  = 0;

	int current_animation = 0;



	ch[0]=0;

	animations[current_animation].init_fp();

	int tick_count = 0;

	unsigned long long t1, t2;

	struct timeval tv;

	while(running) {

		gettimeofday(&tv,NULL);
		t1 = tv.tv_usec;

		animations[current_animation].tick_fp();

		l++;	
		usleep(1000);

		if((ftStatus = FT_SetBaudRate(ftHandle, 38400)) != FT_OK) {
			printf("Error FT_SetBaudRate(%d)\n", (int)ftStatus);
			return 1;
		}
		usleep(1200);

		unsigned char c=0;

		if((ftStatus = FT_Write(ftHandle, &c, 1, &BytesWritten)) != FT_OK) {
			printf("Error FT_Write\n");
			return 1;
		}


		if((ftStatus = FT_SetBaudRate(ftHandle, 250000)) != FT_OK) {
			printf("Error FT_SetBaudRate(%d)\n", (int)ftStatus);
			return 1;
		}

		usleep(200);


		if((ftStatus = FT_Write(ftHandle, ch, 100, &BytesWritten)) != FT_OK) {
			printf("Error FT_Write\n");
			return 1;
		}

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


		tick_count++;


		if(tick_count == animations[current_animation].duration)
		{
			animations[current_animation].deinit_fp();

			current_animation++;
			if(current_animation == animationcount)
			{
				current_animation = 0;
			}
			tick_count=0;

			animations[current_animation].init_fp();


		}
	}
	printf("exiting\n");
	FT_Close(ftHandle); 

	return 0;
}

