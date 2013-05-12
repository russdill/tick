all: tick

CC = gcc
CFLAGS = -O2 -g -Wall
LIBS = -lftdi

OBJS = i2c.o i2c-v2.o tick.o

*.o: Makefile *.h

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tick: $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

clean:
	-rm -f *.o tick
