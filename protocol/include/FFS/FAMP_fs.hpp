#ifndef FAMP_PROTOCOL_FAMP_FS_H
#define FAMP_PROTOCOL_FAMP_FS_H
#include "../common.hpp"

constexpr uint8 FAMP_FS_HEADER_SIG[6]       = {'F', 'A', 'M', 'P', 'F', 'S'};//0x46414D504653  /* "FAMPFS" */
#define FAMP_FS_REVISION                    0x0001
#define FAMP_FS_PARTITION_METADATA_SIZE     0x0200
uint32 FAMP_FS_BEGIN_SIG                    = 0x46534247;   /* "FSBG" */
uint32 FAMP_FS_PARTITION_BEGIN_SIG          = 0x50424547;   /* "PBEG" */

/* Initial address of the FS. */
#define FAMP_FS_INITIAL_ADDRESS             0x8200
#define FAMP_FS_KERNEL_VIRTUAL_ADDRESS      0x80000000

/* Kernel partition name. */
constexpr uint8 FAMP_FS_KERNEL_PART_NAME[6] = {'K', 'E', 'R', 'N', 'E', 'L'};

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

        FAMP_FS_HEADER() = default;
        ~FAMP_FS_HEADER() = default;
    } __attribute__((packed));

    /* Partition access.
     * Who can access the program/data residing in the given partition on the FileSystem?
     * */
    enum class PartitionAccess
    {
        ProtocolOnly    = 0xD0,         /* Only the protocol can access the partition. */
        KernelOnly      = 0xD1,         /* Only the kernel (and Protocol) can access, and manipulate (R/W), the partition. */
        UserOnly        = 0xD2,         /* Only the user (and Protocol) can access, and manipulate (R/W), the partition. */
        Kernel_User     = 0xD3,         /* The kernel and the user (and Protocol) can access, and manipulate (R/W), the partition. */
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
    enum class PartitionType
    {
        Code = 0x0,
        Data = 0x1
    };

    /* Partition address type.
     * This just gives more explicit information over the partition and whether or not
     * it resides in physica or virtual memory space.
     * */
    enum class PartitionAddressType
    {
        Physcial = 0x0,
        Virtual  = 0x1
    };

    /* What identity messed with the partition.
     * This is set upon the partition gettting read, written to or initiated.
     * */
    enum class PartitionIdentity
    {
        Protocol = 0x1,
        User,
        Kernel
    };

    /* Relocation actions.
     * */
    enum class RelocationAction
    {
        Relocate = 0x1,               /* If this is set, this tells the protocol (OS) that the partition (file) is being temporarily relocated, and that `RelocateBack` will occur. */
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
        uint8       PartitionAddressT;      /* Type of partition address (physical or virtual)? */
        uint8       PartitionT;             /* PartitionT = PartitionType. */

        /* Size information. */
        uint32      PartitionSectorSize;    /* How many sectors does the partition (file) take up? */
        uint32      PartitionByteSize;      /* How many bytes does the partition (file) take up? (PartitionSectorSize * 512) */
        uint16      PartitionActionCount;   /* How many actions have occured for the partition (file)? */
        uint8       PartitionActionID;      /* What identity last manipulated the partition (file)? */
        uint32      PartitionDataOffset;    /* Where does the code/data of the partition begin (needed due to some partitions including additional data prior to the data/code beginning)? */

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

        uint8       Padding[0x175];
        uint32      PartitionBeginSig;      /* "PBEG". */
    } __attribute__((packed));
}

#endif