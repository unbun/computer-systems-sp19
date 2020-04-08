#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>

void pages_init(const char* path, int create);
void pages_free();
void* pages_get_page(int pnum);

typedef struct superblock {
    void* data_bm;
    void* inode_map;
    inode* inodes;
    void* data_start;
    int num_inodes;
    int num_free_pages;
    int root_inodes;
};

#endif
