EXTRA_CFLAGS := -I $(src)/

obj-m += myfs.o
myfs-objs := fs_driver.o fstype.o mount.o superblock.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
