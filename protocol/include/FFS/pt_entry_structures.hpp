#ifndef FAMP_PROTOCOL_PARTITION_TABLE_ENTRY_STRUCTURES_H
#define FAMP_PROTOCOL_PARTITION_TABLE_ENTRY_STRUCTURES_H
#include "../common.hpp"

#define ENTRY_IS_BOOTABLE           0x80
#define ENTRY_IS_NOT_BOOTABLE       0x00

#define ENTRY_TYPE_SS               0x0E    /* SS - Second Stage */
#define ENTRY_TYPE_KERNEL           0x0D
#define ENTRY_TYPE_FILESYSTEM       0x0C
#define ENTRY_TYPE_UNUSED           0x01

namespace PartitionTable_Entry_Structs
{
    struct partition_entry
    {
        uint8       bootable_entry;
        uint16      starting_sector;
        uint8       entry_type;
        bool        auto_read_program;
        uint16      last_sector;
        uint32      relative_sector;
        uint8       sector_amnt;
        uint32      padding;

        #ifndef OS_RELATED
        partition_entry() = default;
        ~partition_entry() = default;
        #endif
    } __attribute__((packed, aligned(1)));
}

#endif