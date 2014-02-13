UNAME := $(shell uname)

COMPILER = gcc


FLAGS= -I. -I../firmware --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes  -Wextra -L. -lportmidi 
FLAGS+= -DLIBFTDI=1 -lftdi1 -Wl,-rpath /usr/local/lib
#FLAGS+= -lftd2xx -Wl,-rpath /usr/local/lib
LDFLAGS=

SOURCES=$(wildcard ../animations/*.c)

LIB_SOURCES=$(wildcard ../libs/*.c)
LIB_HEADERS=$(wildcard ../libs/*.h)

all: dmxMidiCtrl

clean:
	rm -f dmxMidiCtrl

dmxMidiCtrl: $(SOURCES) $(LIB_SOURCES) $(LIB_HEADERS) main.c main.h Makefile 
	@echo "  \033[1;34mCompile\033[0m"
	@$(COMPILER) main.c $(LDFLAGS) -o dmxMidiCtrl -I. -I../libs $(FLAGS) $(LIB_SOURCES) $(SOURCES)   

.PHONY : clean all

