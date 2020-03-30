// Author: Nat Tuck
// CS3650 starter code

#ifndef UTILS_H
#define UTILS_H

#include "float_vec.h"

void seed_rng();
void check_rv(int rv, char* name);
double get_time();
int ints_contains(int* arr, int val, int size);
void copy_rand_values(float* src, floats* dest, int nCopy);
void print_list(float *ll, int size, char *name);
void print_list_ln(float *ll, int size, char *name);

#endif

