.PHONY: clean

obj-m += fprint.o
fprint-objs := driver.o entry.o proc.o

fprint.ko: driver.c fprintdrv.h entry.c entry.h proc.c proc.h constants.h
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
