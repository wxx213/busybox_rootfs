#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int ret;
	int i = 0;
	ret = daemon(0, 0);
	if(ret < 0) {
		perror("daemon error");
		return 1;
	}

	for(;;) i++;
	return 0;
}
