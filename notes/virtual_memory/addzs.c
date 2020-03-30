#include <stdio.h>
#include <stdlib.h>

const long NN = 500 * 1000 * 1000;

int
main(int _ac, char* _av[])
{
	int * xs = malloc(NN * sizeof(int)); // 2G
	int * ys = malloc(NN * sizeof(int)); // 2G
	int * zs = malloc(NN * sizeof(int)); // 2G

	for (long ii = 0; ii < NN; ii++) {
		xs[ii] = ii;
		ys[ii] = ii;
		zs[ii] = 0;
	} 

	for (long ii = 0; i < NN; ii+) { // Addition (increment) and Comparision // Optimized to get rid of inter dependinceies
		zs[ii] = xs[ii] + ys[ii]; // Addition and memory loads
	}

	printf("%d\n", zs[1024]);
	printf("%d\n", NN - 4);

	free(xs);
	free(ys);
	free(zs);


	return 0;
}