#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


//helper to see system call failures
void
check_rv(int rv)
{
	if (rv < 0) {
		perror("fail");
		exit(1);
	}
}

int
main(int _ac, char* _av[])
{
	int rv;
	char msg[] = "Hello, pipe.\n";

	int pipe_fds[2];
	rv = pipe(pipe_fds); //pipe system call, allocates file descriptors to the input
	check_rv(rv);

 	//pipefd[0] -> read from, //pipefd[1] -> write to
	int p_read = pipe_fds[0];
	int p_write = pipe_fds[1];

	rv = write(p_write, msg, strlen(msg));
	check_rv(rv);

	char temp[100];
	rv = read(p_read, temp, 99);
	check_rv(rv);

	printf("%s", temp);

	return 0;

}
