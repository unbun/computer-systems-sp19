/* This file is modifed from lecture notes */

#ifndef SVEC_H
#define SVEC_H

typedef struct svec {
    int size;
    int cap;
    char** data;
} svec;

svec* make_svec();
void  free_svec(svec* sv);

char* svec_get(svec* sv, int ii);
void  svec_put(svec* sv, int ii, char* item);

void svec_push_back(svec* sv, char* item);
void svec_swap(svec* sv, int ii, int jj);

int svec_print(svec* sv);

int svec_contains(svec* sv, char* item, int item_size);
int svec_find(svec* sv, char* item, int item_size);
#endif
