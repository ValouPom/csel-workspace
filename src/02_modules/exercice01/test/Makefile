# Part executed when called from kernel build system
ifneq ($(KERNELRELEASE),)
obj-m += mymodule.o          ## name of the generated module
mymodule-objs := exercice1.o  ## list of objets needed by that module
CFLAGS_skeleton := -DDEBUG   ## to enable printing of debugging messages

# Part executed when called from standard make in the module source directory
else
CVER     := aarch64-buildroot-linux-gnu-
KVER     := 5.15.148
CPU      := arm64

KDIR     := /buildroot/output/build/linux-$(KVER)
TOOLS    := /buildroot/output/host/usr/bin/$(CVER)
MODPATH  := /rootfs
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) 

endif