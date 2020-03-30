#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


#include "svec.h"
#include "execute_tokens.h"

int REDIRECT_IN = 1;
int REDIRECT_OUT = 2;
int LOGICAL_AND = 3;
int LOGICAL_OR = 4;

void
assert_ok(long rv, char* call)
{
    if (rv == -1) {
        fprintf(stderr, "Failed call: %s\n", call);
        abort();
    }
}

void
execute_cd(svec* cmd)
{
    char* dir = svec_get(cmd, 1);
    int rv;
    rv = chdir(dir);
    if (rv == -1) {
        printf("$nush: cd: %s: No such file or directory", dir);
    }
}

void
execute_base_case(svec* xs)
{

    if (strncmp(svec_get(xs, 0), "cd", 2) == 0) {
        execute_cd(xs);
        return;
    }

    if (strncmp(svec_get(xs, 0), "exit", 4) == 0) {
        exit(0);
    }

    int cpid;
    if ((cpid = fork())) { // PARENT
        int status;
        waitpid(cpid, &status, 0); // Child may still be running until we wait.

    } else { // CHILD
        int ii;
        char* args[xs->size + 1];
        for (ii = 0; ii < xs->size + 1; ++ii) {
            if (ii < xs->size) {
                args[ii] = svec_get(xs, ii);
            } else {
                args[ii] = 0;
            }
        }

        execvp(svec_get(xs, 0), args);
        exit(0);
    }
}


void
execute_redirect(svec* tokens, int direction)
{
    int rv;
    int cpid;

    char fout[100];
    char* cc;
    if(direction == REDIRECT_OUT) {
        cc = ">";

    } else if(direction == REDIRECT_IN) {
        cc = "<";

    }

    // get the file name from the tokens
    int ii;
    for (ii = 0; ii < tokens->size; ++ii) {
        if (strncmp(svec_get(tokens, ii), cc, 1) == 0) {
            strcpy(fout, svec_get(tokens, ii + 1));
            break;
        }
    }

    if ((cpid = fork())) { // PARENT
        int status;
        waitpid(cpid, &status, 0); 

    } else { // CHILD
        int fd =1;
        if(direction == REDIRECT_OUT) {
            fd = open(fout, O_CREAT | O_WRONLY, 0644);
            assert_ok(fd, "open for write");

            rv = close(1);
            assert_ok(rv, "close 1");

        } else if(direction == REDIRECT_IN) {
            fd = open(fout, O_RDONLY, 0664);
            assert_ok(fd, "open for read");

            rv = close(0);
            assert_ok(rv, "close 0");

        }
        
        rv = dup(fd);
        assert_ok(rv, "dup");

        rv = close(fd);
        assert_ok(rv, "close fd");

        int ii; // tokens index
        int jj = 0; // args index
        char* args[tokens->size + 1];
        for (ii = 0; ii <= tokens->size; ++ii) {
            if (ii < tokens->size) {
               char* tt = svec_get(tokens, ii);
               if (strncmp(tt, cc, 1) != 0) {
                   args[jj] = tt;
                   jj++;
               } else {
                    args[jj] = 0;
                    break;
                }
            } else {
                args[jj] = 0;
            }	
        }   

        execvp(svec_get(tokens, 0), args);
    }
}


void
execute_redirect_in(svec* tokens)
{
    execute_redirect(tokens, REDIRECT_IN);
}

void
execute_redirect_out(svec* tokens)
{
    execute_redirect(tokens, REDIRECT_OUT);
}

