/*
 * Copyright (c) 2015 Freescale Semiconductor, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Freescale Semiconductor nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************
 * FILE NAME
 *
 *       platform_info.c
 *
 * DESCRIPTION
 *
 *       This file implements APIs to get platform specific
 *       information for OpenAMP.
 *
 **************************************************************************/


#include "common/hil/hil.h"

/* Reference implementation that show cases platform_get_cpu_info and
 platform_get_for_firmware API implementation for Bare metal environment */

#if 0
extern struct hil_platform_ops proc_ops;
#endif

#define VIRTIO_RPMSG_F_NS               0 /* RP supports name service notifications */

/*
 * Linux requires the ALIGN to 0x1000(4KB) instead of 0x80
 */
#define VRING_ALIGN                       0x1000

/*
 * Linux has a different alignment requirement, and its have 512 buffers instead of 32 buffers for the 2 ring
 */
static uint32_t     g_vring_0[8 << 10] = {0};
static uint32_t     g_vring_1[8 << 10] = {0};
#define VRING0_BASE                       &g_vring_0
#define VRING1_BASE                       &g_vring_1


/* IPI_VECT here defines VRING index in MU */
#define VRING0_IPI_VECT                   0
#define VRING1_IPI_VECT                   1

#define MASTER_CPU_ID                     0
#define REMOTE_CPU_ID                     1

/*
 * 32 MSG (16 rx, 16 tx), 512 bytes each, it is only used when RPMSG driver is working in master mode, otherwise
 * the share memory is managed by the other side.
 * When working with Linux, SHM_ADDR and SHM_SIZE is not used
 */

static uint32_t         g_share_buf[20 << 10] = {0};
#define SHM_ADDR                    &g_share_buf
#define SHM_SIZE                    sizeof(g_share_buf)


/**
 * This array provides definition of CPU nodes for master and remote
 * context. It contains two nodes because the same file is intended
 * to use with both master and remote configurations. On zynq platform
 * only one node definition is required for master/remote as there
 * are only two cores present in the platform.
 *
 * Only platform specific info is populated here. Rest of information
 * is obtained during resource table parsing.The platform specific
 * information includes;
 *
 * -CPU ID
 * -Shared Memory
 * -Interrupts
 * -Channel info.
 *
 * Although the channel info is not platform specific information
 * but it is convenient to keep it in HIL so that user can easily
 * provide it without modifying the generic part.
 *
 * It is good idea to define hil_proc structure with platform
 * specific fields populated as this can be easily copied to hil_proc
 * structure passed as parameter in platform_get_processor_info. The
 * other option is to populate the required structures individually
 * and copy them one by one to hil_proc structure in platform_get_processor_info
 * function. The first option is adopted here.
 *
 *
 * 1) First node in the array is intended for the remote contexts and it
 *    defines Master CPU ID, shared memory, interrupts info, number of channels
 *    and there names. This node defines only one channel
 *   "rpmsg-data-channel".
 *
 * 2)Second node is required by the master and it defines remote CPU ID,
 *   shared memory and interrupts info. In general no channel info is required by the
 *   Master node, however in bare-metal master and linux remote case the linux
 *   rpmsg bus driver behaves as master so the rpmsg driver on linux side still needs
 *   channel info. This information is not required by the masters for bare-metal
 *   remotes.
 *
 */
extern struct hil_platform_ops     processor_ops;
struct hil_proc proc_table [] =
{
    /* CPU node for remote context */
    {
        .cpu_id  = MASTER_CPU_ID,
        .sh_buff = { (void *)SHM_ADDR, SHM_SIZE, 0x00 },
        .vdev = {
            .num_vrings = 2,
            .dfeatures  = (1 << VIRTIO_RPMSG_F_NS),
            .gfeatures  = 0,

            .vring_info[0] = { /* TX */
                .vq        = NULL,
                .phy_addr  = (void *)VRING0_BASE,
                .num_descs = 256,
                .align     = VRING_ALIGN,
                .intr_info = { VRING0_IPI_VECT, 0, 0, NULL }
            },
            .vring_info[1] = { /* RX */
                .vq        = NULL,
                .phy_addr  = (void *)VRING1_BASE,
                .num_descs = 256,
                .align     = VRING_ALIGN,
                .intr_info ={ VRING1_IPI_VECT, 0, 0, NULL }
            }
        },

        .num_chnls = 1,
        .chnls     = { {"rpmsg-openamp-demo-channel"} },
        .ops       = &processor_ops,
    },

    /* CPU node for remote context */
    {
        .cpu_id  = REMOTE_CPU_ID,
        .sh_buff = { (void *)SHM_ADDR, SHM_SIZE, 0x00 },
        .vdev = {
            .num_vrings = 2,
            .dfeatures  = (1 << VIRTIO_RPMSG_F_NS),
            .gfeatures  = 0,

            .vring_info[0] = {/* RX */
                .vq        = NULL,
                .phy_addr  = (void *)VRING0_BASE,
                .num_descs = 256,
                .align     = VRING_ALIGN,
                .intr_info = { VRING0_IPI_VECT, 0, 0, NULL }
            },
            .vring_info[1] = {/* TX */
                .vq        = NULL,
                .phy_addr  = (void *)VRING1_BASE,
                .num_descs = 256,
                .align     = VRING_ALIGN,
                .intr_info = { VRING1_IPI_VECT, 0, 0, NULL }
            }
        },

        .num_chnls = 1,
        .chnls     = { {"rpmsg-openamp-demo-channel"} },
        .ops       = &processor_ops,

    }
};

/**
 * platform_get_processor_info
 *
 * Copies the target info from the user defined data structures to
 * HIL proc  data structure.In case of remote contexts this function
 * is called with the reserved CPU ID HIL_RSVD_CPU_ID, because for
 * remotes there is only one master.
 *
 * @param proc   - HIL proc to populate
 * @param cpu_id - CPU ID
 *
 * return  - status of execution
 */
int platform_get_processor_info(struct hil_proc *proc, int cpu_id)
{
    int             idx;

    for(idx = 0; idx < sizeof(proc_table) / sizeof(struct hil_proc); idx++)
    {
        if((cpu_id == HIL_RSVD_CPU_ID) || (proc_table[idx].cpu_id == cpu_id))
        {
            env_memcpy(proc, &proc_table[idx], sizeof(struct hil_proc));
            return 0;
        }
    }
    return -1;
}

int platform_get_processor_for_fw(char *fw_name)
{
    return 1;
}
