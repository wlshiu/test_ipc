
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "core_master.h"
#include "core_remote.h"
#include "openamp/remoteproc.h"
//=============================================================================
//                  Constant Definition
//=============================================================================
/* IPC Device parameters */
static  uint32_t        g_share_buf[5120] = {0};
#define SHM_ADDR                          (void *)&g_share_buf
#define SHM_SIZE                          sizeof(g_share_buf)
#define VRING0_IPI_VECT                   6
#define VRING1_IPI_VECT                   3
#define MASTER_CPU_ID                     0
#define REMOTE_CPU_ID                     1

static uint32_t         g_vring0_buf[16 << 10];
static uint32_t         g_vring1_buf[16 << 10];
#define VIRTIO_RPMSG_F_NS	              0 /* RP supports name service notifications */
#define VRING0_BASE                       &g_vring0_buf
#define VRING1_BASE                       &g_vring1_buf
#define VRING_ALIGN                       0x1000

#define VIRTIO_ID_RPMSG_                    7
#define RPMSG_IPU_C0_FEATURES               1
#define NUM_VRINGS                          0x02
//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

//=============================================================================
//                  Global Data Definition
//=============================================================================
core_attr_t         g_core_attr[CORE_ID_TOTAL];

static struct fw_rsc_vdev   g_rpmsg_vdev =
{
	.type          = RSC_VDEV,
    .id            = VIRTIO_ID_RPMSG_,
    .dfeatures     = RPMSG_IPU_C0_FEATURES,
	.num_of_vrings = NUM_VRINGS,
	.status        = VIRTIO_CONFIG_STATUS_DRIVER_OK,
};

static struct hil_proc      g_processor_priv[] =
{
    [0] = {
        .cpu_id = MASTER_CPU_ID,
        .sh_buff = { (void *)SHM_ADDR, SHM_SIZE, 0x00 },
        .vdev = {
            .num_vrings = 2,
            .dfeatures = (1 << VIRTIO_RPMSG_F_NS),
            .gfeatures = 0,
            .vdev_info = &g_rpmsg_vdev,

            .vring_info[0] = { /* TX */
                .vaddr = (void *)VRING0_BASE,
                .num_descs = 256,
                .align = VRING_ALIGN,
                .intr_info = {
                    .vect_id = VRING0_IPI_VECT,
                    .priority = 0,
                    .trigger_type = 0,
                }
            },
            .vring_info[1] = { /* RX */
                .vaddr = (void *)VRING1_BASE,
                .num_descs = 256,
                .align = VRING_ALIGN,
                .intr_info = {
                    .vect_id = VRING1_IPI_VECT,
                    .priority = 0,
                    .trigger_type = 0,
                }
            }
        },

        .num_chnls = 1,
        .chnls ={ {"rpmsg-openamp-chnl"} },
        .ops = 0,
    },

    [1] = {
        .cpu_id = REMOTE_CPU_ID,
        .sh_buff = { (void *)SHM_ADDR, SHM_SIZE, 0x00 },
        .vdev = {
            .num_vrings = 2,
            .dfeatures = (1 << VIRTIO_RPMSG_F_NS),
            .gfeatures = 0,

            .vring_info[0] = { /* RX */
                .vaddr = (void *)VRING0_BASE,
                .num_descs = 256,
                .align = VRING_ALIGN,
                .intr_info = {
                    .vect_id = VRING0_IPI_VECT,
                    .priority = 0,
                    .trigger_type = 0,
                }
            },
            .vring_info[1] = { /* TX */
                .vaddr = (void *)VRING1_BASE,
                .num_descs = 256,
                .align = VRING_ALIGN,
                .intr_info = {
                    .vect_id = VRING1_IPI_VECT,
                    .priority = 0,
                    .trigger_type = 0,
                }
            }
        },

        .num_chnls = 1,
        .chnls ={ {"rpmsg-openamp-chnnl"} },
        .ops = 0,
    },
};
//=============================================================================
//                  Private Function Definition
//=============================================================================

//=============================================================================
//                  Public Function Definition
//=============================================================================
int main()
{
    struct proc_vdev        *pVirtDev = 0;

    srand(clock());

    memset(&g_core_attr, 0x0, sizeof(g_core_attr));

    //--------------------------------
    // assign basic info
    g_core_attr[CORE_ID_MASTER].core_id     = CORE_ID_MASTER;
    g_core_attr[CORE_ID_MASTER].pProc_cfg   = &g_processor_priv[0];
    g_core_attr[CORE_ID_REMOTE_1].core_id   = CORE_ID_REMOTE_1;
    g_core_attr[CORE_ID_REMOTE_1].pProc_cfg = &g_processor_priv[1];

    //---------------------------------
    // assign virtual device info

    /* master core */
    pVirtDev = &g_core_attr[CORE_ID_MASTER].pProc_cfg->vdev;
    pVirtDev->intr_info.data = (void*)&g_core_attr[CORE_ID_REMOTE_1];

    /* remote core */
    pVirtDev = &g_core_attr[CORE_ID_REMOTE_1].pProc_cfg->vdev;
    pVirtDev->intr_info.data = (void*)&g_core_attr[CORE_ID_MASTER];

    //---------------------------------------
    // initialize cores
    core_master_init(&g_core_attr[CORE_ID_MASTER]);
    core_remote_init(&g_core_attr[CORE_ID_REMOTE_1]);

    core_irq_simulator(&g_core_attr[0], CORE_ID_TOTAL);

    //------------------------------
    // start
    core_master_boot(&g_core_attr[0], CORE_ID_TOTAL);

    while(1)    Sleep((DWORD)-1);

    return 0;
}
