// based on cs3650 starter code

#ifndef DIRECTORY_HINTS_H
#define DIRECTORY_HINTS_H

#define DIR_NAME 48

#include "slist.h"
#include "pages.h"
#include "inode_HINTS.h"

typedef struct dirent {
    char name[DIR_NAME];
    int  inum;
    char _reserved[12];
} dirent;

void directory_init();
int directory_lookup(inode_h* dd, const char* name);
int tree_lookup(const char* path);
int directory_put(inode_h* dd, const char* name, int inum);
int directory_delete(inode_h* dd, const char* name);
slist* directory_list(const char* path);
void print_directory(inode_h* dd);

#endif

