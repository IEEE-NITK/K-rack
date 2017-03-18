#
# Makefile for the linux ext2-filesystem routines.
#

obj-m += kr.o

kr-objs := balloc.o dir.o file.o ialloc.o inode.o \
	  ioctl.o namei.o super.o symlink.o

# ext2-$(CONFIG_EXT2_FS_XATTR)	 += xattr.o xattr_user.o xattr_trusted.o
# ext2-$(CONFIG_EXT2_FS_POSIX_ACL) += acl.o
# ext2-$(CONFIG_EXT2_FS_SECURITY)	 += xattr_security.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
