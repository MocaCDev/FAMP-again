ENTRY(main)
SECTIONS {
    /* Program starts at 0x0700. */
    . = 0x80000000;

    .text : AT(0x80000000)
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