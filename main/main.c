#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#include "main.h"
#include <string.h>
#include<sys/time.h>

#ifdef LIBFTDI
	#include "libftdi1/ftdi.h"
#elif
	#include "ftd2xx.h"
#endif
#include "keyboard.h"

static uint8_t ch[512];
static uint8_t toggle[5];
static uint8_t in[512];
static uint8_t poti[8];

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

	keyboard_init();

	for(uint16_t i = 0; i < 512;i++)
	{
		ch[i]=0;
	}
	
	for(uint16_t i = 0; i < 5;i++)
	{
		toggle[i]=0;
	}
	toggle[0]=1;			
	keyboard_send(176,43,toggle[0]*127);
	keyboard_send(176,44,toggle[1]*127);
	keyboard_send(176,42,toggle[2]*127);
	keyboard_send(176,41,toggle[3]*127);
	keyboard_send(176,45,toggle[4]*127);

//	FILE *input;

/*	input = popen ("../traktorMidiClock/traktorMidiClock", "r");
	if (!input)
	{
		fprintf (stderr,
				"incorrect parameters or too many files.\n");
		return EXIT_FAILURE;
	}
*/

#ifdef LIBFTDI

	int ret;
	struct ftdi_context *ftdi;
	struct ftdi_version_info version;
	if ((ftdi = ftdi_new()) == 0)
	{
		fprintf(stderr, "ftdi_new failed\n");
		return EXIT_FAILURE;
	}
	version = ftdi_get_library_version();
	printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
			version.version_str, version.major, version.minor, version.micro,
			version.snapshot_str);
	if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6001)) < 0)
	{
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		ftdi_free(ftdi);
		return EXIT_FAILURE;
	}
	// Read out FTDIChip-ID of R type chips
	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;
		printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
		printf("FTDI chipid: %X\n", chipid);
	}
	ret = ftdi_set_line_property(ftdi, 8, STOP_BIT_2, NONE);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set line parameters: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
	ret = ftdi_set_baudrate(ftdi, 250000);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set baudrate: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
#elif

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

#endif

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

	while(running) {

		gettimeofday(&tv,NULL);
		t1 = tv.tv_usec;


		KeyboardEvent e;
		while(keyboard_poll(&e)) 
		{
			if((e.type == 176)&&(e.x>=16)&&(e.x<24))
			{
				poti[e.x-16] = e.y;
			}
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
			if((e.type == 176)&&(e.x==0))
			{
				ch[128]=e.y;
			}
			if((e.type == 176)&&(e.x==1))
			{
				ch[129]=e.y;
			}
			if((e.type == 176)&&(e.x==2))
			{
				ch[130]=e.y;
			}
			if((e.type == 176)&&(e.x==3))
			{
				ch[134]=e.y;
			}
			if((e.type == 176)&&(e.x==4))
			{
				ch[135]=e.y;
			}
			if((e.type == 176)&&(e.x==5))
			{
				ch[136]=e.y;
			}

			if((e.type == 176)&&(e.x==43)&&(e.y==127))
			{
				toggle[0] ^= 1;
				keyboard_send(176,43,toggle[0]*127);
			}
			if((e.type == 176)&&(e.x==44)&&(e.y==127))
			{
				toggle[1] ^= 1;
				keyboard_send(176,44,toggle[1]*127);
			}
			if((e.type == 176)&&(e.x==42)&&(e.y==127))
			{
				toggle[2] ^= 1;
				keyboard_send(176,42,toggle[2]*127);
			}
			if((e.type == 176)&&(e.x==41)&&(e.y==127))
			{
				toggle[3] ^= 1;
				keyboard_send(176,41,toggle[3]*127);
			}
			if((e.type == 176)&&(e.x==45)&&(e.y==127))
			{
				toggle[4] ^= 1;
				keyboard_send(176,45,toggle[4]*127);
			}


			//printf("%d %d %d\n", e.x, e.y, e.type);
		}
		//setIn(0,e.y);

		animations[current_animation].tick_fp();
		tick_count++;



		if(toggle[4])
		{
			ch[34] = ch[134]*2;
			ch[35] = ch[135]*2;
			ch[36] = ch[136]*2;
		}
		else
		{
			ch[34] = ch[234]*poti[1]*2.0f;
			ch[35] = ch[235]*poti[1]*2.0f;
			ch[36] = ch[236]*poti[1]*2.0f;
		}

		if(toggle[3] == 0)
		{
			ch[28] = ch[234]*poti[0]*2.0f;
			ch[29] = ch[235]*poti[0]*2.0f;
			ch[30] = ch[236]*poti[0]*2.0f;
		}else{
			ch[28] = ch[128]*2;
			ch[29] = ch[129]*2;
			ch[30] = ch[130]*2;
		}


#ifdef LIBFTDI
		ret = ftdi_set_line_property2(ftdi, 8, STOP_BIT_2, NONE,BREAK_ON);
		if (ret < 0)
		{
			fprintf(stderr, "unable to set line parameters: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
			exit(-1);
		}
		usleep(100);
		ret = ftdi_set_line_property2(ftdi, 8, STOP_BIT_2, NONE,BREAK_OFF);
		if (ret < 0)
		{
			fprintf(stderr, "unable to set line parameters: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
			exit(-1);
		}
		unsigned char c=0;

		ret = ftdi_write_data(ftdi, &c, 0);
		if (ret < 0)
		{
			fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
		}
		usleep(10);

		ret = ftdi_write_data(ftdi, ch, 39);
		if (ret < 0)
		{
			fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
		}
		usleep(2000);
#elif
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

		/*
		   for(uint16_t i = 1;i < 40 ; i++)
		   {
		   printf("%3i ",ch[i]);

		   if(i == 4)
		   printf("| ");
		   if(i == 8)
		   printf("| ");
		   if(i == 15)
		   printf("| ");
		   if(i == 21)
		   printf("| ");
		   if(i == 27)
		   printf("| ");
		   if(i == 33)
		   printf("| ");
		   }
		   printf("\n");
		   */

		if((ftStatus = FT_Write(ftHandle, ch, 39, &BytesWritten)) != FT_OK) {
			printf("Error FT_Write\n");
			return 1;
		}
		usleep(2000);

#endif
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






		if((((tick_count >= animations[current_animation].duration)&& toggle[0]))||(new_animation != current_animation))
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
#ifdef LIBFTDI
	if ((ret = ftdi_usb_close(ftdi)) < 0)
	{
		fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		ftdi_free(ftdi);
		return EXIT_FAILURE;
	}
	ftdi_free(ftdi);
#elif
	FT_Close(ftHandle); 
#endif

	return 0;
}

