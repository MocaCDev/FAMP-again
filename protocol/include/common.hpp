#ifndef FAMP_PROTOCOL_COMMON_H
#define FAMP_PROTOCOL_COMMON_H

#ifndef OS_RELATED
#include <iostream>
#include <cstring>
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef char                 int8;
typedef unsigned char        uint8;
typedef int8*                pint8;
typedef uint8*               puint8;
typedef const int8           cint8;
typedef const uint8          cuint8;
typedef cint8*               cpint8;
typedef cuint8*              cpuint8;

typedef short                int16;
typedef unsigned short       uint16;
typedef int16*               pint16;
typedef uint16*              puint16;
typedef const int16          cint16;
typedef const uint16         cuint16;
typedef cint16*              cpint16;
typedef cuint16*             cpuint16;

typedef int                  int32;
typedef unsigned int         uint32;
typedef int32*               pint32;
typedef uint32*              puint32;
typedef const int32          cint32;
typedef const uint32         cuint32;
typedef cint32*              cpint32;
typedef cuint32*             cpuint32;

typedef long long            int64;
typedef unsigned long long   uint64;
typedef int64*               pint64;
typedef uint64*              puint64;
typedef const int64          cint64;
typedef const uint64         cuint64;
typedef cint64*              cpint64;
typedef cuint64*             cpuint64;

typedef int64                size;
typedef uint64               usize;
typedef const size           csize;
typedef const usize          cusize;

#ifdef __cplusplus
}
#endif

#ifndef __cplusplus
typedef uint8               bool;

#ifndef true
#define true                0x01
#endif

#ifndef false
#define false               0x00
#endif
#endif

/* `OS_RELATED` will be defined in C/C++ programs that are a part of the OS. */
#ifndef OS_RELATED
#define FAMP_ASSERT(cond, msg, ...)         \
if(!(cond)) {                               \
    fprintf(stderr, msg, ##__VA_ARGS__);    \
    exit(EXIT_FAILURE);                     \
}

#define FAMP_LOG(msg, ...)                  \
fprintf(stderr, msg, ##__VA_ARGS__);

#define FAMP_ERROR(msg, ...)                \
FAMP_LOG(msg, ##__VA_ARGS__)                \
exit(EXIT_FAILURE);
#endif

template<typename T>
#ifndef OS_RELATED
    requires (std::is_same<T, uint16>::value
        || std::is_same<T, uint32>::value)
        && (!std::is_same<T, uint8>::value)
#endif
#ifndef OS_RELATED
T revert_value(T &value)
#else
T revert_value(T value)
#endif
{
    #ifdef OS_RELATED
    if(sizeof(T) == 1) return (T) value & 0xFF;
    #endif

    T old_value = value;
    value ^= value;

    switch(sizeof(T))
    {
        case 2: {
            value |= (value << 0) | ((old_value >> 0) & 0xFF);
            value = (value << 8) | ((old_value >> 8) & 0xFF);
            break;
        }
        case 4: {
            value |= (value << 0) | ((old_value >> 0) & 0xFF);
            value = (value << 8) | ((old_value >> 8) & 0xFF);
            value = (value << 8) | ((old_value >> 16) & 0xFF);
            value = (value << 8) | ((old_value >> 24) & 0xFF);
            break;
        }
        default: break;
    }

    return (T) value;
}

#ifdef OS_RELATED
#define __START __attribute__((section("__start")))

#ifndef BIT32_PROGRAM
asm(".code16gcc");
#endif

#ifndef BIT32_PROGRAM
void read_in_memory(uint16 addr, uint8 start_sector, uint8 sector_amount)
{
    uint16 a = (0x02 << 8) | sector_amount;

    /* Read from disk. This will be replaced when ATA PIO support is added. */
    __asm__("mov ax, %0" : : "dN"((uint16)addr));
    __asm__("mov es, ax\nxor bx, bx\nmov ax, %0" : : "d"((uint16) a));
    __asm__("mov ch, 0x00\nmov cl, %0" : : "dN"((uint8) start_sector));
    __asm__("mov dh, 0x00\nmov dl, 0x80\nint 0x13");

    return;
}
#endif
#endif

#endif