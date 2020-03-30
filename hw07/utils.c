// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include "float_vec.h"

void
seed_rng()
{
    struct timeval tv;
    gettimeofday(&tv, 0);

    long pid = getpid();
    long sec = tv.tv_sec;
    long usc = tv.tv_usec;

    srandom(pid ^ sec ^ usc);
}

void
check_rv(int rv, char* name)
{
    if (rv == -1) {
        perror(name);
        fflush(stdout);
        fflush(stderr);
        abort();
    }
}

double
get_time() {
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec + t.tv_usec/1000000.0;
}

int
ints_contains(int* arr, int val, int size)
{
    for(int ii = 0; ii < size; ii++)
    {
        if(arr[ii] == val) {
            //printf("\trepeated at %d: %d\n", ii, val);
            return 1;
        }
    }

    return 0;
}

void
copy_rand_values(float* src, floats* dest, int nCopy) {
    int* used = malloc(nCopy * sizeof(int));

    for (int ii = 0; ii < nCopy; ii++) {
        used[ii] = -1;
    }

    for(int ii = 0; ii < nCopy; ii++) {
        int inum;
        int diff = 0;
        do {
            inum = random() % nCopy;
            diff = ints_contains(used, inum, nCopy);
        } while (diff);

        used[ii] = inum;
        floats_push(dest, src[inum]);
    }

    free(used);
}

void print_list(float *ll, int size, char *name) {
    printf("\n======= %s =======\n", name);
    for (int ii = 0; ii < size; ii++) {
        printf("%f ", ll[ii]);
    }
    printf("\n");
}

void print_list_ln(float *ll, int size, char *name) {
    printf("\n======= %s =======\n", name);
    for (int ii = 0; ii < size; ii++) {
        printf("%f\n ", ll[ii]);
    }
    printf("\n");
}
