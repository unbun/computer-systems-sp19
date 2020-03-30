
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

// See "man strlcpy"
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


// using pair functions from lecture
void
free_pair(hashmap_pair* pp)
{
    if (pp) {
        free(pp->key);
        free(pp);
    }
}

int
hash(char* key)
{
    return strlen(key) * 67;
}

hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = calloc(1, sizeof(hashmap));
    hh->capacity = nn;
    hh->size = 0;
    hh->data = calloc(nn, sizeof(hashmap_pair*));
    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}

void
free_hashmap(hashmap* hh)
{
    for (size_t ii = 0; ii < hh->capacity; ++ii) {
        if (hh->data[ii]) {
            free_pair(hh->data[ii]);
        }
    }
    free(hh->data);
    free(hh);
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

int
hashmap_get(hashmap* hh, char* kk)
{
    long ii = hash(kk) & (hh->capacity - 1);
    assert(ii >= 0 && ii < hh->size);
    return hh->data[ii]->val;
}

void
hashmap_put(hashmap* hh, char* kk, int vv)
{
    if (hh->size >= hh->capacity) {
        hashmap_grow(hh);
    }

    long ii = hash(kk) & (hh->capacity - 1);
    hh->data[ii] = calloc(hh->capacity, sizeof(hashmap_pair*));
    hh->size += 1;
}

void
hashmap_del(hashmap* hh, char* kk)
{
    long ii = hash(kk) & (hh->capacity - 1);
    hashmap_pair** ref = &(hh->data[ii]);
    hashmap_pair* curr = *ref;
    free_pair(curr);
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    assert(ii >= 0 && ii < hh->size);
    hashmap_pair* ref = hh->data[ii];
    return *ref;
}

void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    for (size_t ii = 0; ii < hh->capacity; ++ii) {
        hashmap_pair pair = hashmap_get_pair(hh, ii);
        if (hh->data[ii]) {
            printf("%s, %d\n", pair.key, pair.val);
        }
    }
}

void
hashmap_grow(hashmap* hh)
{
    size_t nn = hh->capacity;
    hashmap_pair** data = hh->data;

    hh->capacity = 2 * nn;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair*));
}
