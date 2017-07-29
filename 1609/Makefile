CC=gcc
CFLAGS=-Wall --std=c99
SRC=wsmp.c encode.c decode.c
HDR=wsmp.h encode.h decode.h
OBJS=wsmp.o encode.o decode.o

main: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c $(SRC)

test: test.c $(OBJS)
	$(CC) $(CFLAGS) test.c $(OBJS) -o test

clean:
	rm *.o
