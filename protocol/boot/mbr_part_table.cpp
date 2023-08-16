#include "mbr_part_table.hpp"

void __attribute__((section("__start"))) main()
{
    *heading = *(struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *) FAMP_DISK_IMAGE_HDR_ADDR;
    heading->HeaderSig = revert_value<uint32> (heading->HeaderSig);
    MBR_ptr_addr += sizeof(*heading);

    /* MBR "entry" code outline. */
    *entry_code = *(struct MBR_bin_outline *) MBR_ptr_addr;
    MBR_ptr_addr += sizeof(*entry_code);
    
    /* MBR partition table entries. */
    struct partition_entry *pentry = (struct partition_entry *) MBR_ptr_addr;

    uint8 starting_sector = pentry->starting_sector;

    __asm__("xor ax, ax\nmov es, ax\n");
    __asm__("mov bx, %0\n" :: "dN"((uint16)SECOND_STAGE_ADDRESS));
    __asm__("mov ah, 0x02\nmov al, %0\n" :: "dN"((uint8) pentry->sector_amnt));
    __asm__("mov ch, 0x00\nmov cl, %0\n" :: "dN"((uint8) starting_sector));
    __asm__("mov dh, 0x00\nmov dl, 0x80\nint 0x13");

    __asm__("jmp 0x070D");

    while(true);
}