void
execute_pipe(svec* tokens)
{
    int cpidwrap;

    if ((cpidwrap = fork())) {
        int status;
        waitpid(cpidwrap, &status, 0);

    } else {
        int rv;

        int pipe_fds[2];
        rv = pipe(pipe_fds);
        assert_ok(rv, "pipe");

        int p_read = pipe_fds[0];
        int p_write = pipe_fds[1];
        
        int cpid1;
        if ((cpid1 = fork())) { // PARENT 1
            int cpid2;

            if((cpid2 = fork())) { // PARENT 2
                close(p_read);
                close(p_write);

                int status;
                waitpid(cpid1, &status, 0);
                waitpid(cpid2, &status, 0);
            } else { // CHILD 2

                close(0);
                
                rv = dup(p_read);
                assert_ok(rv, "dup p_read");

                close(p_write);
                close(p_read);

                int jj = 0; // args incrementor
                int build_args = 0;
                char* args[tokens->size];

                int pipeii = 0;

                for (int ii = 0; ii <= tokens->size; ++ii) {
                    if (ii < tokens->size) {
                        char* tt = svec_get(tokens, ii);
                        if (strncmp(tt, "|", 2) == 0) {
                            build_args = 1;
                            pipeii = ii;
                        } else if (build_args) {
                            args[jj] = tt;
                            jj++;
                        }
                    } else {
                        args[jj] = 0;
                    }
                }
                execvp(svec_get(tokens, pipeii + 1), args);
            }
        } else { // CHILD 1
            close(1);

            rv = dup(p_write);
            assert_ok(rv, "dup p_write");

            close(p_read);
            close(p_write);

            char* args[tokens->size + 1];

            for (int ii = 0; ii < tokens->size; ++ii) {
                char* tt = svec_get(tokens, ii);
                if (strncmp(tt, "|", 2) != 0) {
                    args[ii] = tt;
                } else {
                    args[ii] = 0;
                    break;
                }
            }
            execvp(svec_get(tokens, 0), args);
        }
    }
}

void
execute_background(svec* tokens)
{
    int cpid;
    if ((cpid = fork())) {
        // don't wait
    } else {
        char* args[tokens->size + 1];

        for (int ii = 0; ii < tokens->size + 1; ++ii) {
                if (ii < tokens->size) {
                        args[ii] = svec_get(tokens, ii);
                } else {
                        args[ii] = 0;
                }
        }
        execvp(svec_get(tokens, 0), args);
    }
}

void 
execute_logic(svec* tokens, int opFlag) {
    char* op;

    if (opFlag == LOGICAL_AND) {
        op = "&&";
    } else if (opFlag == LOGICAL_OR) {
        op = "||";
    }

    int cpid;

    if ((cpid = fork())) { // PARENT
        int status;
        waitpid(cpid, &status, 0);

        // can't short circuit answer:
        // or[false], and[true]
        if (((opFlag == LOGICAL_OR) && WEXITSTATUS(status)) 
            || ( (opFlag == LOGICAL_AND) && !WEXITSTATUS(status))) {

            int jj = 0; // args incrementer
            int build_args = 0;

            int opii = 0;

            char* args[tokens->size];
            for (int ii = 0; ii <= tokens->size; ++ii) {
                if (ii < tokens->size) {
                    char* tt = svec_get(tokens, ii);
                    if (strncmp(tt, op, 2) == 0) {
                        build_args = 1;
                        opii = ii;
                    } else if (build_args) {
                        args[jj] = tt;
                        jj++;
                    }
                } else {
                    args[jj] = 0;
                }
            }

            // force exit(0) because status is being overridden
            if (strncmp(svec_get(tokens, opii + 1), "exit", 4) == 0) {
                exit(0);
            } else {
                execvp(svec_get(tokens, opii + 1), args);
            }
        }

    } else { // CHILD
        int ii;
        char* args[tokens->size + 1];
        for (ii = 0; ii < tokens->size; ++ii) {
            char* tt = svec_get(tokens, ii);
            if (strncmp(tt, op, 2) != 0) {
                args[ii] = tt;
            } else {
                args[ii] = 0;
                break;
            }
        }
        execvp(svec_get(tokens, 0), args);
    }
}

void 
execute_logical_and(svec* tokens) {
    execute_logic(tokens, LOGICAL_AND);
}



void 
execute_logical_or(svec* tokens){
    execute_logic(tokens, LOGICAL_OR);
}