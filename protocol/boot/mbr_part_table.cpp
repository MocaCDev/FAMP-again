#include "mbr_part_table.hpp"

#define MBR_partition_table_entries_offset     0x1BE
#define second_MBR_partition_table_entry       (0x1BE + 0) + 16
#define third_MBR_partition_table_entry        (0x1BE + 16) + 16
#define fourth_MBR_partition_table_entry       (0x1BE + 32) + 16

void __attribute__((section("__start"))) main()
{
    /*heading = *(struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *) FAMP_DISK_IMAGE_HDR_ADDR;
    heading->HeaderSig = revert_value<uint32> (heading->HeaderSig);
    MBR_ptr_addr += sizeof(*heading);*/

    /* MBR "entry" code outline. */
    //*entry_code = *(struct MBR_bin_outline *) MBR_ptr_addr;
    //MBR_ptr_addr += sizeof(*entry_code);
    
    /* MBR partition table entries. */
    *pentry = *(struct partition_entry *) (0x7C00 + MBR_partition_table_entries_offset);//MBR_ptr_addr;
    read_in_memory(SECOND_STAGE_ADDRESS/16, pentry->starting_sector, pentry->sector_amnt);

    //MBR_ptr_addr += sizeof(*pentry);
    *pentry = *(struct partition_entry *) (0x7C00 + second_MBR_partition_table_entry);

    __asm__("jmp 0x0:0x7E0D");

    while(true);
}