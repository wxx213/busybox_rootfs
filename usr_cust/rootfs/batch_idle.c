#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int print_verb = 0;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if (!strncmp(argv[1], "-p", 2)) {
			print_verb = 1;
		}
	}
	int i = 0;
	while(1) {
		if (print_verb && i == 0) {
			printf("batch_idle process is running\n");
		}
		i ++;
	}
	return 0;
}
