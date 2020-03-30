#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

int
main(int _ac, char* _av[])
{
	int cpid;

	if ((cpid = fork())){
		
		int status;
        waitpid(cpid, 0, 0);
        printf("After running echo.\n");
	}
	else {
        int fd = open("stdout.txt", O_CREAT | O_APPEND | O_WRONLY, 0644);

		close(1); //closes STDOUT
		dup(fd); // copies fd into lowest available slot (just closed 1)
		close(fd);

		execlp("echo", "echo", "child", "says", "hi", NULL);
	}

	return 0;
}