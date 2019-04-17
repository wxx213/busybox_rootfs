#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

// misc_mm common header start
struct misc_mm_memory {
    void *addr;
    unsigned long size;
};

#define MISC_MM_ALLOC_MEM     _IOWR('m', 0, struct misc_mm_memory)
#define MISC_MM_FREE_MEM      _IOWR('m', 1, struct misc_mm_memory)
#define MISC_MM_KERN_MEM      _IO('m', 2)
// misc_mm common header end

#define DEVICE_NAME "/dev/misc_mm"

int main(int argc, char *argv[])
{
	int ret, fd;
	struct misc_mm_memory misc_mm;
	void *mmap_addr;
	char *para;

	if(argc < 2) {
		printf("Invalid parameter\n");
		printf("Usage: misc_mm_test option\n");
		printf("option:\n");
		printf("    ioctl_alloc  alloc and free user space memory.\n");
		printf("    ioctl_kern   alloc and free kernel space memory\n");
		printf("    map          mmap memory\n");
		return 1;
	}
	para = argv[1];
	fd = open(DEVICE_NAME, O_RDWR, 0755);
	if(fd < 0) {
		perror("open device node failed");
		return 1;
	}
	if(0 == strncmp(para, "ioctl_alloc", strlen("ioctl_alloc"))) {
		misc_mm.size = 100;
		ret = ioctl(fd, MISC_MM_ALLOC_MEM, &misc_mm);
		if(ret) {
			perror("ioctl alloc memory error");
			close(fd);
			return 1;
		}
		memcpy(misc_mm.addr, "misc_mm test string", sizeof("misc_mm test string"));
		ret = ioctl(fd, MISC_MM_FREE_MEM, &misc_mm);
		if(ret) {
			perror("ioctl free memory error");
			close(fd);
			return 1;
		}
	}
	else if(0 == strncmp(para, "ioctl_kern", strlen("ioctl_kern"))) {
		ret = ioctl(fd, MISC_MM_KERN_MEM);
		if(ret) {
			perror("ioctl MISC_MM_KERN_MEM error");
			close(fd);
			return 1;
		}
	}
	else if(0 == strncmp(para, "map", strlen("map"))) {
		mmap_addr = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(MAP_FAILED == mmap_addr) {
			perror("mmap failed");
			close(fd);
			return 1;
		}
	}
	else {
		printf("Unsported parameter\n");
	}
	close(fd);
	return 0;
}
