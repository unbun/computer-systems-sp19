#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
main(int ac, char* av[])
{
	// creating a new file descriptor, file descriptor 3
	// with system calls, open sets the file permission if it's creating it
	int thing = open("thing2.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644); 
	char* msg = "Hello, thing2\n";
	write(thing, msg, strlen(msg));
	close(thing);

	char temp[80];
	int input = open("thing2.txt", O_RDONLY);
	int len = read(input, temp, 80);
	close(input);

	write(1, temp, len);

	return 0;
}