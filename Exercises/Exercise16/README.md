# What is this about?

This is an ongoing assignment to solve the problems mentioned in the URL.
http://www.cs.columbia.edu/~jae/4118-LAST/HW6-fs.html

## How to run this?
* `make' to compile the source
* `sudo insmod ./lwnfs.ko' to insert the filesystem module
* `mkdir some_dir' to create a new directory for mounting the filesystem
* `touch image' create an empty file
* `sudo mount -o loop -t lwnfs ./image ./some_dir' to mount the filesystem
* You can successfully `cd' into the directory and cat the counter files.

More features coming soon.
Contact:
Adithya Bhat
<dth.bht@gmail.com>
