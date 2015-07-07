UNAME := $(shell uname)

COMPILER = gcc

SDL_OUTPUT = false


FLAGS= -I. -I../firmware --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes  -Wextra -L. 
LDFLAGS= -lportmidi -lftdi1 

SOURCES=$(wildcard ../animations/*.c)

LIB_SOURCES=$(wildcard ../libs/*.c)
LIB_HEADERS=$(wildcard ../libs/*.h)

ifeq ($(SDL_OUTPUT), true)

	FLAGS   += `sdl-config --cflags` -DSDL_OUTPUT
	LDFLAGS += `sdl-config --libs`
endif

all: dmxMidiCtrl

clean:
	rm -f dmxMidiCtrl

dmxMidiCtrl: $(SOURCES) $(LIB_SOURCES) $(LIB_HEADERS) main.c main.h Makefile 
	@echo "  \033[1;34mCompile\033[0m"
	@$(COMPILER) main.c  -o dmxMidiCtrl -I. -I../libs $(FLAGS) $(LIB_SOURCES) $(SOURCES) $(LDFLAGS) 

.PHONY : clean all

