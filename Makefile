# Makefile

obj-m += exmodule.o
obj-m += exchrdev.o
obj-m += excdev.o
obj-m += exmisc.o
obj-m += extrace.o
obj-m += exgpio.o
obj-m += exinterrupt.o
obj-m += exwaitqueue.o
obj-m += exthreaded.o
obj-m += exworkqueue.o
obj-m += exmutex.o
obj-m += exspinlock.o
obj-m += exi2c.o
obj-m += exwarnon.o
obj-m += exparam.o
obj-m += exdebugfs.o
obj-m += exsysfs.o
obj-m += exerror.o
obj-m += exdt.o

ccflags-y += -Wall

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

KDIR=$(HOME)/project/linux-pi

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) *.html
