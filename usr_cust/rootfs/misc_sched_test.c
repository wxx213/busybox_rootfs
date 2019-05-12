#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int ret;

	ret = daemon(0, 0);
	if(ret < 0) {
		perror("daemon error");
		return 1;
	}

	// the daemon process context, parent has exited.
	while(1) {

	}
	return 0;
}
