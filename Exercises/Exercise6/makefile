EXTRA_CFLAGS := -I$(src)/

obj-m += devfile.o
devfile-objs := driver.o read.o release.o open.o write.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
