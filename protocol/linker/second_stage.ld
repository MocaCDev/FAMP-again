ENTRY(main)
SECTIONS {
    /* Program starts at 0x0500. */
    . = 0x7E00;

    /* MBR related addresses. */
    disk_image_heading = 0x0900;
    mbr_entry_code = 0x0A84;

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