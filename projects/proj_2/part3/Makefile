ifneq ($(KERNELRELEASE),)
	obj-m := elev_sched.o
	#obj-y := passenger.o floor.o elevator.o
	#my_elev-objs :=  passenger.o

	elev_sched-y := my_elev.o passenger.o floor.o elevator.o

else
	KERNELDIR ?= /lib/modules/`uname -r`/build/
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	-rm -rf *.ko *.o Module* *.mod.c .tmp_versions *.order .*.cmd
