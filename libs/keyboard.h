#ifndef _KEYBOARD_H
#define _KEYBOARD_H
//	keyboard.h

#include	<portmidi.h>

typedef struct {
	unsigned char type, x, y;
} KeyboardEvent;

typedef struct
{
	PortMidiStream* midi_stream ;
	PortMidiStream* midi_stream_out ;
} MidiObj;

int		keyboard_init(MidiObj* m,char* name);
void	keyboard_kill(MidiObj* m);
int		keyboard_poll(MidiObj* m,KeyboardEvent* e);
int		keyboard_send(MidiObj* m,uint8_t a,uint8_t b, uint8_t c);


#endif
