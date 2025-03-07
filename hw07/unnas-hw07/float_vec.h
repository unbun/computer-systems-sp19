// Author: Nat Tuck
// CS3650 starter code

#ifndef FLOAT_VEC_H
#define FLOAT_VEC_H

typedef struct floats {
    long size;
    long cap;
    float* data;
} floats;

floats* make_floats(long nn);
void floats_push(floats* xs, float xx);
void floats_put(floats* xs, int ii, float item);
void free_floats(floats* xs);
void floats_print(floats* xs);
float floats_get(floats* xs, int ii);
int floats_contains(floats* xs, float xx);

#endif
