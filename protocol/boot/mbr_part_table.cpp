#define OS_RELATED
#include <common.hpp>
#include <FFF/FFF_structures.hpp>

using namespace FFF_Structures;

void __START main()
{
    __asm__("mov ah, 0x0E");
    __asm__("mov al, 'A'");
    __asm__("int 0x10");
    
    while(true);
}