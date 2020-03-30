#include <stdio.h> //fopen

int
main(int _ac, char* av[])
{
	FILE* thing = fopen("thing1.txt", "w");
	fprintf(thing, "Hello thing1\n");
	fclose(thing);


	char temp[80];
	FILE* input = fopen("thing1.txt", "r");
	fgets(temp, 80, input); // puts 
	fclose(input);

	fprintf(stdout, "%s", temp);
	return 0;
}