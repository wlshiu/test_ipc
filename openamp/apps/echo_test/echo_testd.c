/* This is a sample demonstration application that showcases usage of rpmsg
This application is meant to run on the remote CPU running baremetal code.
This application echoes back data that was sent to it by the master core. */

#include <stdio.h>
#include <openamp/open_amp.h>
#include "rsc_table.h"
#include "platform_info.h"

#define SHUTDOWN_MSG    0xEF56A55A

//#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...)
#define LPERROR(format, ...) LPRINTF("ERROR: " format, ##__VA_ARGS__)

/* External functions */
extern int init_system(void);
extern void cleanup_system(void);

/* Local variables */
static struct remote_proc *proc = NULL;
static struct rsc_table_info rsc_info;
static int evt_chnl_deleted = 0;
static int evt_virtio_rst = 0;

static void virtio_rst_cb(struct hil_proc *hproc, int id)
{
    /* hil_proc only supports single virtio device */
    (void)id;

    if (!proc || proc->proc != hproc || !proc->rdev)
        return;

    LPRINTF("Resetting RPMsg\n");
    evt_virtio_rst = 1;
}

/*-----------------------------------------------------------------------------*
 *  RPMSG callbacks setup by remoteproc_resource_init()
 *-----------------------------------------------------------------------------*/
static void rpmsg_read_cb(struct rpmsg_channel *rp_chnl, void *data, int len,
                          void *priv, unsigned long src)
{
    (void)priv;
    (void)src;

    /* On reception of a shutdown we signal the application to terminate */
    if ((*(unsigned int *)data) == SHUTDOWN_MSG)
    {
        evt_chnl_deleted = 1;
        return;
    }

    /* Send data back to master */
    if (rpmsg_send(rp_chnl, data, len) < 0)
    {
        LPERROR("rpmsg_send failed\n");
    }
}

static void rpmsg_channel_created(struct rpmsg_channel *rp_chnl)
{
    (void)rp_chnl;
}

static void rpmsg_channel_deleted(struct rpmsg_channel *rp_chnl)
{
    (void)rp_chnl;

    evt_chnl_deleted = 1;
}

/*-----------------------------------------------------------------------------*
 *  Application
 *-----------------------------------------------------------------------------*/
int app(struct hil_proc *hproc)
{
    int status = 0;

    /* Initialize RPMSG framework */
    LPRINTF("Try to init remoteproc resource\n");
    status = remoteproc_resource_init(&rsc_info, hproc,
                                      rpmsg_channel_created,
                                      rpmsg_channel_deleted, rpmsg_read_cb,
                                      &proc, 0);

    if (RPROC_SUCCESS != status)
    {
        LPERROR("Failed  to initialize remoteproc resource.\n");
        return -1;
    }
    LPRINTF("Init remoteproc resource succeeded\n");

    hil_set_vdev_rst_cb(hproc, 0, virtio_rst_cb);

    LPRINTF("Waiting for events...\n");
    while(1)
    {
        hil_poll(proc->proc, 0);

        /* we got a shutdown request, exit */
        if (evt_chnl_deleted)
        {
            break;
        }

        if (evt_virtio_rst)
        {
            /* vring rst callback, reset rpmsg */
            LPRINTF("De-initializing RPMsg\n");
            rpmsg_deinit(proc->rdev);
            proc->rdev = NULL;

            LPRINTF("Reinitializing RPMsg\n");
            status = rpmsg_init(hproc, &proc->rdev,
                                rpmsg_channel_created,
                                rpmsg_channel_deleted,
                                rpmsg_read_cb,
                                1);
            if (status != RPROC_SUCCESS)
            {
                LPERROR("Reinit RPMsg failed\n");
                break;
            }
            LPRINTF("Reinit RPMsg succeeded\n");
            evt_chnl_deleted = 0;
            evt_virtio_rst = 0;
        }
    }

    /* disable interrupts and free resources */
    LPRINTF("De-initializating remoteproc resource\n");
    remoteproc_resource_deinit(proc);

    return 0;
}

/*-----------------------------------------------------------------------------*
 *  Application entry point
 *-----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    unsigned long proc_id = 0;
    unsigned long rsc_id = 0;
    struct hil_proc *hproc;
    int status = -1;

    LPRINTF("Starting application...\n");

    /* Initialize HW system components */
    init_system();

    if (argc >= 2)
    {
        proc_id = strtoul(argv[1], NULL, 0);
    }

    if (argc >= 3)
    {
        rsc_id = strtoul(argv[2], NULL, 0);
    }

    hproc = platform_create_proc(proc_id);
    if (!hproc)
    {
        LPERROR("Failed to create proc platform data.\n");
    }
    else
    {
        rsc_info.rsc_tab = get_resource_table(
                               (int)rsc_id, &rsc_info.size);
        if (!rsc_info.rsc_tab)
        {
            LPERROR("Failed to get resource table data.\n");
        }
        else
        {
            status = app(hproc);
        }
    }

    LPRINTF("Stopping application...\n");
    cleanup_system();

    return status;
}

