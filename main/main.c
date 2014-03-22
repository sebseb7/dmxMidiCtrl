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
#else
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

void rgb2hsv(int r, int g, int b, double *hr, double *sr, double *vr)
{
	double rd, gd, bd, h, s, v, max, min, del, rc, gc, bc;

	/* convert RGB to HSV */
	rd = r / 127.0;            /* rd,gd,bd range 0-1 instead of 0-255 */
	gd = g / 127.0;
	bd = b / 127.0;

	/* compute maximum of rd,gd,bd */
	if (rd>=gd) { if (rd>=bd) max = rd;  else max = bd; }
	else { if (gd>=bd) max = gd;  else max = bd; }

	/* compute minimum of rd,gd,bd */
	if (rd<=gd) { if (rd<=bd) min = rd;  else min = bd; }
	else { if (gd<=bd) min = gd;  else min = bd; }

	del = max - min;
	v = max;
	if (max != 0.0) s = (del) / max;
	else s = 0.0;

	h = 0;
	if (s != 0.0) {
		rc = (max - rd) / del;
		gc = (max - gd) / del;
		bc = (max - bd) / del;

		if      (rd==max) h = bc - gc;
		else if (gd==max) h = 2 + rc - bc;
		else if (bd==max) h = 4 + gc - rc;

		h = h * 60;
		if (h<0) h += 360;
	}

	//printf("HSV %f %f %f\n",h,s,v);

	*hr = h;  *sr = s;  *vr = v;
}


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
		
//		if(strstr(di->name, "nanoKONTROL") && di->input)
	
//	MidiObj midi_korg;
#ifdef KTRL_F1
	MidiObj midi_f1;
	keyboard_init(&midi_f1,"Traktor Kontrol F1");
#endif
#ifdef TOUCH_OSC
	MidiObj midi_touch;
	keyboard_init(&midi_touch,"TouchOSC Bridge");
#endif


//	keyboard_init(midi_korg);

	for(uint16_t i = 0; i < 512;i++)
	{
		ch[i]=0;
	}

	toggle[0]=1;			
	toggle[1]=1;			
	toggle[2]=1;			
	toggle[3]=1;			
	toggle[4]=1;			
#ifdef KTRL_F1
	keyboard_send(&midi_f1,188,37,toggle[0]*127);
	keyboard_send(&midi_f1,188,38,toggle[1]*127);
	keyboard_send(&midi_f1,188,39,toggle[2]*127);
	keyboard_send(&midi_f1,188,40,toggle[3]*127);
	keyboard_send(&midi_f1,188,12,toggle[4]*127);
#else
	keyboard_send(&midi_korg,176,43,toggle[0]*127);
	keyboard_send(&midi_korg,176,44,toggle[1]*127);
	keyboard_send(&midi_korg,176,42,toggle[2]*127);
	keyboard_send(&midi_korg,176,41,toggle[3]*127);
	keyboard_send(&midi_korg,176,45,toggle[4]*127);
#endif
#ifdef TOUCH_OSC
	keyboard_send(&midi_touch,176,43,toggle[0]*127);
	keyboard_send(&midi_touch,176,44,toggle[1]*127);
	keyboard_send(&midi_touch,176,42,toggle[2]*127);
	keyboard_send(&midi_touch,176,41,toggle[3]*127);
	keyboard_send(&midi_touch,176,45,toggle[4]*127);
#endif
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
#else

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

#ifdef KTRL_F1
	for(uint8_t i = 0;i <= 16;i++)
	{
		keyboard_send(&midi_f1,176,i+20,1); // HUE
		keyboard_send(&midi_f1,177,i+20,127); // SAT
		keyboard_send(&midi_f1,178,i+20,0); // BRI
	}
	keyboard_send(&midi_f1,176,20,1);
	keyboard_send(&midi_f1,177,20,127);
	keyboard_send(&midi_f1,178,20,127);
#elif
	for(uint8_t i = 32;i <= 39;i++)
	{
		keyboard_send(&midi_korg,176,i,0);
		keyboard_send(&midi_korg,176,i+16,0);
		keyboard_send(&midi_korg,176,i+32,0);
	}
	keyboard_send(&midi_korg,176,32,127);
