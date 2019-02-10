#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int ret, fd;

	ret = mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);
	if(ret != 0) {
		perror("mount devtmpfs error");
		goto out;
	}

	ret = mount("/dev/sda", "/root", "ext4", 0, NULL);
	if(ret != 0) {
		perror("mount root on /dev/sda error");
		goto out;
	}

	ret = mount("/dev", "/root/dev", NULL, MS_MOVE, NULL);
	if(ret != 0) {
		perror("move devtmpfs to /root/dev error");
		goto out;
	}

	ret = chdir("/root");
	if(ret != 0) {
	perror("change to /root directory error");
		goto out;
	}
	
	ret = mount(".", "/", NULL, MS_MOVE, NULL);
	if(ret != 0) {
		perror("overmount the root error");
		goto out;
	}

	ret = chroot(".");
	if(ret != 0) {
		perror("chroot error");
		goto out;
	}

	ret = chdir("/");
	if(ret != 0) {
		perror("chdir / error");
		goto out;
	}

	fd = open("/dev/console", O_RDWR);
	if(fd < 0) {
		perror("open /dev/console error");
		goto out;
	}
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);

	printf("initramfs init exec completed, start to exec root init process\n");
	execv("/sbin/init", argv);
out:
	while(1);
	return 0;
}
