// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "barrier.h"

barrier*
make_barrier(int nn)
{
    barrier* bb = malloc(sizeof(barrier));
    assert(bb != 0);
    assert(nn != 0);

    if(pthread_mutex_init(&bb->mutex, 0) < 0)
    {
        perror("mutex init");
        abort();
    }

    if(pthread_cond_init(&bb->cond, 0) < 0)
    {
        pthread_mutex_destroy(&bb->mutex);
        perror("cond init");
        abort();
    }

    bb->count = 0;
    bb->seen  = nn;

    return bb;
}

void
barrier_wait(barrier* bb)
{
    pthread_mutex_lock(&bb->mutex);
    ++(bb->count);
    
    if(bb->count >= bb->seen)
    {
        bb->count = 0;
        pthread_cond_broadcast(&bb->cond);
        pthread_mutex_unlock(&bb->mutex);
        return;
    } else {
        pthread_cond_wait(&bb->cond, &(bb->mutex));
        pthread_mutex_unlock(&bb->mutex);
        return;
    }
}

void
free_barrier(barrier* bb)
{
    pthread_cond_destroy(&bb->cond);
    pthread_mutex_destroy(&bb->mutex);
    free(bb);
}

