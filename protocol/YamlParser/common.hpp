#ifndef YAML_PARSER_COMMON_H
#define YAML_PARSER_COMMON_H
#include <iostream>
#include <cstring>
#include <stdlib.h>

extern "C" {
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

    #ifndef true
    #define true                 0x01
    #endif

    #ifndef false
    #define false                0x00
    #endif
}

#define YAML_ASSERT(cond, msg, ...)        \
if(!(cond)) {                              \
fprintf(stderr, msg, ##__VA_ARGS__);       \
exit(EXIT_FAILURE);                        \
}

#endif
