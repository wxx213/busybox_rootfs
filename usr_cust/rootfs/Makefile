currdir := $(shell pwd)

outdir := $(EXPORT_ROOTFS_OUT_DIR)

usr_cust_rootfs_all:
	mkdir -p $(outdir)/dev
	mkdir -p $(outdir)/proc
	mkdir -p $(outdir)/sys
	mkdir -p $(outdir)/lib
	mkdir -p $(outdir)/lib64
	mkdir -p $(outdir)/etc/init.d
	mkdir -p $(outdir)/usr
	mkdir -p $(outdir)/lib/x86_64-linux-gnu
	mkdir -p $(outdir)/vioblk
	mkdir -p $(outdir)/scsib
	mkdir -p $(outdir)/usr/lib/x86_64-linux-gnu
	mkdir -p $(outdir)/usr/share/udhcpc
	mkdir -p $(outdir)/tmp
	mkdir -p $(outdir)/usr/bin
	gcc $(currdir)/usr/test.c -o $(outdir)/usr/test
	cp $(currdir)/etc/init.d/rcS $(outdir)/etc/init.d/
	cp $(currdir)/usr/test_a $(outdir)/usr/
	cp $(currdir)/usr/test_d $(outdir)/usr/
	cp -d $(currdir)/lib/x86_64-linux-gnu/* $(outdir)/lib/x86_64-linux-gnu/
	cp -d $(currdir)/lib64/ld-linux-x86-64.so.2 $(outdir)/lib64/
	cp $(currdir)/usr/test $(outdir)/usr/
	cp -d -r $(currdir)/usr/lib/x86_64-linux-gnu/* $(outdir)/usr/lib/x86_64-linux-gnu/
	cp $(currdir)/default.script $(outdir)/usr/share/udhcpc/
	gcc $(currdir)/mk_miscfs.c -o $(outdir)/usr/bin/mk_miscfs
	gcc $(currdir)/misc_mm_test.c -o $(outdir)/usr/bin/misc_mm_test
	gcc $(currdir)/misc_sched_test.c -static -o $(outdir)/usr/bin/misc_sched_test
