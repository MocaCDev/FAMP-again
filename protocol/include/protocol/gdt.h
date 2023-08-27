#ifndef FAMP_PROTOCOL_GDT_H
#define FAMP_PROTOCOL_GDT_H

namespace FampGDT
{
    struct gdt_description
    {
        uint16 size;
        uint32 address;
    };

    struct gdt
    {
        /* 32-bit: null descriptor. */
        unsigned long long  null_desc;

        /* 32-bit: code segment. */
        uint16              code32_limit;          // 0xFFFFF for full 32-bit range
        uint16              code32_base;
        uint8               code32_base2;
        uint8               code32_access;         // access (present, ring 0, code segment, executable, direction 0, readable)
        uint8               code32_gran;           // granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
        uint8               code32_base_high;

        /* 32-bit: data segment. */
        uint16              data32_limit;         // 0xFFFFF for full 32-bit range
        uint16              data32_base;
        uint8               data32_base2;
        uint8               data32_access;        // access (present, ring 0, code segment, executable, direction 0, readable)
        uint8               data32_gran;          // granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
        uint8               data32_base_high;

        /* 16-bit: code segment. */
        uint16              code16_limit;         // should be 0xFFFFF(same as 32-bit code/data segments)
        uint16              code16_base;
        uint8               code16_base2;
        uint8               code16_access;       // access (present, ring 0, code segment, executable, direction 0, readable)
        uint8               code16_gran;         // granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
        uint8               code16_base_high;

        /* 16-bit: data segment. */
        uint16              data16_limit;         // should be 0xFFFFF(same as 32-bit code/data segments)
        uint16              data16_base;
        uint8               data16_base2;
        uint8               data16_access;       // access (present, ring 0, code segment, executable, direction 0, readable)
        uint8               data16_gran;         // granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
        uint8               data16_base_high;
    };
}

#endif