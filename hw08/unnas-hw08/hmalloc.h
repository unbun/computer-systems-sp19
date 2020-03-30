#ifndef HMALLOC_H
#define HMALLOC_H

#include <stddef.h>
//#include "xmalloc.h"


typedef struct cell {
    size_t curr_size;     // size of one block
    long         item;
    struct cell* rest;
} cell;

/////////////////////////////////////////////////////////////////////
////////////////////////////// xmalloc.h //////////////////////////////

typedef struct hm_stats {
    long pages_mapped;
    long pages_unmapped;
    long chunks_allocated;
    long chunks_freed;
    long free_length;
} hm_stats;

hm_stats* hgetstats();
void hprintstats();
void* xmalloc(size_t size);
void xfree(void* item);
cell* xmallocHlp_get_free_block(size_t min_size);

//////////////////////////////////////////////////////////////////
/////////////////// list.h///////////////////////////////////////


static
cell*
cons(long item, cell* rest)
{
    cell* xs = xmalloc(sizeof(cell));
    xs->item = item;
    xs->rest = rest;
    return xs;
}

static
long
count_list(cell* xs)
{
    long nn = 0;
    while (xs) {
        nn++;
        xs = xs->rest;
    }
    return nn;
}

static
void
free_list(cell* xs)
{
    while (xs) {
        cell* ys = xs->rest;
        xfree(xs);
        xs = ys;
    }
}

static
cell*
copy_list(cell* xs)
{
    if (xs == 0) {
        return 0;
    }

    cell* ys = copy_list(xs->rest);
    return cons(xs->item, ys);
}

///////////////////////////////////////
/////// From HW08 /////////////////////
///////////////////////////////////////

static cell*
cell_insert_block(cell* to_insert, cell* list_head)
{
    if (list_head == NULL)
    {
        to_insert->rest = NULL;
        return to_insert;
    }

    // inserting before (or at if the size is big enough) the head
    if (to_insert < list_head)
    {
        // Any two adjacent blocks on the free list get coalesced (joined together) into one bigger block.
        // This is the special case where the head needs to be coalesced, by something in front of it
        if ((void*)to_insert + to_insert->curr_size == list_head)
        {
            //at head, in front
            to_insert->curr_size += list_head->curr_size; // add the list to the node
            to_insert->rest = list_head->rest; // replace the head with the node
            return to_insert;
        }

        // replace the head with the node, but keep the head (don't coalesce)
        to_insert->rest = list_head;
        return to_insert;
    }

    // Any two adjacent blocks on the free list get coalesced (joined together) into one bigger block.
    if ((void*)list_head + list_head->curr_size == to_insert)
    {
        //at head, behind
        list_head->curr_size += to_insert->curr_size;
        return cell_insert_block(list_head, list_head->rest); // head is now bigger, need to shift the list down
    }

    // insert after head
    list_head->rest = cell_insert_block(to_insert, list_head->rest);
    return list_head;
}
// void print_list(llist_node* list_head); // was giving clang errors and wasn't needed




#endif
