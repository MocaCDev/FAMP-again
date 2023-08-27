#include "second_stage.hpp"

void __START main()
{
    /* Load in the FS worker program that resides after the second stage bootloader. */
    //uint8 starting_sector = 0;

    /* Skip the heading, MBR entry code and first entry (this program resides in the first entry). */
    //MBR_ptr_addr += sizeof(struct FAMP_PROTOCOL_DISK_IMAGE_HEADING);
    //MBR_ptr_addr += sizeof(struct MBR_bin_outline);
    //MBR_ptr_addr += sizeof(struct partition_entry);
    
    /* Get the second entry data. */
    //struct partition_entry *pentry = (struct partition_entry *) MBR_ptr_addr;
    //starting_sector = (uint8) pentry->starting_sector;

    /* Read in the data. */
    //__asm__("xor ax, ax\nmov es, ax\n");
    //__asm__("mov bx, %0\n" :: "dN"((uint16)FAMP_FS_INITIAL_ADDRESS));
    //__asm__("mov ah, 0x02\nmov al, %0\n" :: "dN"((uint8) pentry->sector_amnt));
    //__asm__("mov ch, 0x00\nmov cl, %0\n" :: "dN"((uint8) starting_sector));
    //__asm__("mov dh, 0x00\nmov dl, 0x80\nint 0x13");

    //struct FAMP_FS_HEADER *fs_header = (struct FAMP_FS_HEADER *) temp_fs_addr;
    //temp_fs_addr += sizeof(*fs_header);
    //struct FAMP_FS_PARTITION_METADATA *kernel_metadata = (struct FAMP_FS_PARTITION_METADATA *) temp_fs_addr;
    
    /*puint8 filesystem_virtual_address = (puint8) (FAMP_FS_KERNEL_VIRTUAL_ADDRESS + (filesystem_SBK + kernel_metadata->PartitionByteSize));

    for(uint32 i = 0; i < filesystem_SBK + kernel_metadata->PartitionByteSize; i++)
    {
        *filesystem_virtual_address = *temp_fs_addr;
        temp_fs_addr++;
        filesystem_virtual_address++;
    }*/

    /*temp_fs_addr = (puint8) filesystem;
    temp_fs_addr += filesystem_SBK;
    uint32 index = 0;
    
    for(uint32 i = 1024; i < 1024 + kernel_metadata->PartitionByteSize; i++)
    {
        virtual_kernel_data[index] = *temp_fs_addr;
        temp_fs_addr++;
        index++;
    }

    __load_gdt();

    __asm__("jmp 0x8:0x1000");*/
    //__prepare_jump_to_pmode();
    //__load_gdt();


    if(filesystem[0] == 0x46)
    __asm__("mov ah, 0x0e\nmov al, 'a'\nint 0x10");
    //__asm__("jmp 0x00:0x7E0D");

    while(1);
}