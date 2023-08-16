#include "fs_worker.hpp"

void __START main()
{
    if(disk_image_heading->UsedEntries == 2)
        __asm__("jmp 0x7E00");
    if(disk_image_heading->HeaderSig == FAMP_HEADER_START_SIGNATURE)
    {
        __asm__("mov ah, 0x0E\n");
        __asm__("mov al, 'A'\n");
        __asm__("int 0x10");
        __asm__("mov ah, 0x0E\n");
        __asm__("mov al, 'S'\n");
        __asm__("int 0x10");
        __asm__("mov ah, 0x0E\n");
        __asm__("mov al, 'S'\n");
        __asm__("int 0x10");
    }
    while(true);
}