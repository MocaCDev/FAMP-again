GDT:
            .null_desc1          dq 0x0

            ; 32-bit code segment
            .code32_limit1       dw 0xFFFF                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            .code32_base1        dw 0                        ; base (bits 0-15) = 0x0
            .code32_base21       db 0                        ; base (bits 16-23)
            .code32_acces1       db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
            .code32_gran1        db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            .code32_base_high1   db 0                        ; base high

            ; 32-bit data segment
            .data32_limit1       dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            .data32_base1        dw 0                        ; base (bits 0-15) = 0x0
            .data32_base21       db 0                        ; base (bits 16-23)
            .data32_access1      db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
            .data32_gran1        db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            .data32_base_high1   db 0                       ; base high

GDTDesc:
    dw GDTDesc - GDT - 1
    dd GDT

global __load_gdt
use16
__load_gdt:
    in al, 0x92
    or al, 0x02
    out 0x92, al

    cli
    lgdt [GDTDesc]
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    ret

    jmp word 0x8:__prepare_jump_to_pmode

global __prepare_jump_to_pmode
use32
__prepare_jump_to_pmode:
    ;jmp word 0x8:.__set_registers
;.__set_registers:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;jmp 0x08:0x0700
    jmp 0x8:test

use32
test:
    mov word [0xb8000], 'a'

    hlt
    jmp $