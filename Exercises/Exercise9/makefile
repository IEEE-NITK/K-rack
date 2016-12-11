EXTRA_CFLAGS := -I$(src)/

obj-m += freak.o
freak-objs := module9.o read.o open.o release.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
