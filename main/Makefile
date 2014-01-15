UNAME := $(shell uname)

COMPILER = gcc


FLAGS= -I. -I../firmware --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes  -fsingle-precision-constant
LDFLAGS=
BINEXT=

ifeq ($(UNAME), Darwin)
	COMPILER = clang
	FLAGS   += -Qunused-arguments

	FLAGS   += `sdl-config --cflags`
	LDFLAGS += `sdl-config --libs`
endif

ifeq ($(UNAME), Linux)
	FLAGS   += `sdl-config --cflags`
	LDFLAGS += `sdl-config --libs`
endif

ifeq ($(UNAME), MINGW32_NT-6.1)
	FLAGS   += -I/MinGW/msys/1.0/include/
	LDFLAGS += -L/MinGW/msys/1.0/lib -lmingw32 -lSDLmain -lSDL
	BINEXT = .exe
endif

SOURCES=$(wildcard ../animations/*.c)

LIB_SOURCES=$(wildcard ../libs/math.c ../libs/hsv2rgb.c)
LIB_HEADERS=$(wildcard ../libs/math.h ../libs/hsv2rgb.h)

all: dmxMidiCtrl

clean:
	rm -f dmxMidiCtrl

dmxMidiCtrl: $(SOURCES) $(LIB_SOURCES) $(LIB_HEADERS) main.c main.h Makefile 
	@echo -e "  \033[1;34mCompile\033[0m"
	@$(COMPILER) $(FLAGS) $(LIB_SOURCES) $(SOURCES) main.c $(LDFLAGS) -o dmxMidiCtrl -I. 

.PHONY : clean all

