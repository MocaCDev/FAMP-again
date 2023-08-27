#include "fs_worker.hpp"

static puint8 FS_data = (puint8) FAMP_FS_INITIAL_ADDRESS;
static puint8 FS_dest_addr;

static puint8 fs = (puint8) 0xF000;

void __START main()
{
    /* Move*/
    /*puint8 FS_dest_addr = (puint8) FAMP_FS_INITIAL_ADDRESS;
    FS_data = (puint8) FAMP_FS_INITIAL_ADDRESS;

    for(uint32 i = 0; i < 1024 + kernel_partition_metadata->PartitionByteSize; i++)
    {
        *FS_dest_addr = *FS_data;
        FS_dest_addr++;
        FS_data++;
    }*/

    while(true);
}