#endif
	srand(time(NULL));

	signal(SIGINT, intHandler);

	int current_animation = 0;
	int new_animation = 0;

	poti[0] = 127;
	poti[1] = 127;


	ch[0]=0;

	animations[current_animation].init_fp();

	int tick_count = 0;

	unsigned long long last_frame;

	struct timeval tv;

	while(running) {



		KeyboardEvent e;
#ifdef TOUCH_OSC
		while(keyboard_poll(&midi_touch,&e)) 
		{
			if((e.type == 176)&&(e.x==43)&&(e.y==127))
			{
				toggle[0] ^= 1;
				keyboard_send(&midi_f1,188,12,toggle[0]*127);
				//keyboard_send(&midi_korg,176,43,toggle[0]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,43,toggle[0]*127);
#endif
			}
			if((e.type == 176)&&(e.x==44)&&(e.y==127))
			{
				toggle[1] ^= 1;
				keyboard_send(&midi_f1,188,37,toggle[1]*127);
				//keyboard_send(&midi_korg,176,44,toggle[1]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,44,toggle[1]*127);
#endif
			}
			if((e.type == 176)&&(e.x==42)&&(e.y==127))
			{
				toggle[2] ^= 1;
				keyboard_send(&midi_f1,188,38,toggle[2]*127);
				//keyboard_send(&midi_korg,176,42,toggle[2]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,42,toggle[2]*127);
#endif
			}
			if((e.type == 176)&&(e.x==41)&&(e.y==127))
			{
				toggle[3] ^= 1;
				keyboard_send(&midi_f1,188,39,toggle[3]*127);
				//keyboard_send(&midi_korg,176,41,toggle[3]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,41,toggle[3]*127);
#endif
			}
			if((e.type == 176)&&(e.x==45)&&(e.y==127))
			{
				toggle[4] ^= 1;
				keyboard_send(&midi_f1,188,40,toggle[4]*127);
				//keyboard_send(&midi_korg,176,45,toggle[4]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,45,toggle[4]*127);
#endif
			}
			
			//printf("%d %d %d\n", e.x, e.y, e.type);
		}
#endif
#ifdef KTRL_F1
		while(keyboard_poll(&midi_f1,&e)) 
		{
			if((e.type == 188)&&(e.x>=2)&&(e.x<6))
			{
				poti[e.x-2] = e.y;
		
			}
			if((e.type == 188)&&(e.y == 127)&&(e.x>=20)&&(e.x<36)&&(e.x < 20+animationcount))
			{
				new_animation = e.x-20;
			}
			if((e.type == 188)&&(e.x==6))
			{
				ch[128]=e.y;
				double h,s,v;
				rgb2hsv(ch[128], ch[129], ch[130], &h,&s,&v);
				keyboard_send(&midi_f1,176,35,h/360.0f*127.0f);
				keyboard_send(&midi_f1,177,35,s*127.0f);
				keyboard_send(&midi_f1,178,35,v*127.0f);
			}
			if((e.type == 188)&&(e.x==7))
			{
				ch[129]=e.y;
				double h,s,v;
				rgb2hsv(ch[128], ch[129], ch[130], &h,&s,&v);
				keyboard_send(&midi_f1,176,35,h/360.0f*127.0f);
				keyboard_send(&midi_f1,177,35,s*127.0f);
				keyboard_send(&midi_f1,178,35,v*127.0f);
			}
			if((e.type == 188)&&(e.x==8))
			{
				ch[130]=e.y;
				double h,s,v;
				rgb2hsv(ch[128], ch[129], ch[130], &h,&s,&v);
				keyboard_send(&midi_f1,176,35,h/360.0f*127.0f);
				keyboard_send(&midi_f1,177,35,s*127.0f);
				keyboard_send(&midi_f1,178,35,v*127.0f);
			}
			if((e.type == 188)&&(e.x==47))
			{
				ch[134]=e.y;
			}
			if((e.type == 188)&&(e.x==48))
			{
				ch[135]=e.y;
			}
			if((e.type == 188)&&(e.x==49))
			{
				ch[136]=e.y;
			}

			if((e.type == 188)&&(e.x==12)&&(e.y==127))
			{
				toggle[0] ^= 1;
				keyboard_send(&midi_f1,188,12,toggle[0]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,43,toggle[0]*127);
#endif
			}
			if((e.type == 188)&&(e.x==37)&&(e.y==127))
			{
				toggle[1] ^= 1;
				keyboard_send(&midi_f1,188,37,toggle[1]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,44,toggle[1]*127);
#endif
			}
			if((e.type == 188)&&(e.x==38)&&(e.y==127))
			{
				toggle[2] ^= 1;
				keyboard_send(&midi_f1,188,38,toggle[2]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,42,toggle[2]*127);
#endif
			}
			if((e.type == 188)&&(e.x==39)&&(e.y==127))
			{
				toggle[3] ^= 1;
				keyboard_send(&midi_f1,188,39,toggle[3]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,41,toggle[3]*127);
#endif
			}
			if((e.type == 188)&&(e.x==40)&&(e.y==127))
			{
				toggle[4] ^= 1;
				keyboard_send(&midi_f1,188,40,toggle[4]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,45,toggle[4]*127);
#endif
			}


			printf("%d %d %d\n", e.x, e.y, e.type);
		}
#else
		while(keyboard_poll(&midi_korg,&e)) 
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
				keyboard_send(&midi_korg,176,43,toggle[0]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,43,toggle[0]*127);
#endif
			}
			if((e.type == 176)&&(e.x==44)&&(e.y==127))
			{
				toggle[1] ^= 1;
				keyboard_send(&midi_korg,176,44,toggle[1]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,44,toggle[1]*127);
#endif
			}
			if((e.type == 176)&&(e.x==42)&&(e.y==127))
			{
				toggle[2] ^= 1;
				keyboard_send(&midi_korg,176,42,toggle[2]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,42,toggle[2]*127);
#endif
			}
			if((e.type == 176)&&(e.x==41)&&(e.y==127))
			{
				toggle[3] ^= 1;
				keyboard_send(&midi_korg,176,41,toggle[3]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,41,toggle[3]*127);
#endif
			}
			if((e.type == 176)&&(e.x==45)&&(e.y==127))
			{
				toggle[4] ^= 1;
				keyboard_send(&midi_korg,176,45,toggle[4]*127);
#ifdef TOUCH_OSC
				keyboard_send(&midi_touch,176,45,toggle[4]*127);
#endif
			}
		}
