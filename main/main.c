#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

#include "main.h"
#include <string.h>
#include<sys/time.h>

#include <termios.h>
#include <fcntl.h>
#include <errno.h>
//#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
#include <sys/ioctl.h>
#include <IOKit/serial/ioss.h>
//#endif

static int serial_bridge;
static unsigned char ch[255];

static int running = 1;

void intHandler(int dummy) 
{
	close(serial_bridge);
	running = 0;
	printf("exiting\n");
}

void init_serial() {

	serial_bridge = open("/dev/cu.usbserial-A602SRIU", O_RDWR);

	struct termios config;
	memset(&config, 0, sizeof(config));
	tcgetattr(serial_bridge, &config);

	config.c_iflag = 0;
	config.c_oflag = 0;
	config.c_lflag = 0;
	config.c_cc[VMIN] = 1;
	config.c_cc[VTIME] = 5;

	cfsetospeed(&config, B115200);
	cfsetispeed(&config, B115200);
	config.c_cflag = CS8 | CREAD | CLOCAL | CSTOPB;
	tcsetattr(serial_bridge, TCSANOW, &config);



	speed_t speed = 9600;
	if ( ioctl( serial_bridge,  IOSSIOSPEED, &speed ) == -1 )
	{
		printf( "Error %d calling ioctl( ..., IOSSIOSPEED, ... )\n", errno );
	}

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


void Delay(uint16_t t)
{

}

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


	srand(time(NULL));

	init_serial();
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
			speed_t speed = 38400;
			if ( running && ioctl( serial_bridge,  IOSSIOSPEED, &speed ) == -1 )
			{
				printf( "Error %d calling ioctl( ..., IOSSIOSPEED, ... )\n", errno );
			}
			usleep(1200);

			unsigned char c=0;
			write(serial_bridge,&c,1);
			tcdrain(serial_bridge);
			usleep(200);

			speed = 250000;
			if ( running && ioctl( serial_bridge,  IOSSIOSPEED, &speed ) == -1 )
			{
				printf( "Error %d calling ioctl( ..., IOSSIOSPEED, ... )\n", errno );
			}
			usleep(200);




			if(running) write(serial_bridge,ch,100);
			tcdrain(serial_bridge);

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

	return 0;
}

