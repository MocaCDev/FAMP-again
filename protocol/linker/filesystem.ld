ENTRY(main)
SECTIONS {
    /* Program starts at 0x0700. */
    . = 0xF000;

    .text : AT(0xF000)
    {
    }

    .data :
    {
        *(.bss*);
        *(.data*);
        *(.rodata*);
        *(COMMON*);
    }

}