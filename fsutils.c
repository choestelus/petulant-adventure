#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

const char magic_number[] = "E1";
const int block_size = 64;
const char end_meta[] = "END";

gint64 blocks_available(gint64 size)
{
    /* from free space available*/
    gint64 no_blocks1 = (gint64)( (gint64)(0.8*size) / (gint64)block_size);
    /* from file metadata available; 30 is disk metadata size in bytes */
    gint64 no_blocks2 = (gint64)((gint64)(0.2*size) - 30 - no_blocks1) / 29;

    return (no_blocks1 < no_blocks2)?no_blocks1:no_blocks2;
}

void init_metadata(FILE *fp, int _is_raid0)
{

    char is_raid0 = (char)_is_raid0;
    fseeko(fp, 0, SEEK_END);
    gint64 disk_size = (gint64)ftello(fp);
    gint64 no_of_blocks = blocks_available(disk_size);
    char enabled_raid0 = is_raid0;
    char raid0_number = 42;
    char consistence = 1;

    rewind(fp);

    fwrite(magic_number, sizeof(char), sizeof(magic_number)/sizeof(char), fp);
    fwrite(&block_size, sizeof(int), sizeof(block_size)/sizeof(int), fp);
    fwrite(&disk_size, sizeof(gint64), sizeof(disk_size)/sizeof(gint64), fp);
    fwrite(&no_of_blocks, sizeof(gint64), sizeof(no_of_blocks)/sizeof(gint64), fp);
    fwrite(&enabled_raid0, sizeof(char), sizeof(enabled_raid0)/sizeof(char), fp);
    fwrite(&raid0_number, sizeof(char), sizeof(raid0_number)/sizeof(char), fp);
    fwrite(&consistence, sizeof(char), sizeof(consistence)/sizeof(char), fp);
    fwrite(end_meta, sizeof(char), sizeof(end_meta)/sizeof(char), fp);
    fflush(fp);

    /* free space management */
    char *free_space_bytemap = (char*)calloc(sizeof(char), no_of_blocks);
    fwrite(free_space_bytemap, sizeof(char), no_of_blocks, fp);
    fflush(fp);
    free(free_space_bytemap);
}
gint64 get_disk_size(FILE *fp)
{
    gint64 ds = -1;
    rewind(fp);
    fseeko(fp, 7, SEEK_SET);
    fread(&ds, sizeof(gint64), sizeof(ds)/sizeof(gint64), fp);
    return ds;
}
gint64 get_no_of_blocks(FILE *fp)
{
    gint64 nob = -1;
    rewind(fp);
    fseeko(fp, 15, SEEK_SET);
    fread(&nob, sizeof(gint64), sizeof(nob)/sizeof(gint64), fp);
    return nob;
}
gint64 get_file_meta_offset(FILE *fp)
{
    return 30+get_no_of_blocks(fp);
}

int put_file(const char *key, const char *src, FILE *fp)
{
    // fp is pre-seeked to destination offset
    FILE *fsrc = fopen(src, "rb");
    fseeko(fsrc, 0, SEEK_END);
    guint size = (guint)ftello(fsrc);
    time_t atime = time(NULL);
    gint64 offset = (gint64)ftello(fp);

    rewind(fsrc);
    fwrite(key, sizeof(char), strlen(key), fp);
    fwrite(&atime, sizeof(time_t), 1, fp);
    fwrite(&offset, sizeof(gint64), 1, fp);
    fwrite(&size, sizeof(guint), 1, fp);
    fflush(fp);

    guchar buffer[block_size];
    gint64 bytes_written = 0;
    while(!feof(fsrc))
    {
        int read_bytes = fread(buffer, sizeof(guchar), block_size, fsrc);
        fwrite(buffer, sizeof(guchar), read_bytes, fp);
        bytes_written += read_bytes;
        fflush(fp);
    }
    fclose(fsrc);

    return bytes_written;
}

