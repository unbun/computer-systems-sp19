#include <sys/mman.h> //mmap
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void
assert_ok(long rv, char* call)
{
	if (rv == -1) {
		fprintf(stderr, "Failed call: %s\n", call);
		abort();
	}
}

int
main (int _ac, char* _av[])
{
	int* shared = mmap(0, 4096, PROT_READ | PROT_WRITE,
					 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
					 // makes it shared
	for(int ii = 0; ii < 10; ii++) {
		shared[ii] = ii;
	}

	int cpid;
	if((cpid = fork())) {
		printf("parent: sleep 1\n");
		sleep (1);

		printf("parent: mutating array\n");
		for (int ii = 0; ii < 10; ii++) {
			shared[ii] = ii * 100;
		}

		waitpid(cpid, 0, 0);
	} else {
		printf("child: array contents\n");

		for (int ii = 0; ii < 10; ii++) {
			printf("%d ", shared[ii]);
		}

		printf("\nchild: sleep 2\n");
		sleep(2);


		for (int ii = 0; ii < 10; ii++) {
			printf("%d ", shared[ii]);
		}

		printf("\n");
	}

	return 0;
} 