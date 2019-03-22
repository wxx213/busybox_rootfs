currdir := $(shell pwd)

outdir := $(EXPORT_INITRD_OUT_DIR)

usr_cust_initrd_all:
	echo $(outdir)
	mkdir -p $(outdir)/dev $(outdir)/proc $(outdir)/sys $(outdir)/etc/init.d
	cp $(currdir)/rcS $(outdir)/etc/init.d/rcS
	ln -sf /bin/busybox $(outdir)/init
	mkdir -p $(outdir)/scsib
	cp $(currdir)/crashrcS $(outdir)/etc/init.d/crashrcS
	
	
