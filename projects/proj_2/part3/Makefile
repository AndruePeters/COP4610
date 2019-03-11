ifneq ($(KERNELRELEASE),)
	obj-y += passenger.o floor.o
	obj-m := elev_mod.o
else
	KERNELDIR ?= /lib/modules/`uname -r`/build/
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	-rm -rf *.ko *.o Module* *.mod.c .tmp_versions *.order
