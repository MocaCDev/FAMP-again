ENTRY(main)
SECTIONS {
    /* Program starts at 0x0500. */
    . = 0x7E00;

    /* MBR related addresses. */
    disk_image_heading = 0x0B00;
    mbr_entry_code = 0x0C84; /* TODO: Do we need this? */
    filesystem = 0xF000;
    pentry = 0x1000;

    .text : AT(0x7E00)
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