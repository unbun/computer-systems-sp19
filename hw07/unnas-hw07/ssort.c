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


long* sizes;
int P; // size of sizes
float* input;
float* output; // using this array for personal debugging
long count; // size of input and output (called N in assignment)
floats* samples; // size = P + 1 (after its filled)

barrier* bb;

char* fin_name;
char* fout_name;


/**
 * The parallel code for sample sort worked. However, I couldn't get the file writing to work on my system. I included
 * the code here. I thoroughly tested the sample sorting (using the output float array) and was able to run the tests
 * for the report. The only part of the code that doesn't work is writing the output to a file in the thread worker.
 * My suspicion is that line 125 is the issue.
 */



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

void
fill_samples(float* data, long size, int P)
{
    // Randomly select 3*(P-1) items from the array. Sort those items
    int randNum = 3 * (P - 1);
    floats* rSort = make_floats(randNum);
    copy_rand_values(data, rSort, randNum);
    qsort_floats(rSort);

    // Take the median of each group of three in the sorted array, producing an array (samples) of (P-1) items.
    // [Since it's sorted, the medians of groups of three are index 1, 4, 7...(+3)]
    samples = make_floats(P+1);
    floats_push(samples, 0.0f); // Add 0 at the start

    for(int ii = 1; ii < randNum; ii += 3){
        floats_push(samples, floats_get(rSort, ii));
    }

    floats_push(samples, FLT_MAX); // and +inf at the end (or the min and max values of the type being sorted)


    free_floats(rSort);
}

void *partition_worker(void* pp) {
    long fpp = (long) pp;
    int ipp = (int) fpp;

    // Each thread builds a local array of items to be sorted
    // by scanning the full input and taking items between samples[p] and samples[p+1].

    float min = floats_get(samples, ipp);
    float max = floats_get(samples, ipp + 1);
    double start = get_time();


    floats* local_array = make_floats(2);

    for(int ii = (int) 0; ii < (int) count; ii++) {
        float element = input[ii];
        if(element >= min && element < max) {
            floats_push(local_array, element);
        }
    }

    // Sort locally
    qsort_floats(local_array);

    // Write the local size to sizes[p]
    sizes[ipp] = local_array->size;
    printf("%d: start %f, count %ld\n", ipp, start,  sizes[ipp]);
    //printf("%d sample range: %f -> %f\n", ipp, min, max);

    barrier_wait(bb); // Wait for everyone to make their local arrays
    //printf("[THREAD %d] past barrier\n", ipp);

    // Write the data out to the file.

    int jj = 0; //output index for this partition. Use SEEK_SET
    for (int ii = 0; ii < ipp; ii++) {
        jj += sizes[ii];
    }

    int f_write = open(fout_name, O_CREAT | O_TRUNC | O_RDWR, 0644);
    lseek(f_write, (jj + 1) * sizeof(float) , SEEK_CUR);

    for (int ii = 0; ii < local_array->size; ii++) {
        float element_sorted = floats_get(local_array, ii);
        write(f_write, &element_sorted, sizeof(element_sorted)); // seems to be writing random floats in random order or just zeros
        output[ii + jj] = element_sorted;
    }

    close(f_write);
    free_floats(local_array);

    return NULL;
}

int
main(int argc, char* argv[])
{
   /*** Set up ***/

   if (argc != 4) {
        printf("Usage:\n");
        printf("\t%s P input.dat output.dat\n", argv[0]);
        return 1;
    }


    P = atoi(argv[1]);
    fin_name = argv[2];
    fout_name = argv[3];

    seed_rng();

    int rv;
    struct stat st;
    rv = stat(fin_name, &st);
    check_rv(rv, "stat");

    const int fsize = st.st_size;
    if (fsize < 8) {
        printf("File too small.\n");
        return 1;
    }

    int fd = open(fin_name, O_RDWR);
    check_rv(fd, "open");

    int* f_read = mmap(0, 4096, PROT_READ|PROT_WRITE,
                      MAP_SHARED, fd, 0);
    check_rv((long)f_read, "mmap fread");

    int f_write = open(fout_name, O_CREAT | O_TRUNC | O_RDWR, 0644);
    check_rv(f_write, "output file open()");

    //write the file size to the first bytes
    float file_size = 4096;
    write(f_write, &file_size, sizeof(file_size));
    close(f_write);

    count = f_read[0];
    input = malloc(count * sizeof(float));
    output = malloc(count * sizeof(float));

    for (int ii = 1; ii < count + 1; ++ii) {
        // printf("file[%d] = %f\n", ii, (float)file[ii]);
        int idata = f_read[ii];
        float fdata = (float) idata;
        input[ii - 1] = fdata;
        output[ii - 1] = 0.0f;
    }

//     print_list_ln(input, count, "unsorted");

    /*** Sample ***/

    long sizes_bytes = P * sizeof(long);
    sizes = mmap(0, sizes_bytes, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANONYMOUS, -1, 0); // DONE: This should be shared

    fill_samples(input, count, P);

    bb = make_barrier(P);

    /*** Partition ***/

    pthread_t* part_threads;

    part_threads = malloc(P * sizeof(pthread_t));

    for (long tt = 0; tt < P; tt++) {
        pthread_create(&part_threads[tt], NULL, partition_worker, (void *) tt);
    }

    for (long tt = 0; tt < P; tt++) {
        pthread_join(part_threads[tt], NULL);
    }

//     print_list_ln(output, count, "sorted");

    free(part_threads);
    free_barrier(bb);

    rv = munmap(f_read, 4096);
    check_rv(rv, "munmap fin");
    rv = munmap(sizes, sizes_bytes);
    check_rv(rv, "munmap sizes");
    rv = close(fd);
    check_rv(rv, "close");
    free(input);

    return 0;
}

