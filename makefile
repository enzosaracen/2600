CC	= gcc
CFLAGS	= -Wall -Wextra -ggdb3 $(shell sdl-config --cflags --libs)
OBJ	= src/cpu.o \
	  src/cmd.o \
	  src/tia.o \
	  src/util.o

HFILES	= src/2600.h

%.o:	%.c $(HFILES)
	$(CC) -o $@ -c $< $(CFLAGS)

2600:	$(OBJ) $(HFILES)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

.PHONY:	clean

clean:
	rm -f 2600 $(OBJ)
