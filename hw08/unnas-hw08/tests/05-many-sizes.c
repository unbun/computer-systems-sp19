
#include <stdio.h>
#include <string.h>

#include "hmalloc.h"
#include "tools.h"

int
main(int _ac, char* _av[])
{
    int** xs = xmalloc(1000 * sizeof(int *));

    for (int ii = 0; ii < 1000; ++ii) {
        xs[ii] = xmalloc(smax(5 * ii, sizeof(int)));
        memset(xs[ii], 1, 5 * ii);
        *xs[ii] = ii;
    }

    int sum = 0;

    for (int ii = 0; ii < 1000; ++ii) {
        sum += *xs[ii];
        xfree(xs[ii]);
    }

    printf("Sum = %d\n", sum);

    xfree(xs);

    hprintstats();
    return 0;
}



