#ifndef __FSUTILS_H__
#define __FSUTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

extern const int block_size;
extern const char magic_number[];
extern const char end_meta[];

void init_metadata(FILE *fp, char is_raid0);
gint64 blocks_available(gint64 size);
gint64 get_disk_size(FILE *fp);
gint64 get_no_of_blocks(FILE *fp);
gint64 get_file_meta_offset(FILE *fp);

int put_file(const char *key, const char *src, FILE *fp);
#endif
