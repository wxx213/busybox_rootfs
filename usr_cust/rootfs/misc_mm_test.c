#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// misc_mm common header start
struct misc_mm_memory {
    void *addr;
    unsigned long size;
};

#define MISC_MM_ALLOC_MEM     _IOWR('m', 0, struct misc_mm_memory)
#define MISC_MM_FREE_MEM      _IOWR('m', 1, struct misc_mm_memory)
// misc_mm common header end

#define DEVICE_NAME "/dev/misc_mm"

int main()
{
	int ret, fd;
	struct misc_mm_memory misc_mm;
	void *mmap_addr;

	fd = open(DEVICE_NAME, O_RDWR, 0755);
	if(fd < 0) {
		perror("open device node failed");
		return 1;
	}
	misc_mm.size = 10;
	ret = ioctl(fd, MISC_MM_ALLOC_MEM, &misc_mm);
	if(ret) {
		perror("ioctl alloc memory error");
		close(fd);
		return 1;
	}
	ret = ioctl(fd, MISC_MM_FREE_MEM, &misc_mm);
	if(ret) {
		perror("ioctl free memory error");
		close(fd);
		return 1;
	}
	mmap_addr = mmap(NULL, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == mmap_addr) {
		perror("mmap failed");
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}
