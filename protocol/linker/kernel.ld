ENTRY(main)
SECTIONS {
    /* Program starts at 0x0700. */
    . = 0xA000;

    /* MBR related addresses. */
    disk_image_heading = 0x0900;
    mbr_entry_code = 0x0A84;

    filesystem_phs_addr = 0x8200;
    filesystem = 0x80000600;

    .text : AT(0xA000)
    {
        *(__start*);
        *(.text*);
    }

    .data :
    {
        *(.bss*);
        *(.data*);
        *(.rodata*);
        *(COMMON*);
    }

}