all: mite

CC = gcc
CFLAGS = -O2 -g -Wall
LIBS = -lftdi

OBJS = i2c.o i2c-v1.o i2c-v2.o mite.o

*.o: Makefile *.h

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

mite: $(OBJS)
	$(CC) $^ -o $@ $(LIBS)

clean:
	-rm -f *.o mite
