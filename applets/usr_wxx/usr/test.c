#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DISK_NODE "/dev/vda"
#define BLOCK_SIZE 4096

int main()
{
	int fd, ret;
	off_t disk_size, pos;
	unsigned int inodes;

	fd = open(DISK_NODE, O_WRONLY, 0666);
	if(fd < 0) {
		printf("open %s failed\n", DISK_NODE);
		return 1;
	}

	disk_size = lseek(fd, 0, SEEK_END);
	if(disk_size < 0) {
		printf("lseek %s failed\n", DISK_NODE);
		close(fd);
		return 1;
	}
	printf("disk size = %lu\n", disk_size);

	ret = lseek(fd, 0, SEEK_SET);
	if(ret < 0) {
		printf("lseek %s failed 2\n", DISK_NODE);
		close(fd);
		return 1;
	}
	else if(0 == ret) {
		printf("lseek %s succeeded 2\n", DISK_NODE);
	}

	pos = lseek(fd, BLOCK_SIZE, SEEK_SET);
	if(pos < 0) {
		printf("lseek %s failed 3\n", DISK_NODE);
		close(fd);
		return 1;
	}
	printf("pos = %lu\n", pos);

	ret = read(fd, &inodes, 4);
	if(ret < 0) {
		printf("read %s failed, errno = %s\n", DISK_NODE, strerror(errno));
		close(fd);
		return 1;
	}
	printf("inodes = %u\n", inodes);
	close(fd);
	return 0;
}
