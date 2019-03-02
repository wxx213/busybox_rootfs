currdir := $(shell pwd)

ifeq ($(USR_WXX_TARGET),initrd)
usr_wxx_initrd:
	make -C $(currdir)/initrd
else
usr_wxx_rootfs:
	make -C $(currdir)/rootfs
endif

