#include "rfos.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

const char magic_number[] = "e1e1";
const int block_size = 64;

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

    /** Your code for Put method here **/
    guint err = 0;
    /** End of Put method execution, returning values **/
    rfos_complete_put(object, invocation, err);

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
        exit(errno);
    }

    gint64 disk_size = 0;
    fseek(fp, 0, SEEK_END);
    disk_size = (gint64)ftello(fp);
    fprintf(stderr, "%s %ld\n", "size = ", disk_size);
    fprintf(stderr, "sizeof(gint64) = %lu sizeof(disk_size) = %lu\n", sizeof(gint64), sizeof(disk_size));

    rewind(fp);

    fwrite(magic_number, sizeof(char), sizeof(magic_number)/sizeof(char), fp);
    fwrite(&disk_size, sizeof(gint64), sizeof(disk_size)/sizeof(gint64), fp);

    fflush(fp);

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
