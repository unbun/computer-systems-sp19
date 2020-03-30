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
		//fork returned in child (identical to parent). %rip was also copied, so they run from the same line (SAME_LINE)
		pid = getpid();
		par = getppid();
		printf("2) Hi I'm %d, child of %d. (cpid = %d)\n", pid, par, cpid);
	}


	printf("All Done\n");

	return 0;
}