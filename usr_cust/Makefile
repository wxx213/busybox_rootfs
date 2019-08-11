currdir := $(shell pwd)

ifeq ($(USR_CUST_TARGET),initrd)
usr_cust_initrd:
	make -C $(currdir)/initrd
endif

ifeq ($(USR_CUST_TARGET),rootfs)
usr_cust_rootfs:
	make -C $(currdir)/rootfs
endif

ifeq ($(USR_CUST_TARGET),container_rootfs)
usr_cust_container_rootfs:
	make -C $(currdir)/container_rootfs
endif
