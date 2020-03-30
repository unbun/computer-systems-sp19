#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"
#include "barrier.h"


// Synchronization tools
#define BARRIER_COUNT 1000
pthread_barrier_t barrier;

// Function headers
void Usage(char* prog_name);
void Print_items(int *l, int size, char *name);
int Is_used(int seed, int offset, int range);
int Int_comp(const void * a,const void * b);
void *Thread_work(void* rank);

// Global variables
int i, PP, sample_size, NN, suppress_output;
int *items, *sample_keys, *sorted_keys, *sizes, *tmp_items, *sorted_items;
int *raw_dist, *prefix_dist, *col_dist, *prefix_col_dist;
char *input_file;


/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

  fprintf(stderr, "Usage: %s [number of threads] [sample size] [items size] [name of input file] [Optional suppress output(n)]\n", prog_name);
  exit(0);
}  /* Usage */



/*--------------------------------------------------------------------
 * Function:    Print_items
 * Purpose:     Print items in formatted fashion
 * In arg:      l, size, name
 */
void Print_items(int *l, int size, char *name) {
    printf("\n======= %s =======\n", name);
    for (i = 0; i < size; i++) {
    	  printf("%d ", l[i]);
    }
    printf("\n");
}  /* Print_items */



/*--------------------------------------------------------------------
 * Function:    Is_used
 * Purpose:     Check if the random seeded key is already selected in sample
 * In arg:      seed, offset, range
 */
