#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "tokenize.h"

void
chomp(char* text)
{
    char *pp;
    if ((pp=strchr(text, '\n')) != NULL)
        *pp = '\0';
}

int
main(int _argc, char* _argv[]) {

    char line[100];

    while (1) {
        printf("tokens$ ");

        fflush(stdout);

        // line = read_line()
        char* rv = fgets(line, 96, stdin);
        if(!rv) { // handles EOF call
            printf("\n");
            exit(0);
        }

        chomp(line);

        tokenize_and_rev_print(line);
     }

     return 0;
 }