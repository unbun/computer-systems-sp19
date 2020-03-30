#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

#define FLT_MAX 3.402823466e+38F /* max value */


int 
compare_floats (const void * a, const void * b)
{
  float fa = *(const float*) a;
  float fb = *(const float*) b;
  return (fa > fb) - (fa < fb);
}

void
qsort_floats(floats* xs)
{
    // DONE: call qsort to sort the array
    qsort(xs->data, xs->size, sizeof(float), compare_floats);
    return;
}

floats*
sample(float* data, long size, int P)
{
    // DONE: sample the input data, per the algorithm decription

    // Randomly select 3*(P-1) items from the array.
    int randNum = 3 * (P - 1);

    floats* rSort = make_floats(randNum);

    copy_rand_values(data, rSort, randNum, (int) size);

    // Sort those items.
    qsort_floats(rSort);

    // Take the median of each group of three in the sorted array, producing an array (samples) of (P-1) items.
    // [Since it's sorted, the medians of groups of three are index 1, 4, 7...(+3)]
    floats* samples = make_floats(P+1); 
    floats_push(samples, 0.0f); // Add 0 at the start 

    for(int ii = 1; ii < randNum; ii += 3){
        floats_push(samples, floats_get(rSort, ii));
    }

    floats_push(samples, FLT_MAX); // and +inf at the end (or the min and max values of the type being sorted) 

    free_floats(rSort);

    return samples;
}

void
sort_worker(int pnum, float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    floats* xs = make_floats(10);
    // TODO: select the floats to be sorted by this worker

    float start = floats_get(samps, pnum);
    (void) start;

    struct timeval tv;
    gettimeofday(&tv, 0);

    printf("%d: start %.04f, count %ld\n", pnum, (float)tv.tv_usec, xs->size);

    // TODO: some other stuff

    qsort_floats(xs);

    // TODO: probably more stuff

    free_floats(xs);
}

void
run_sort_workers(float* data, long size, int P, floats* samps, long* sizes, barrier* bb)
{
    pid_t kids[P];

    // DONE: spawn P processes, each running sort_worker

    for (int ii = 0; ii < P; ++ii) {

        int cpid;
        if((cpid = fork())){
            kids[ii] = cpid;

            //barrier_wait(bb); // i know that these barriers are causing deadlock, but im not sure how to fix it :(

            int rv = waitpid(kids[ii], 0, 0);
            check_rv(rv);

        } else {
            //barrier_wait(bb);
            kids[ii] = getpid();
            sort_worker(ii, data, size, P, samps, sizes, bb); // TODO: actually partition based on P
        }
    }
}

void
sample_sort(float* data, long size, int P, long* sizes, barrier* bb)
{
    floats* samps = sample(data, size, P);
    //floats_print(samps);
    run_sort_workers(data, size, P, samps, sizes, bb);
    free_floats(samps);
}

int
main(int argc, char* argv[])
{
    alarm(120);

    if (argc != 3) {
        printf("Usage:\n");
        printf("\t%s P data.dat\n", argv[0]);
        return 1;
    }

    const int P = atoi(argv[1]);
    const char* fname = argv[2];

    seed_rng();

    int rv;
    struct stat st;
    rv = stat(fname, &st);
    check_rv(rv);

    const int fsize = st.st_size;
    if (fsize < 8) {
        printf("File too small.\n");
        return 1;
    }

    int fd = open(fname, O_RDWR);
    check_rv(fd);

    int* file = mmap(0, 4096, PROT_READ|PROT_WRITE,
                       MAP_SHARED, fd, 0); // DONE: load the file with mmap.
    check_rv((long)file);

    // DONE: These should probably be from the input file.
    long count = file[0];
    float* data = malloc(count * sizeof(float));
    for (int ii = 1; ii < count + 1; ++ii) {
        // printf("file[%d] = %f\n", ii, (float)file[ii]);
        int idata = file[ii];
        float fdata = (float) idata;
        data[ii - 1] = fdata;
    }


    // floats_print(data);

    long sizes_bytes = P * sizeof(long);
    long* sizes = mmap(0, sizes_bytes, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0); // DONE: This should be shared

    barrier* bb = make_barrier(P);

    sample_sort(data, count, P, sizes, bb);

    free_barrier(bb);

    // DONE: munmap your mmaps
    rv = munmap(file, 4096);
    check_rv(rv);
    rv = munmap(sizes, sizes_bytes);
    check_rv(rv);
    rv = close(fd);
    check_rv(rv);
    free(data);


    return 0;
}

