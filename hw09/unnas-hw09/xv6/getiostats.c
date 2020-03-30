#include "types.h"
#include "stat.h"

int
main(int _ac, char *_av[])
{
	return 
		//getiostat(void); OR
		//getiostat(int fd, struct iostats* stats);
}

int
getiostat(struct file *f, struct iostats *st)
{
	if(f->type == FD_INODE){
	    ilock(f->ip);
	    stati(f->ip, st);
	    iunlock(f->ip);
	    return 0;
  	}
	return -1;
}