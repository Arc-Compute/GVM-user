/*
 * Copyright (C) 2022 2666680 Ontario Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef GPU_NVIDIA_RESMAN_CLASSES_NV2080_H
#define GPU_NVIDIA_RESMAN_CLASSES_NV2080_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Command to allocate a Nv2080 device
#define NV2080_CLASS 0x00002080

//! Command to get the BUS PCI info.
#define NV2080_GET_BUS_PCI_INFO 0x20801801

/*! \brief Gets the BUS PCI info.
 *
 * This acts as a parameter into a method which gives us the BUS PCI info for a GPU.
 */
struct BusGetPciInfo {
    uint32_t dev_id;                 //!< Device ID.
    uint32_t sub_dev_id;             //!< Sub device ID.
    uint32_t revision_id;            //!< Revision ID.
    uint32_t ext_id;                 //!< Extension ID.
};

//! Command to get physical channel counts.
#define NV2080_FIFO_GET_PHYSICAL_CHANNEL_COUNT 0x20801108

/*! \brief Gets the physical fifo count.
 *
 * This provides us physical fifo channel counts.
 */
struct Nv2080CtrlFIFOGetPhysicalChannelCount {
    uint32_t physical_channel_count; //!< Physical channel count.
    uint32_t physical_channel_use;   //!< Physical channel count in use.
};

//! Command to get DMA Info from the host.
#define NV2080_GET_DMA_INFO 0x20802503

/*! \brief Information regarding the DMA information of the GPU.
 *
 * This acts as a value being returned to provide information regarding
 * the DMA index.
 */
struct Nv2080CtrlDMAInfo {
    uint32_t index;                  //!< Index of the DMA information.
    uint32_t value;                  //!< Value of the DMA information.
};

/*! \brief Parameters to get DMA info.
 *
 * This acts as a parameter to the system to get DMA info about the card
 * in question.
 */
struct Nv2080CtrlDMAGetInfoParams {
    uint32_t table_size;
    struct Nv2080CtrlDMAInfo dma_info[256];
};

//! Command to get FB Info V2
#define NV2080_GET_FB_INFO_V2 0x20801303

// Thanks to NVIDIA open source of the NV2080_CTRL_FB_INFO data.
#define NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_COUNT                (0x00000000U) // Deprecated
#define NV2080_CTRL_FB_INFO_INDEX_COMPRESSION_SIZE                 (0x00000001U)
#define NV2080_CTRL_FB_INFO_INDEX_DRAM_PAGE_STRIDE                 (0x00000002U)
#define NV2080_CTRL_FB_INFO_INDEX_TILE_REGION_FREE_COUNT           (0x00000003U)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_COUNT                  (0x00000004U)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_SIZE                        (0x00000005U)
#define NV2080_CTRL_FB_INFO_INDEX_BANK_SWIZZLE_ALIGNMENT           (0x00000006U)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_SIZE                         (0x00000007U)
#define NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE                   (0x00000008U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE                        (0x00000009U)
#define NV2080_CTRL_FB_INFO_INDEX_MAPPABLE_HEAP_SIZE               (0x0000000AU)
#define NV2080_CTRL_FB_INFO_INDEX_BUS_WIDTH                        (0x0000000BU)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_CFG                          (0x0000000CU)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_TYPE                         (0x0000000DU)
#define NV2080_CTRL_FB_INFO_INDEX_BANK_COUNT                       (0x0000000EU)
#define NV2080_CTRL_FB_INFO_INDEX_OVERLAY_OFFSET_ADJUSTMENT        (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_SPACE_SIZE_KB          (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_HEAP_SIZE_KB           (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_GPU_VADDR_MAPPBLE_SIZE_KB        (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_EFFECTIVE_BW                     (0x0000000FU) // Deprecated (index reused to return 0)
#define NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB                   (0x00000010U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_BASE_KB                     (0x00000011U)
#define NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_SIZE_KB      (0x00000012U)
#define NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_BASE_KB      (0x00000013U)
#define NV2080_CTRL_FB_INFO_INDEX_PARTITION_MASK                   (0x00000014U)
#define NV2080_CTRL_FB_INFO_INDEX_VISTA_RESERVED_HEAP_SIZE         (0x00000015U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE                        (0x00000016U)
#define NV2080_CTRL_FB_INFO_INDEX_RAM_LOCATION                     (0x00000017U)
#define NV2080_CTRL_FB_INFO_INDEX_FB_IS_BROKEN                     (0x00000018U)
#define NV2080_CTRL_FB_INFO_INDEX_FBP_COUNT                        (0x00000019U)
#define NV2080_CTRL_FB_INFO_INDEX_FBP_MASK                         (0x0000001AU)
#define NV2080_CTRL_FB_INFO_INDEX_L2CACHE_SIZE                     (0x0000001BU)
#define NV2080_CTRL_FB_INFO_INDEX_MEMORYINFO_VENDOR_ID             (0x0000001CU)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_AVAIL_SIZE                  (0x0000001DU)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_START                       (0x0000001EU)
#define NV2080_CTRL_FB_INFO_INDEX_BAR1_MAX_CONTIGUOUS_AVAIL_SIZE   (0x0000001FU)
#define NV2080_CTRL_FB_INFO_INDEX_USABLE_RAM_SIZE                  (0x00000020U)
#define NV2080_CTRL_FB_INFO_INDEX_TRAINIG_2T                       (0x00000021U)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_COUNT                        (0x00000022U)
#define NV2080_CTRL_FB_INFO_INDEX_LTS_COUNT                        (0x00000023U)
#define NV2080_CTRL_FB_INFO_INDEX_L2CACHE_ONLY_MODE                (0x00000024U)
#define NV2080_CTRL_FB_INFO_INDEX_PSEUDO_CHANNEL_MODE              (0x00000025U)
#define NV2080_CTRL_FB_INFO_INDEX_SMOOTHDISP_RSVD_BAR1_SIZE        (0x00000026U)
#define NV2080_CTRL_FB_INFO_INDEX_HEAP_OFFLINE_SIZE                (0x00000027U)
#define NV2080_CTRL_FB_INFO_INDEX_1TO1_COMPTAG_ENABLED             (0x00000028U)
#define NV2080_CTRL_FB_INFO_INDEX_SUSPEND_RESUME_RSVD_SIZE         (0x00000029U)
#define NV2080_CTRL_FB_INFO_INDEX_ALLOW_PAGE_RETIREMENT            (0x0000002AU)
#define NV2080_CTRL_FB_INFO_INDEX_LTC_MASK                         (0x0000002BU)
#define NV2080_CTRL_FB_INFO_POISON_FUSE_ENABLED                    (0x0000002CU)
#define NV2080_CTRL_FB_INFO_FBPA_ECC_ENABLED                       (0x0000002DU)
#define NV2080_CTRL_FB_INFO_DYNAMIC_PAGE_OFFLINING_ENABLED         (0x0000002EU)
#define NV2080_CTRL_FB_INFO_INDEX_FORCED_BAR1_64KB_MAPPING_ENABLED (0x0000002FU)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_SIZE                 (0x00000030U)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_ALIGNMENT            (0x00000031U)
#define NV2080_CTRL_FB_INFO_INDEX_P2P_MAILBOX_BAR1_MAX_OFFSET_64KB (0x00000032U)
#define NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_TOTAL_KB      (0x00000033U)
#define NV2080_CTRL_FB_INFO_INDEX_PROTECTED_MEM_SIZE_FREE_KB       (0x00000034U)
#define NV2080_CTRL_FB_INFO_INDEX_ECC_STATUS_SIZE                  (0x00000035U)

