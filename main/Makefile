UNAME := $(shell uname)

COMPILER = gcc


FLAGS= -I. -I../firmware --std=gnu99 -Wall  -funsigned-char -Wundef -Wsign-compare  -Wstrict-prototypes  -fsingle-precision-constant -Wextra -L. -lftd2xx -Wl,-rpath /usr/local/lib
LDFLAGS=
BINEXT=

ifeq ($(UNAME), Darwin)
	COMPILER = clang
	FLAGS   += -Qunused-arguments

endif

ifeq ($(UNAME), Linux)
endif

ifeq ($(UNAME), MINGW32_NT-6.1)
	FLAGS   += -I/MinGW/msys/1.0/include/
	LDFLAGS += -L/MinGW/msys/1.0/lib -lmingw32 
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
	@$(COMPILER) main.c $(LDFLAGS) -o dmxMidiCtrl -I. $(FLAGS) $(LIB_SOURCES) $(SOURCES)   

.PHONY : clean all

