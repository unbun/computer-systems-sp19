
#include <sys/mman.h>
#include <stdio.h>
#include <assert.h>

#include "hmalloc.h"
// #include "llist.h"


/////////////////////////////////////////////////////////////////////
////////////////////////////// xmalloc.c ////////////////////////////

/*
typedef struct hm_stats {
  long pages_mapped;
  long pages_unmapped;
  long chunks_allocated;
  long chunks_freed;
  long free_length;
} hm_stats;
*/


const size_t PAGE_SIZE = 4096;
static hm_stats stats; // This initializes the stats to 0.

static cell* free_list_head = NULL;

long
free_list_length()
{
    return count_list(free_list_head);
}

void
free_list_insert(cell* node)
{
    free_list_head = cell_insert_block(node, free_list_head);
}

hm_stats*
hgetstats()
{
    stats.free_length = free_list_length();
    return &stats;
}

void
hprintstats()
{
    stats.free_length = free_list_length();
    fprintf(stderr, "\n== husky malloc stats ==\n");
    fprintf(stderr, "Mapped:   %ld\n", stats.pages_mapped);
    fprintf(stderr, "Unmapped: %ld\n", stats.pages_unmapped);
    fprintf(stderr, "Allocs:   %ld\n", stats.chunks_allocated);
    fprintf(stderr, "Frees:    %ld\n", stats.chunks_freed);
    fprintf(stderr, "Freelen:  %ld\n", stats.free_length);
}

static
size_t
div_up(size_t xx, size_t yy)
{
    // This is useful to calculate # of pages
    // for large allocations.
    size_t zz = xx / yy;

    if (zz * yy == xx)
    {
        return zz;
    }
    else
    {
        return zz + 1;
    }
}

void*
xmalloc(size_t size)
{
    stats.chunks_allocated += 1;
    size += sizeof(size_t);

    // Use the start of the block to store its size.
    // Return a pointer to the block after the size field.
    void* new_bstart;
    size_t new_bsize;

    // Requests with (B < 1 page = 4096 bytes)
    if (size < PAGE_SIZE)
    {
        //See if there’s a big enough block on the free list. If so, select the first one ...
        cell* node = xmallocHlp_get_free_block(size);

        //  ... and remove it from the list.
        if (node != NULL)
        {
            new_bstart = (void*)node;
            new_bsize = node->curr_size;
        }
        else // If you don’t have a block, mmap a new block (1 page)
        {
            new_bsize = PAGE_SIZE;
            new_bstart = mmap(NULL, new_bsize, PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            assert(new_bstart != 0);
            stats.pages_mapped += 1;
        }

        // If the block is bigger than the request, and the leftover is big enough to
        // store a free list cell, return the extra to the free list.
        if (new_bsize - size > sizeof(cell))
        {
            cell* new_block = (cell*)(new_bstart + size);
            new_block->curr_size = new_bsize - size;
            free_list_insert(new_block);
            new_bsize = size;
        }
    }
    else // Requests with (B >= 1 page = 4096 bytes):
    {
        size_t num_pages = div_up(size, PAGE_SIZE); // Calculate the number of pages needed for this block.
        new_bsize = PAGE_SIZE * num_pages; // // Allocate that many pages
        new_bstart = mmap(NULL, new_bsize, PROT_READ | PROT_WRITE, // with mmap
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

        assert(new_bstart != 0);
        stats.pages_mapped += num_pages;
    }
    
    *((size_t*)new_bstart) = new_bsize;
    return new_bstart + sizeof(size_t);
}

// See if there’s a big enough block on the free list. If so, select the first one, remove it from the list, and return int
// if not, return null
cell*
xmallocHlp_get_free_block(size_t min_size)
{
    if (free_list_head == NULL)
    {
        return NULL;
    }

    cell* nn = free_list_head;

    //head is big enough to use
    if (free_list_head->curr_size >= min_size)
    {
        free_list_head = free_list_head->rest;
        return nn;
    }

    cell* pp;

    //iterate through the rest of the nodes
    do
    {
        pp = nn; // need to update prev b4 iterating
        nn = nn->rest;

        //stop if end of list OR found block big enough
    } while (nn != NULL && nn->curr_size < min_size);

    if (nn != NULL) // didn't reach end of list
    {
        pp->rest = nn->rest;
    }

    return nn;
}

void
xfree(void* item)
{
    stats.chunks_freed += 1;

    void* bstart = item - sizeof(size_t);
    size_t bsize = *((size_t*)bstart);

    // If the block is < 1 page
    if (bsize < PAGE_SIZE)
    {
        free_list_insert((cell*)bstart); // then stick it on the free list.
    }
    else
    {
        int rv = munmap(bstart, bsize); // then munmap it.
        assert(rv == 0);
        stats.pages_unmapped += bsize / PAGE_SIZE;
    }
}

