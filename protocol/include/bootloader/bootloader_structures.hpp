#ifndef FAMP_PROTOCOL_BOOTLOADER_STRUCTURES_H
#define FAMP_PROTOCOL_BOOTLOADER_STRUCTURES_H

#include "../FFF/FFF_structures.hpp"
using namespace FFF_Structures;

/* The disk image header is at an offset of 5 in the MBR binary
 * due to a jmp instruction.
 * */
#define FAMP_DISK_IMAGE_HDR_OFFSET  0x05
#define FAMP_DISK_IMAGE_HDR_ADDR    (0x7C00 + FAMP_DISK_IMAGE_HDR_OFFSET)
#define FAMP_MBR_ADDR               0x7C00

#define ENTRY_IS_BOOTABLE           0x80
#define ENTRY_IS_NOT_BOOTABLE       0x00

#define ENTRY_TYPE_SS               0x0E    /* SS - Second Stage */
#define ENTRY_TYPE_KERNEL           0x0D
#define ENTRY_TYPE_FILESYSTEM       0x0C
#define ENTRY_TYPE_UNUSED           0x01

#ifdef OS_RELATED
static puint8 MBR_ptr_addr = (puint8) FAMP_DISK_IMAGE_HDR_ADDR;
#endif

namespace BootloaderStructs
{
    struct MBR_bin_outline
    {
        uint16          MBR_start_ID;
        uint8           MBR_start_ID_name[4];
        uint16          padding;

        /* There is 383 bytes (0x17F) of entry-code that is allowed between
         * the section data and the next piece of section data (MBR partition table entries).
         * */
        uint8           entry_code[0x17A];

        uint16          MBR_ptbl_entry_ID;
        uint8           MBR_ptbl_entry_ID_name[4];
        uint16          padding2;

        //uint8           rest_of_MBR[42];
    } __attribute__((packed));

    enum class PartitionNumber
    {
        FirstPart = 0x0,
        SecondPart,
        ThirdPart,
        FourthPart
    };

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

        partition_entry() = default;

        ~partition_entry() = default;
    } __attribute__((packed));
}

#endif