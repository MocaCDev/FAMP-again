ENTRY(main)
SECTIONS {
    /* Program starts at 0x0700. */
    . = 0x0700;

    /* MBR related addresses. */
    disk_image_heading = 0x0B00;
    mbr_entry_code = 0x0C84;

    .text : AT(0x0700)
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