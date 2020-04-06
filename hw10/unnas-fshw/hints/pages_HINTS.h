// based on cs3650 starter code

#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>

#define NUM_PAGES 256
#define BLOCK_SIZE 4096 // 4 KB

void   pages_init(const char* path);
void   pages_free();
void*  pages_get_page(int pnum);
void*  get_pages_bitmap();
void*  get_inode_bitmap();
int    alloc_page();
void   free_page(int pnum);

// not exactly a super_block but this represents the first two blocks
// of the drive shown in lecture. This cuts up the file system
// into:
// - bitmaps(boolean arrays) for seeing if pages/inodes are written
// - the inodes array, and any relevant data for the inodes
// - ref to the data
//		- including the root
typedef struct extfs_head {
    int data_map[NUM_PAGES]; // Each int = true(1) or false(0)
    int inode_map[NUM_PAGES]; // Each int = 1 inode, probably don't need all 256 of them

    inode* inodes;
    int num_inodes;

    int root_inode;
    void* data_start;
    int num_free_pages; // This might be removed later


} extfs_head;

#endif