#endif


		unsigned long long current_time;
		int32_t time_diff;
		gettimeofday(&tv,NULL);
		current_time = tv.tv_usec;
		time_diff = current_time - last_frame;

		if(time_diff < 0 )
		{
			time_diff+=1000000;
		}

		if((uint32_t)time_diff > animations[current_animation].timing)
		{
			time_diff -= animations[current_animation].timing;
			animations[current_animation].tick_fp();
			tick_count++;
			gettimeofday(&tv,NULL);
			last_frame = tv.tv_usec - time_diff;
		}



		if(toggle[3])
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

		if(toggle[4] == 0)
		{
			ch[28] = ch[234]*poti[0]*2.0f;
			ch[29] = ch[235]*poti[0]*2.0f;
			ch[30] = ch[236]*poti[0]*2.0f;
		}else{
			ch[28] = ch[128]*2;
			ch[29] = ch[129]*2;
			ch[30] = ch[130]*2;
		}
		if(toggle[2]==0)
		{
			ch[16]=0;
			ch[17]=0;
			ch[18]=0;
			ch[22]=0;
			ch[23]=0;
			ch[24]=0;
		}
		if(toggle[1]==0)
		{
			ch[2]=0;
			ch[6]=0;
			ch[4]=0;
			ch[8]=0;
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
#else
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


		if((((tick_count >= animations[current_animation].duration)&& toggle[0]))||(new_animation != current_animation))
		{
			animations[current_animation].deinit_fp();

#ifdef KTRL_F1
			keyboard_send(&midi_f1,176,20+(current_animation),0);
			keyboard_send(&midi_f1,177,20+(current_animation),0);
			keyboard_send(&midi_f1,178,20+(current_animation),0);
#else
			if(current_animation < 8)
			{
				keyboard_send(&midi_korg,176,32+current_animation,0);
			}
			else if(current_animation < 16)
			{
				keyboard_send(&midi_korg,176,48+(current_animation-8),0);
			}
#endif

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
#ifdef KTRL_F1
			keyboard_send(&midi_f1,176,20+(current_animation),1);
			keyboard_send(&midi_f1,177,20+(current_animation),127);
			keyboard_send(&midi_f1,178,20+(current_animation),127);
#else
			if(current_animation < 8)
			{
				keyboard_send(&midi_korg,176,32+current_animation,127);
			}
			else if(current_animation < 16)
			{
				keyboard_send(&midi_korg,176,48+(current_animation-8),127);
			}
#endif
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
#else
	FT_Close(ftHandle); 
#endif

	return 0;
}