int Is_used(int seed, int offset, int range) {
  int i;
	for (i = offset; i < (offset + range); i++) {
		if (sample_keys[i] == items[seed]) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
} /* Is_used */



/*--------------------------------------------------------------------
 * Function:    Int_comp
 * Purpose:     Comparison function for integer, used by qsort
 * In arg:      a, b
 */
int Int_comp(const void * a,const void * b) {
    int va = *(const int*) a;
    int vb = *(const int*) b;
    return (va > vb) - (va < vb);
}



/*-------------------------------------------------------------------
 * Function:    Thread_work
 * Purpose:     Run BARRIER_COUNT barriers
 * In arg:      rank
 * Global var:  barrier
 * Return val:  Ignored
 */
void *Thread_work(void* rank) {
  long my_rank = (long) rank;
  int i, j, seed, index, offset, local_chunk_size, local_sample_size;
  int local_pointer, s_index, my_segment, col_sum;
  int *local_data;

  local_chunk_size = NN / PP;
  local_sample_size = sample_size / PP;
  
  // printf("Hi this is thread %ld, I have %d chunks and should do %d samples. \n", my_rank, local_chunk_size, local_sample_size);
  
  // Get sample keys randomly from original items
  srandom(my_rank + 1);  
  offset = my_rank * local_sample_size;
  
  for (i = offset; i < (offset + local_sample_size); i++) {
	  do {
		  // If while returns 1, you'll be repeating this
		  seed = (my_rank * local_chunk_size) + (random() % local_chunk_size);
	  } while (Is_used(seed, offset, local_sample_size));
	  // If the loop breaks (while returns 0), data is clean, assignment
	  sample_keys[i] = items[seed];
	  index = offset + i;
	  
	  // printf("T%ld, seed = %d\n", my_rank, seed);
	  // printf("T%ld, index = %d, i = %d, key = %d, LCS = %d\n\n", my_rank, index, i, items[seed], local_sample_size);
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Parallel count sort the sample keys
  for (i = offset; i < (offset + local_sample_size); i++) {
	  int mykey = sample_keys[i];
	  int myindex = 0;
	  for (j = 0; j < sample_size; j++) {
		  if (sample_keys[j] < mykey) {
			  myindex++;
		  } else if (sample_keys[j] == mykey && j < i) {
			  myindex++;
		  } else {
		  }
	  }
	  // printf("##### P%ld Got in FINAL, index = %d, mykey = %d, myindex = %d\n", my_rank, i, mykey, myindex);
	  sorted_keys[myindex] = mykey;
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Besides thread 0, every thread generates a splitter
  // sizes[0] should always be zero
  if (my_rank != 0) {
	  sizes[my_rank] = (sorted_keys[offset] + sorted_keys[offset-1]) / 2;
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);

  // Using block partition to retrieve and sort local chunk
  local_pointer = my_rank * local_chunk_size;
  local_data = malloc(local_chunk_size * sizeof(int));

  j = 0;
  for (i = local_pointer; i < (local_pointer + local_chunk_size); i++) {  
	  local_data[j] = items[i];
	  j++;
  }
  
  // Quick sort on local data before splitting into buckets
  qsort(local_data, local_chunk_size, sizeof(int), Int_comp);
  
  // index in the splitter array
  s_index = 1;	
  // starting point of this thread's segment in dist arrays
  my_segment = my_rank * PP; 
  
  // Generate the original distribution array, loop through each local entry
  for (i = 0; i < local_chunk_size; i++) {
	  if (local_data[i] < sizes[s_index]) {
		  // If current elem lesser than current splitter
		  // That means it's within this bucket's range, keep looping
	  } else {
		  // Elem is out of bucket's range, time to increase splitter
		  // Keep increasing until you find one that fits
		  // Also make sure if equals we still increment
		  while (s_index < PP && local_data[i] >= sizes[s_index]) {
			  s_index++;
		  }
	  }
	  // Add to the raw distribution array, -1 because splitter[0] = 0
	  raw_dist[my_segment + s_index-1]++;
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Generate prefix sum distribution array 
  // (NOTE: does not need to wait for the whole raw_dist to finish, thus no barrier)
  // For the specific section that this thread is in charge of...
  // +1 initially because we don't process the first element at all
  for (i = my_segment; i < (my_segment + PP); i++) {
	  if (i == my_segment) {
		  prefix_dist[i] = raw_dist[i];	 
		  // printf("Thread %ld ### i = %d, prefix_dist[i] = %d, raw_dist[i] = %d\n", my_rank, i, prefix_dist[i], raw_dist[i]); 	
	  } else {
		  prefix_dist[i] = raw_dist[i] + prefix_dist[i - 1];
		  // printf("Thread %ld ### i = %d, prefix_dist[i] = %d, raw_dist[i] = %d , raw_dist[i-1] = %d\n", my_rank, i, prefix_dist[i], raw_dist[i], raw_dist[i - 1]);
	  }
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Generate column distribution array 
  // For the specific section that this thread is in charge of...
  // +1 initially because we don't process the first element at all
  for (i = my_segment; i < (my_segment + PP); i++) {
	  if (i == my_segment) {
		  prefix_dist[i] = raw_dist[i];	 
		  // printf("Thread %ld ### i = %d, prefix_dist[i] = %d, raw_dist[i] = %d\n", my_rank, i, prefix_dist[i], raw_dist[i]); 	
	  } else {
		  prefix_dist[i] = raw_dist[i] + prefix_dist[i - 1];
		  // printf("Thread %ld ### i = %d, prefix_dist[i] = %d, raw_dist[i] = %d , raw_dist[i-1] = %d\n", my_rank, i, prefix_dist[i], raw_dist[i], raw_dist[i - 1]);
	  }
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Generate column sum distribution, each thread responsible for one column
  col_sum = 0;
  for (i = 0; i < PP; i++) {
	  col_sum += raw_dist[my_rank + i * PP];
  }
  col_dist[my_rank] = col_sum;
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Generate prefix column sum distribution, each thread responsible for one column
  // This step is very risky to conduct parallelly, I decided to not do that
  if (my_rank == 0) {
	  for (i = 0; i < PP; i++) {
		  if (i == 0) {
		  	prefix_col_dist[i] = col_dist[i];
		  } else {
		  	prefix_col_dist[i] = col_dist[i] + prefix_col_dist[i - 1];
		  }
	  }
  }
  
  // Reassemble the partially sorted items, prepare for retrieval
  for (i = 0; i < local_chunk_size; i++) {
	  tmp_items[local_pointer + i] = local_data[i];
  }
  
  // Ensure all threads have reached this point, and then let continue
  pthread_barrier_wait(&barrier);
  
  // Reassemble each thread's partially sorted items based on buckets
  // Allocate an array based on the column sum of this specific bucket
  int my_first_D = col_dist[my_rank];
  int *my_D = malloc(my_first_D * sizeof(int));
  printf("~~~ Thread %ld got here, my_first_D = %d\n", my_rank, my_first_D);
  
  int b_index = 0;
  // int i_manual = 0;
  // For each thread in the column...
  for (i = 0; i < PP; i++) {
	  // offset = i * local_chunk_size + prefix_dist[i, my_rank-1];
	  // offset = (i_manual * local_chunk_size) + prefix_dist[i*PP + my_rank-1];
	  
	  if (my_rank == 0) {
		  offset = (i * local_chunk_size);
		  printf("@@@ Thread %ld, prefix_dist = %d, i = %d, offset = %d\n", my_rank, prefix_dist[i*PP + my_rank-1], i, offset);	  	
	  } else {
	  	  offset = (i * local_chunk_size) + prefix_dist[i*PP + my_rank-1];
	  }
	  
	  if (raw_dist[i*PP + my_rank] != 0) {
		  // If this row doesn't have anything belong to this bucket
		  // Do not increase i_manual
		  // i_manual++;
		  for (j = 0; j < raw_dist[i*PP + my_rank]; j++) {
			  if (my_rank == 0) {
				  printf("### Thread %ld, raw_index = %d, b_index = %d, offset = %d, j = %d, offset+j = %d, elem = %d\n", my_rank, raw_dist[i*PP + my_rank], b_index, offset, j, offset + j, tmp_items[offset + j]);
			  }
			  my_D[b_index] = tmp_items[offset + j];
			  b_index++;
		  }
		  
	  } 
  }
  // Quick sort on local bucket
  qsort(my_D, my_first_D, sizeof(int), Int_comp);
  // Print_items(my_D, my_first_D, "Thread items");
  
  
  // Ensure all threads have reached this point, and then let continue
  // pthread_barrier_wait(&barrier);
  
  // Merge thread bucket data into final sorted items
  if (my_rank == 0) {
	  for (i = 0; i < my_first_D; i++) {
	  // printf("~~~ Thread %ld, sorted_items[%d] = %d\n", my_rank, i, my_D[i]);
		  sorted_items[i] = my_D[i];
	  }
  } else {
	  offset = prefix_col_dist[my_rank-1];
	  for (i = 0; i < my_first_D; i++) {
		  // printf("~~~ Thread %ld, offset = %d, sorted_items[%d] = %d\n", my_rank, offset, offset+i, my_D[i]);
		  sorted_items[offset + i] = my_D[i];
	  }
  }
  
  return NULL;
}  /* Thread_work */



/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
  long thread;
  pthread_t* mthread; 
  double start, finish;

  suppress_output = 0;
  // for (int i = 0; i < argc; ++i){
  //   printf("Command line args === argv[%d]: %s\n", i, argv[i]);
  // }  

  if (argc == 5) { 
  } else if (argc == 6 && (strcmp(argv[5], "n") == 0)) {
	  // printf("==== %s\n", argv[5]);
	  suppress_output = 1;
  } else {
	Usage(argv[0]);
  }
  
  PP = strtol(argv[1], NULL, 10);
  sample_size = 3 * (PP - 1);
  NN = strtol(argv[3], NULL, 10);
  input_file = argv[4];

  // Allocate memory for variables
  mthread = malloc(PP*sizeof(pthread_t));
  items = malloc(NN * sizeof(int));
  tmp_items = malloc(NN * sizeof(int));
  sorted_items = malloc(NN * sizeof(int));
  sample_keys = malloc(sample_size * sizeof(int));
  sorted_keys = malloc(sample_size * sizeof(int));
  sizes = malloc(PP * sizeof(int));
  
  // One dimensional distribution arrays
  raw_dist = malloc(PP * PP * sizeof(int));
  col_dist = malloc(PP * sizeof(int));
  prefix_dist = malloc(PP * PP * sizeof(int));
  prefix_col_dist = malloc(PP * sizeof(int));
  

  pthread_barrier_init(&barrier, NULL, PP);
  

  // Read items content from input file
  FILE *fp = fopen(input_file, "r+");
  for (i = 0; i < NN; i++) {
  	  if (!fscanf(fp, "%d", &items[i])) {
    	  break;
      }
  }
  Print_items(items, NN, "original items");
  
  start = get_time();
  
  for (thread = 0; thread < PP; thread++)
     pthread_create(&mthread[thread], NULL,
         Thread_work, (void*) thread);

  for (thread = 0; thread < PP; thread++) 
     pthread_join(mthread[thread], NULL);
  
  finish = get_time();
  
  // Print_items(sample_keys, sample_size, "Sample keys (unsorted)");
  Print_items(sorted_keys, sample_size, "Sample keys (sorted)");
  Print_items(sizes, PP, "sizes");
  Print_items(raw_dist, PP * PP, "Raw dist");
  Print_items(prefix_dist, PP * PP, "Prefix dist");
  Print_items(col_dist, PP, "Colsum dist");
  Print_items(prefix_col_dist, PP, "Prefix colsum dist");
  Print_items(tmp_items, NN, "Temp items");
  
  // Only print items data if not suppressed
  if (suppress_output == 0) {
	  Print_items(sorted_items, NN, "Sorted items");
  }
  
  // Print elapsed time regardless
  printf("Elapsed time = %e seconds\n", finish - start);


  pthread_barrier_destroy(&barrier);
  // pthread_mutex_destroy(&barrier_mutex);
  // pthread_cond_destroy(&ok_to_proceed);

  free(mthread);
  
  return 0;
}  /* main */