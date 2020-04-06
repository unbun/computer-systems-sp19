#include <stdio.h>
#include "bitmap.h"


int 
bitmap_get(void* bm, int ii) {
	int bb = ii >> 6;
  	int cc = ii & 63;	

  	int* ibm = (int*)bm;

  	return (ibm[bb] >> cc) & 1;
}

void 
bitmap_put(void* bm, int ii, int vv) {
	int bb = ii >> 6;
  	int cc = ii & 63;	

  	int* ibm = (int*)bm;


  	if (vv == 0) {
  		ibm[bb] &= ~(1 << cc);
  	} else {
  		ibm[bb] |= 1 << cc;
  	}
}

void 
bitmap_print(void* bm, int size) {
	printf("========== bitmap [%d] ==========\n", size);

	for (int ii = 0; ii < size; ii++) {
		int element = bitmap_get(bm, ii);
		printf("[%d] = %d\n", ii, element);
	}

	printf("=================================\n");
}	
