#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "svec.h"
#include "tokenize.h"
#include "execute_tokens.h"

void
execute(svec* cmd)
{
    int cpid;

    if (svec_contains(cmd, "<", 1)) {
        execute_redirect_in(cmd);

    } else if (svec_contains(cmd, ">", 1)) {
        execute_redirect_out(cmd);

    } else if (svec_contains(cmd, "&&", 2)) {
        execute_logical_and(cmd);

    } else if (svec_contains(cmd, "||", 2)) {
        execute_logical_or(cmd);

    } else if (svec_contains(cmd, "|", 1)) {
        execute_pipe(cmd);

    } else if (svec_contains(cmd, "&", 1)) {
        execute_background(cmd);

    } else {
        execute_base_case(cmd);
    }

}

void
chomp(char* text)
{
    char *pp;
    if ((pp=strchr(text, '\n')) != NULL)
        *pp = '\0';
}

int
main(int argc, char* argv[])
{
    char cmd[256];

    if (argc == 1) {
        printf("nush$ ");

        while(fgets(cmd, 256, stdin) != NULL) {
            fflush(stdout);
            if (cmd[0] != '\n') {
                svec* tokens = tokenize(cmd);
                execute(tokens);
                free_svec(tokens);
            }
            printf("nush$ ");
        }
    }
    else {
        FILE* pFile;
        pFile = fopen(argv[1], "r");
        if (pFile != NULL) {
            while(fgets(cmd, 256, pFile) != NULL) {
                fflush(stdout);
                chomp(cmd);
                svec* tokens = tokenize(cmd);

                // if (svec_find(tokens, "exit", 4)) {
                //     exit(0);
                // }

                execute(tokens);

                free_svec(tokens);
            }
        }
        fclose(pFile);
    }
    return 0;
} 