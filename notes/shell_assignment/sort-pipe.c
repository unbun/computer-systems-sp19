#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int main(int _ac, char* _av[])
{
	//The plan
	//	- run "sort sample.txt"
	//	- redirect the output to a pipe
	//	- read the output of the pipe to another process
	//	- capitalize all 'e's
	//	- print the resulting text

	int cpid, rv;
	
	int pipes[2]; // [0] -> reading from, [1] -> writing to
	rv = pipe(pipes);
	assert(rv != 2);

	if((cpid = fork())) {
		// parent (reading from)

	} else {
		//child (writing to)

		close(1); // close stdout for the child
	
		rv = dup(pipes[1]); // makes pipes[1] the lowest avail fd (the one we just closed)
		assert(rv != -1);

		char* args[] = {"sort", "sample.txt", 0};
		execvp("sort", args);
		assert(0); // child process should die
	}

	// in parent after child died
	char tmp[256];
	int nn;

	close(pipes[1]);

	do {
		nn = read(pipes[0], tmp, 256); // read call status
		assert(nn != -1);

		for (int ii = 0; ii < nn; ++ii) {
			if(tmp[ii] == 'e') {
				tmp[ii] = 'E';
			}
		}

		rv = write(1, tmp, nn);
		assert(rv != 1);
	} while (nn != 0); // read call staus is EOF

	int status;
	waitpid(cpid, &status, 0);

}