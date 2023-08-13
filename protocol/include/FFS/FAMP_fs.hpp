#ifndef FAMP_PROTOCOL_FAMP_FS_H
#define FAMP_PROTOCOL_FAMP_FS_H
#include "../common.hpp"

#define FAMP_FS_HEADER_SIG                  0x46414D504653  /* "FAMPFS" */
#define FAMP_FS_REVISION                    0x0001
#define FAMP_FS_PARTITION_METADATA_SIZE     0x0200
#define FAMP_FS_BEGIN_SIG                   0x46534247      /* "FSBG" */
#define FAMP_FS_PARTITION_BEGIN_SIG         0x46424547      /* "FBEG" */

namespace FAMP_FS
{
    /* "Partition Header". */
    struct FAMP_FS_HEADER
    {
        uint8       FampFS_Sig[6];      /* "FAMPFS" */
        uint16      FampFS_Rev;         /* Should always be equal to `FAMP_FS_REVISION`. */
        uint16      FampFS_NOP;         /* NOP - Number Of Partitions; this is the number of partitions that the FileSystem consists of. */

        uint16      padding[0xF9];
        uint32      FampFS_BeginSig;    /* "FSBG" */

        #ifndef OS_RELATED
        FAMP_FS_HEADER() = default;
        ~FAMP_FS_HEADER() = default;
        #endif
    };

    /* Partition access.
     * Who can access the program/data residing in the given partition on the FileSystem?
     * */
    enum class PartitionAccess
    {
        ProtocolOnly    = 0xD0,         /* Only the protocol can access the partition. */
        KernelOnly      = 0xD1,         /* Only the kernel (and Protocol) can access, and manipulate (R/W), the partition. */
        UserOnly        = 0xD2,         /* Only the user (and Protocol) can access, and manipulate (R/W), the partition. */
        Kernel_User     = 0xD3,         /* The kernel and the user (and Protocol) can access, and manipulate (R/W), the partition. */
        Protocol_Kernel = 0xD4          /* Only the protocol and kernel can access, and manipulate (R/W), the partition. */
    };

    /* Action taking place on a partition.
     * This is useful to know what is happening with a current partition.
     * */
    enum class PartitionAction
    {
        Writing = 0xF0,    /* The partition (file) is being written to. */
        Reading = 0xF1,    /* The partition (file) is being read. */
        Nothing = 0xF2     /* The partition (file) has no action being acted on it. */
    };

    /* PartitionType.
     * If the partition is a program, it will be set to `Code` otherwise it will be set to `Data`.
     * */
    enum PartitionType
    {
        Code,
        Data
    };

    /* What identity messed with the partition.
     * This is set upon the partition gettting read, written to or initiated.
     * */
    enum class PartitionIdentity
    {
        Protocol,
        User,
        Kernel
    };

    /* Relocation actions.
     * */
    enum class RelocationAction
    {
        Relocate,               /* If this is set, this tells the protocol (OS) that the partition (file) is being temporarily relocated, and that `RelocateBack` will occur. */
        RelocateAndStay,        /* If this is set, this tells the protocol (OS) that the partition (file) is being permanently relocated. */
        RelocateBack            /* If this is set, this tells the protocol (OS) that the partition (file) can be loaded into its original address. */
    };

    /* "Metadata" of each partition that the FileSystem consists of. */
    struct FAMP_FS_PARTITION_METADATA
    {
        uint8       PartitionName[100];     /* AKA Filename. */
        uint8       PartitionAcc;           /* PartitionAcc = PartitionAccess. */
        uint8       PartitionAct;           /* PartitionAct = PartitionAction. */
        uint32      PartitionAddress;       /* Where does the partition (file) get loaded into memory? */
        uint8       PartitionT;             /* PartitionT = PartitionType. */

        /* Size information. */
        uint32      PartitionSectorSize;    /* How many sectors does the partition (file) take up? */
        uint32      PartitionByteSize;      /* How many bytes does the partition (file) take up? (PartitionSectorSize * 512) */
        uint16      PartitionActionCount;   /* How many actions have occured for the partition (file)? */
        uint8       PartitionActionID;      /* What identity last manipulated the partition (file)? */
        uint32      PartitionDataOffset;    /* Where do the partitions that make up the FileSystem begin? */

        /* Relocation data (not required; gets filled out upon the partition (file) needing to be relocated). */
        bool        NeedsRelocated;         /* Does the partition (file) need to be relocated? */

        /* If `NeedsRelocated` is true, the following will be filled out.
         * Else, the following will be all zeros.
         * */
        uint8       RequestedBy;            /* What identity requested for the partition (file) to be relocated? */
        uint32      RelocationAddress;      /* Where is the partition (file) being relocated to? */
        bool        WillReturn;             /* If true, `OriginalAddress` will be set to `PartitionAddress` before `PartitionAddress` is changed to the new address. */
        uint32      OriginalAddress;
        uint8       RelocateAcc;            /* RelocateAcc = RelocateAction; if `WillReturn` is true, this will be kept track of to see if it changed to `RelocateBack`, else it will be set to `RelocateAndStay`. */

        uint8       Padding[0x176];
        uint32      PartitionBeginSig;      /* "PBEG". */
    };
}

#endif