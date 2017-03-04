EXTRA_CFLAGS := -I $(src)/

obj-m += krack.o
krack-objs := mount.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
