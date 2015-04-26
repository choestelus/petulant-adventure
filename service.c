#include "rfos.h"
#include "fsutils.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

char ***disk_array;
int disk_number;

static gboolean on_handle_get (
    RFOS *object,
    GDBusMethodInvocation *invocation,
    const gchar *key,
    const gchar *outpath) {

    /** Your Code for Get method here **/
    guint err = 0;
    /** End of Get method execution, returning values **/
    rfos_complete_get(object, invocation, err);
    return TRUE;
}

static gboolean on_handle_put (
    RFOS *object,
    GDBusMethodInvocation *invocation,
    const gchar *key,
    const gchar *src) {

    guint err = 0;

    FILE **fpd;
    fpd = (FILE**)malloc(sizeof(FILE*)*disk_number);
    int i = 0;
    for(i=0; i< disk_number-1; i++)
    {
        fpd[i] = fopen((*disk_array)[i+1], "rb+");
    }
    fprintf(stderr, "%s\n", "still works");
    /** Your code for Put method here **/
    // gint64 bytes_written1 = put_file(key, src, fp1);
    // gint64 bytes_written2 = put_file(key, src, fp2);
    for(i = 0; i<disk_number -1; i++)
    {
        fprintf(stderr, "%s\n", "sitll works in write loop");
        gint64 bytes_written = put_file(key, src, fpd[i]);
        fprintf(stderr, "written: %lld\n", (long long int)bytes_written);
    }

    err = 0;
    /** End of Put method execution, returning values **/
    rfos_complete_put(object, invocation, err);
    for(i=0; i< disk_number -1; i++)
        fclose(fpd[i]);
    free(fpd);

    return TRUE;
}

static void on_name_acquired (GDBusConnection *connection,
    const gchar *name,
    gpointer user_data)
{
    /* Create a new RFOS service object */
    RFOS *skeleton = rfos_skeleton_new ();
    /* Bind method invocation signals with the appropriate function calls */
    g_signal_connect (skeleton, "handle-get", G_CALLBACK (on_handle_get), NULL);
    g_signal_connect (skeleton, "handle-put", G_CALLBACK (on_handle_put), NULL);
    /* Export the RFOS service on the connection as /kmitl/ce/os/RFOS object  */
    g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (skeleton),
        connection,
        "/kmitl/ce/os/RFOS",
        NULL);
}

void init_disk(const char *disk_name, int tag)
{
    FILE *fp;
    fp = fopen(disk_name, "rb+");
    if(fp == NULL)
    {
        fprintf(stderr, "fopen failed with errno %d\n", errno);
        perror("Error description: ");
        exit(errno);
    }

    init_metadata(fp, tag);
    // gint64 ds, nob;
    // int bs;
    // char enr0, r0no;
    // char mgno[3], endmt[4];

    // rewind(fp);
    // fseeko(fp, 0, SEEK_SET);
    // fread(mgno, sizeof(char), sizeof(mgno)/sizeof(char), fp);
    // rewind(fp);
    // fseeko(fp, 3, SEEK_SET);
    // fread(&bs, sizeof(int), sizeof(bs)/sizeof(int), fp);
    // fseeko(fp, 7, SEEK_SET);
    // fread(&nob, sizeof(gint64), sizeof(nob)/sizeof(gint64), fp);
    // fseeko(fp, 15, SEEK_SET);
    // fread(&ds, sizeof(gint64), sizeof(nob)/sizeof(gint64), fp);
    // fseeko(fp, 23, SEEK_SET);
    // fread(&enr0, sizeof(char), sizeof(enr0)/sizeof(char), fp);
    // fseeko(fp, 24, SEEK_SET);
    // fread(&r0no, sizeof(char), sizeof(r0no)/sizeof(char), fp);
    // fseeko(fp, 25, SEEK_SET);
    // fread(endmt, sizeof(char), sizeof(endmt)/sizeof(char), fp);

    // printf("read:\n");
    // printf("%s %d %lld %lld %d %d %s\n", mgno, bs, (long long int)nob, (long long int)ds, enr0, r0no, endmt);

    fclose(fp);

    fprintf(stderr, "%s\n", "disk initialized");
}

int main (int argc, char** argv)
{
    disk_number = argc;
    disk_array = &argv;
    int i;
    for(i=1; i < argc; i++)
    {
        init_disk(argv[i], 0);
    }

    /* Initialize daemon main loop */
    GMainLoop *loop = g_main_loop_new (NULL, FALSE);
    /* Attempt to register the daemon with 'kmitl.ce.os.RFOS' name on the bus */
    g_bus_own_name (G_BUS_TYPE_SESSION,
        "kmitl.ce.os.RFOS",
        G_BUS_NAME_OWNER_FLAGS_NONE,
        NULL,
        on_name_acquired,
        NULL,
        NULL,
        NULL);
    /* Start the main loop */
    g_main_loop_run (loop);
    return 0;
}
