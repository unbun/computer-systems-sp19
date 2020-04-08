// based on cs3650 starter code

#ifndef INODE_H
#define INODE_H

#include "pages.h"

typedef struct inode_h {
    int refs; // reference count
    int mode; // permission & type
    int size; // bytes
    int ptrs[2]; // direct pointers
    int iptr; // single indirect pointer
} inode_h;

void print_inode(inode_h* node);
inode_h* get_inode(int inum);
int alloc_inode();
void free_inode();
int grow_inode(inode_h* node, int size);
int shrink_inode(inode_h* node, int size);
int inode_get_pnum(inode_h* node, int fpn);

#endif