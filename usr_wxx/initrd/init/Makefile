currdir := $(shell pwd)

outdir := $(EXPORT_INITRD_OUT_DIR)

usr_wxx_initrd_init:
	gcc init.c -static -o $(outdir)/init
