#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>

const char magic_number[] = "E1";
const int block_size = 64;
const char end_meta[] = "END";
void init_metadata(FILE *fp, char is_raid0)
{

    fseeko(fp, 0, SEEK_END);
    gint64 disk_size = (gint64)ftello(fp);
    gint64 no_of_blocks = 4200;
    char enabled_raid0 = is_raid0+42;
    char raid0_number = 42;

    rewind(fp);

    fwrite(magic_number, sizeof(char), sizeof(magic_number)/sizeof(char), fp);
    fwrite(&block_size, sizeof(int), sizeof(block_size)/sizeof(int), fp);
    fwrite(&no_of_blocks, sizeof(gint64), sizeof(no_of_blocks)/sizeof(gint64), fp);
    fwrite(&disk_size, sizeof(gint64), sizeof(disk_size)/sizeof(gint64), fp);
    fwrite(&enabled_raid0, sizeof(char), sizeof(enabled_raid0)/sizeof(char), fp);
    fwrite(&raid0_number, sizeof(char), sizeof(raid0_number)/sizeof(char), fp);
    fwrite(end_meta, sizeof(char), sizeof(end_meta)/sizeof(char), fp);
    fflush(fp);
}
