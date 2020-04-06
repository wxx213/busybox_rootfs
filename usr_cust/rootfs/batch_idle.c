#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

static int print_verb = 0;
static int fork_new = 0;

void (siguser1handler)(int sig)
{
	if ( sig == SIGUSR1) {
		fork_new = 1;
	}
}

int main(int argc, char *argv[])
{
	pid_t pid = getpid();
	if (argc > 1) {
		if (!strncmp(argv[1], "-p", 2)) {
			print_verb = 1;
		}
	}
	if (signal(SIGUSR1, siguser1handler) == SIG_ERR) {
		printf("signal SIGUSR error\n");
	}
	int i = 0;
	while(1) {
		if (0 == i) {
			if (print_verb) {
				printf("batch_idle process is running, pid : %d\n", pid);
			}
			if (fork_new) {
				fork_new = 0;
				pid = fork();
				if (pid < 0) {
					printf("fork new process error\n");
				} else if (pid == 0) {
					while(1) i++;
				}
			}
		}
		i++;
	}
	return 0;
}
