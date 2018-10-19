CC=gcc
LIBS=-lcap
CFLAGS=-g
RM=rm -f
SET_PREVILLAGE=setcap cap_setfcap+eip

defalut: all set_previllage

all: ioctl.c
	$(CC) *.c -o ioctl $(LIBS) $(CFLAGS)


set_previllage:
	$(SET_PREVILLAGE) ./ioctl

clean:
	$(RM) ioctl

