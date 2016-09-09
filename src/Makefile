# This file is Makefile (part of XIcon)
#

CC=gcc

# If your compiler defines BYTE, SHORT, etc. by default, try -DSKIPTYPES
# If you're trying to debug, use -g instead of -O2
CFLAGS=-O2

# OS/2
EMXBIND=emxbind xicon -s16384 -p

# Unix
#EMXBIND=echo emxbind unnecessary

OBJFILES=xicon.o bitmap.o generic.o iconio.o macread.o \
         os2read.o os2write.o palette.o xbmread.o readxbm.o \
         winread.o winwrite.o xpm3write.o

all: xicon

xicon: $(OBJFILES)
	$(CC) $(CFLAGS) -o xicon $(OBJFILES)
	$(EMXBIND)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

xicon.o: xicon.c macread.h bitmap.h icondata.h iconio.h os2write.h os2read.h

xbmread.o: xbmread.c xbmread.h iconio.h icondata.h iconvars.h generic.h bitmap.h palette.h

xpm3write.o: xpm3write.c xpm3write.h iconio.h icondata.h iconvars.h

readxbm.o: readxbm.c iconio.h

bitmap.o: bitmap.c icondata.h

generic.o: generic.c iconvars.h icondata.h

iconio.o: iconio.c icondata.h macdata.h macread.h

macread.o: macread.c macdata.h iconio.h iconvars.h generic.h palette.h

winread.o: winread.c windata.h iconio.h iconvars.h generic.h palette.h

winwrite.o: winwrite.c windata.h winwrite.h

os2read.o: os2read.c icondata.h iconvars.h os2data.h iconio.h

os2write.o: os2write.c icondata.h iconvars.h iconio.h os2data.h os2write.h

palette.o: palette.c palette.h

clean: 
	rm -f xicon xicon.exe *.o



