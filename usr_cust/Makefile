currdir := $(shell pwd)

ifeq ($(USR_CUST_TARGET),initrd)
usr_cust_initrd:
	make -C $(currdir)/initrd
else
usr_cust_rootfs:
	make -C $(currdir)/rootfs
endif

