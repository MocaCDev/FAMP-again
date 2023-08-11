ENTRY(main)
SECTIONS {
    /* Program starts at 0x0500. */
    . = 0x7E00;

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