/* valid fb RAM type values */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_UNKNOWN                       (0x00000000U)
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDRAM                         (0x00000001U)
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR1                          (0x00000002U) /* SDDR and GDDR (aka DDR1 and GDDR1) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR2                         (0x00000003U) /* SDDR2 Used on NV43 and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR2                          NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR2 /* Deprecated alias */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR2                         (0x00000004U) /* GDDR2 Used on NV30 and some NV36 */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR3                         (0x00000005U) /* GDDR3 Used on NV40 and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR4                         (0x00000006U) /* GDDR4 Used on G80 and later (deprecated) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR3                         (0x00000007U) /* SDDR3 Used on G9x and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_DDR3                          NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR3 /* Deprecated alias */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5                         (0x00000008U) /* GDDR5 Used on GT21x and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR2                        (0x00000009U) /* LPDDR (Low Power SDDR) used on T2x and later. */


#define NV2080_CTRL_FB_INFO_RAM_TYPE_SDDR4                         (0x0000000CU) /* SDDR4 Used on Maxwell and later */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR4                        (0x0000000DU) /* LPDDR (Low Power SDDR) used on T21x and later.*/
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM1                          (0x0000000EU) /* HBM1 (High Bandwidth Memory) used on GP100 */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM2                          (0x0000000FU) /* HBM2 (High Bandwidth Memory-pseudo channel) */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR5X                        (0x00000010U) /* GDDR5X Used on GP10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6                         (0x00000011U) /* GDDR6 Used on TU10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_GDDR6X                        (0x00000012U) /* GDDR6X Used on GA10x */
#define NV2080_CTRL_FB_INFO_RAM_TYPE_LPDDR5                        (0x00000013U) /* LPDDR (Low Power SDDR) used on T23x and later.*/
#define NV2080_CTRL_FB_INFO_RAM_TYPE_HBM3                          (0x00000014U) /* HBM3 (High Bandwidth Memory) v3 */

/*! \brief Information regarding the FB information of the GPU.
 *
 * This acts as a value being returned to provide information regarding
 * the FB index.
 */
struct Nv2080CtrlFBInfo {
    uint32_t index;                  //!< Index of the FB information.
    uint32_t value;                  //!< Value of the FB information.
};

/*! \brief Gets FB Info regarding the GPU.
 *
 * Gets the FB info for the entire GPU.
 */
struct Nv2080GetFBInfoV2 {
    uint32_t table_size;
    struct Nv2080CtrlFBInfo fb_info[0x35];
};

//! Command to get GID information.
#define NV2080_GPU_GET_GID_INFO 0x2080014A

/*! \brief Gets GPU GID information.
 *
 * Gets GPU GID information.
 */
struct Nv2080GpuGetGidInfo {
    uint32_t index;
    uint32_t flags;
    uint32_t length;
    uint8_t data[0x100];
};

#ifdef __cplusplus
};
#endif

#endif
