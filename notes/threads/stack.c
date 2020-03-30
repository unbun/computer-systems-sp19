#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>

//global variables (in .data)

const int NN = 100;
const int PP = 1;

int stack[5];
int sp = 0; // the index where we can insret the next item

void
stack_push(int xx)
{
	stack[sp++] = xx;
}

int
stack_pop()
{
	return stack[--sp];
}

void
producer_thread(void* _arg){
	for (int ii = 0; ii < NN; ++ii) {
		stack_push(ii);
	}
}

int
main(int _ac, char* _av[])
{

	pthread_t threads[PP];
	int rv;

	for (int ii = 0; ii < PP; ++ii) {
		// spawn a worker thread
		rv = pthread_create(&(threads[ii]), 0, producer_thread, 0);
		assert(rv == 0);
	}

	for (int ii = 0; ii < NN; ++ii) {
		printf("%d\n", stack_pop());
	}

	for (int ii = 0; ii < PP; ++ii) {
		// spawn a worker thread
		rv = pthread_join(threads[ii], 0);
		assert(rv == 0);
	}

	return 0;
}