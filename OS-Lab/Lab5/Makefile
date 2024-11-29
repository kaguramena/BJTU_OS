obj-m += mydevice.o

mydevice_test : mydevice_test.c
	gcc -o $@ $<

test: mydevice_test mydevice.ko
	sudo rmmod mydevice
	sudo insmod mydevice.ko
	sudo chmod 666 /dev/mydevice
	./mydevice_test

all: mydevice_test
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean