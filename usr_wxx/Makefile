currdir := $(shell pwd)

ifeq ($(USR_WXX_TARGET),initrd)
usr_wxx_initrd:
	make -C $(currdir)/initrd
else
usr_wxx_rootfs:
	$(currdir)/rootfs/install_wxx.sh $(EXPORT_ROOTFS_OUT_DIR) $(currdir)/rootfs
endif

