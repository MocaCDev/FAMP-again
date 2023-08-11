#ifndef FAMP_FILE_FORMAT_STRUCTURES_H
#define FAMP_FILE_FORMAT_STRUCTURES_H
#include "../common.hpp"

/* Size of sub heading. */
#define FAMP_SUBHEADING_SIZE                    0x0D

/* Initial disk image header information.
 * The initial disk image header resides inside the first sector of the disk image (first 512 bytes).
 * */
#define FAMP_HEADER_START_SIGNATURE             0x46414D50      /* FAMP */
#define FAMP_HEADER_END_SIGNATURE               0x4545          /* EE */
#define FAMP_CURRENT_REVISION                   0x00001000
#define FAMP_MAX_USED_ENTRIES                   0x04

/* Sub heading header information. 
 * Sub headings reside at the beginning of every program following the MBR.
 * */
#define FAMP_SUBHEADER_SIGNATURE                0x46534844       /* FSHD */

/* Section IDs that can be found in the "outline" of an assembly
 * program that requires critical enforecement of the protocol. 
 * */
#define FAMP_MBR_START_NAME           0x46454E54      /* FENT - Famp Entry */
#define FAMP_MBR_START_ID             0xF100
#define FAMP_MBR_PTBL_ENTRIES_NAME    0x464D5054      /* FMPT - Famp MBR Partition Table */
#define FAMP_MBR_PTBL_ENTRIES_ID      0xF2F2

/* Memory stamp information. */
#define FAMP_MEM_STAMP_PTBLE_P        0x10F0          /* PTBLE_P - Partition Table Program ID */
uint32 FAMP_MEM_STAMP_FEND_SIG      = (uint32) 0x46454E44;

namespace FFF_Structures
{
    enum class os_types: uint8
    {
        OT_32bit    = 0x02,
        OT_64bit    = 0x03
    };

    enum class fs_types: uint16
    {
        FT_FAMP     = 1 << 0x04,
        FT_FAT32    = 1 << 0x05,
        FT_ISO9660  = 1 << 0x06,
        FT_EXT      = 1 << 0x07
    };

    struct FAMP_PROTOCOL_DISK_IMAGE_HEADING
    {
        uint32          HeaderSig;
        uint16          padding1;

        uint16          ProtocolRevision;
        uint16          padding2;

        uint8           UsedEntries;
        uint16          EntryOffsets[4];

        uint8           os_name[15];
        uint8           os_type;
        uint8           os_version[6];
        uint8           filesystem_type;
        bool            in_production;

        uint16          HeaderEnd;
        uint16          padding3;

        #ifndef OS_RELATED
        FAMP_PROTOCOL_DISK_IMAGE_HEADING() = default;
        ~FAMP_PROTOCOL_DISK_IMAGE_HEADING() = default;
        #endif
    };

    struct FAMP_PROTOCOL_SUBHEADING
    {
        uint32          SubHeadingSig;
        uint16          padding;

        uint32          ProgramSizeInBytes;
        uint16          ProgramSizeInSectors;

        bool            IsAsmProgram;

        #ifndef OS_RELATED
        FAMP_PROTOCOL_SUBHEADING() = default;
        ~FAMP_PROTOCOL_SUBHEADING() = default;
        #endif
    };

    /* Data over the section that is occurring.
     * 2-byte ID signifying what the section is/what it is doing, alongside a
     * 4-byte ID name.
     * */
    struct FAMP_SECTION_DATA
    {
        uint16          SectionID;
        uint32          SectionIDName;
        uint16          padding;

        FAMP_SECTION_DATA() = default;
        ~FAMP_SECTION_DATA() = default;
    };

    /* The configuration does not need this, so only have it when the OS is running. */
    //#ifdef OS_RELATED
    struct FAMP_PROTOCOL_MEMORY_STAMP
    {
        uint16          MemID;
        uint32          MemIDSig;
        uint16          padding;
    } __attribute__((packed));
    //#endif
}

#endif