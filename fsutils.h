#ifndef __FSUTILS_H__
#define __FSUTILS_H__

#include <stdio.h>
#include <stdlib.h>

extern const int block_size;
extern const char magic_number[];
extern const char end_meta[];

void init_metadata(FILE *fp, char is_raid0);

#endif
