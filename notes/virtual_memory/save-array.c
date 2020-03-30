#include <sys/mman.h> //mmap
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
	long size = 9 * sizeof(int);

	int fd = open("./array.mem", O_CREAT | O_TRUNC | O_RDWR, 0644); // starts as length of 0
	assert_ok(fd, "open");

	int rv = ftruncate(fd, size); // extended to 40 bytes, all with value 0/null
	assert_ok(rv, "ftruncate");

	//set up 1 new page that fits 40 byte array 
	// and map the page to the mempor of the file
	int* xs = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	assert_ok((long) xs, "mmap");

	for (int ii = 0; ii < 10; ++ ii) {
		xs[ii] = ii* ii; // intel writes with little-endian
	}

	rv = munmap(xs, size);
	assert_ok(rv, "munmap");

	rv = close(fd);
	assert_ok(rv, "close");

	return 0;
} 