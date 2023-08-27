#ifndef FAMP_PROTOCOL_FS_WORKER_H
#define FAMP_PROTOCOL_FS_WORKER_H
#define OS_RELATED
#include <common.hpp>
#include <FFF/FFF_structures.hpp>
#include <FFS/FAMP_fs.hpp> /* for FAMP_FS_INITIAL_ADDRESS */
#include <bootloader/bootloader_structures.hpp>

using namespace FFF_Structures;
using namespace FAMP_FS;
using namespace BootloaderStructs;

extern uint8 filesystem[];
static puint8 virtual_kernel_data = (puint8) FAMP_FS_KERNEL_VIRTUAL_ADDRESS;

struct FAMP_FS_HEADER *fs_header = (struct FAMP_FS_HEADER *) 0x0D00;
struct FAMP_FS_PARTITION_METADATA *kernel_partition_metadata = (struct FAMP_FS_PARTITION_METADATA *) 0x0F00;

#endif