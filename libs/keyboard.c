//	keyboard.cpp

#include	<stdio.h>
#include	<string.h>
#include	"keyboard.h"
#include	"main.h"

enum {
	KEYBOARD_MAX_EVENTS = 256,
};


static int initialized = 0;

static int find_device_id_in(char* name)
{
	const PmDeviceInfo *di;
	for(int i= 0; ; i++)
	{
		di= Pm_GetDeviceInfo(i);
		if(!di) break;
		if(strstr(di->name, name) && di->input)
		{
			printf("found in device '%s' with interf '%s'\n", di->name, di->interf);
			return i;
		}
	}
	return -1;
}

static int find_device_id_out(char* name)
{
	const PmDeviceInfo *di;
	for(int i= 0; ; i++)
	{
		di= Pm_GetDeviceInfo(i);
		if(!di) break;
		if(strstr(di->name, name) && di->output)
		{
			printf("found out device '%s' with interf '%s'\n", di->name, di->interf);
			return i;
		}
	}
	return -1;
}

int keyboard_init(MidiObj* m,char* name) {
		
	m->midi_stream_out = NULL;
	m->midi_stream = NULL;
	
	// open midi device
	if(! initialized)
	{
		Pm_Initialize();
		initialized=1;
	}

	int devid= find_device_id_in(name);
	int devid_out= find_device_id_out(name);
	const PmDeviceInfo* dev_info = Pm_GetDeviceInfo(devid);
	const PmDeviceInfo* dev_info_out = Pm_GetDeviceInfo(devid_out);

	if(dev_info_out) {
		Pm_OpenOutput(&(m->midi_stream_out), devid_out, NULL, KEYBOARD_MAX_EVENTS, NULL, NULL,0);
	}
	if(dev_info) {
		Pm_OpenInput(&(m->midi_stream), devid, NULL, KEYBOARD_MAX_EVENTS, NULL, NULL);
		return 1;
	}
	return 0;
}

void keyboard_kill(MidiObj* m) {
	if(m->midi_stream_out) {
		Pm_Close(m->midi_stream_out);
		m->midi_stream_out = NULL;
	}
	if(m->midi_stream) {
		Pm_Close(m->midi_stream);
		m->midi_stream = NULL;
	}
	//Pm_Terminate();
}

int keyboard_send(MidiObj* m,uint8_t a,uint8_t b , uint8_t c) {

	if(!m->midi_stream_out) return 0;

	static PmEvent events[KEYBOARD_MAX_EVENTS];

	events[0].timestamp =  0;
	events[0].message = Pm_Message( a,b,c );

	int i = Pm_Write(m->midi_stream_out, events, 1);

	return i;
}


int keyboard_poll(MidiObj* m,KeyboardEvent* e) {

	if(!m->midi_stream) return 0;

	static PmEvent events[KEYBOARD_MAX_EVENTS];
	static int pos = 0;
	static int len = 0;

	if(pos == len) {
		pos = 0;
		len = Pm_Read(m->midi_stream, events, KEYBOARD_MAX_EVENTS);
		if(!len) return 0;
	}


	*e = *(KeyboardEvent*) &events[pos++].message;

	return 1;
}

