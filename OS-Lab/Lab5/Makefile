obj-m += mydevice.o
obj-m += myalarm.o

mydevice_test : mydevice_test.c
	gcc -o $@ $<

myalarm_test : myalarm_test.c
	gcc -o $@ $<

test_device: mydevice_test mydevice.ko
	sudo insmod mydevice.ko
	sudo chmod 666 /dev/mydevice
	./mydevice_test
	sudo rmmod mydevice

test_alarm: myalarm_test myalarm.ko
	sudo insmod myalarm.ko
	sudo chmod 666 /dev/myalarm
	./myalarm_test
	sudo rmmod myalarm

all: mydevice_test myalarm_test
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean