TARGET = FontLoader
OBJS = main.o ../../fontloader.o graphics.o framebuffer.o

INCDIR =
CFLAGS = -G0 -Wall -O2
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS =
LIBS= -lpspgu -lpng -lz -lm -lfreetype

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Font Loader Test

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
