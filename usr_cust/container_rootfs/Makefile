currdir := $(shell pwd)

outdir := $(CONTAINER_ROOTFS_OUT_DIR)

usr_cust_container_rootfs_all:
	mkdir -p $(outdir)/dev $(outdir)/proc $(outdir)/sys $(outdir)/etc/init.d
	cp $(currdir)/rcS $(outdir)/etc/init.d/rcS
