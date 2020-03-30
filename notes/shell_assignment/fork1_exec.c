#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int
main(int _ac, char* _av[])
{
	int opid = getpid();
	int opar = getppid();
	int cpid, pid, par;

	printf("Hi I'm %d, child of %d\n", opid, opar);

	if ((cpid = fork())) {     					// SAME_LINE
		//fork returned in parent
		pid = getpid();
		par = getppid();
		printf("1) Hi I'm %d, child of %d. (cpid = %d)\n", pid, par, cpid);

		//parent process will wait for child:
		int st; // exit status information for child
		wait(&st); // wait until one of ur children die
	}
	else {
		//in child

		execlp("echo", "echo", "this", "is", "echo", NULL);
		// l: take argv as arguments directly
		// p: use PATH env variable to find program
		//
		// Exec never returns
		// It just completely wipes out the current process,
		// and makes it the echo program. So "All Done" won't be run in the child process.
		// the new echo is the same process, so it has the same pid
	}


	printf("All Done\n");

	return 0;
}