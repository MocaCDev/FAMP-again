ENTRY(main)
SECTIONS {
    /* Program starts at 0x0700. */
    . = 0x0700;

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