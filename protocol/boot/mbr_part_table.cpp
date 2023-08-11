//#define OS_RELATED
//#include <common.hpp>
//#include <FFF/FFF_structures.hpp>

//using namespace FFF_Structures;

void __attribute__((section("__start"))) main()
{
    __asm__("mov ah, 0x0E\n");
    __asm__("mov al, 'a'\n");
    __asm__("int 0x10");
    
    while(1);
}