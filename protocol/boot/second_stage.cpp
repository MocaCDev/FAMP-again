#define OS_RELATED
#include <common.hpp>

void __START main()
{
    __asm__("mov ah, 0x0E");
    __asm__("mov al, 'S'");
    __asm__("int 0x10");
    
    while(1);
}