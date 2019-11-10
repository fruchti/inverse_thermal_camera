#include "debug.h"

void Debug_Print(const char *message)
{
    uint32_t m[] = {2, (uint32_t)message, strlen(message)};
    __asm__("mov r0, #0x05;"
            "mov r1, %[m];"
            "bkpt #0xAB"
            :
            : [m] "r" (m)
            : "r0", "r1", "memory");
}

void Debug_PutChar(char c)
{
    uint32_t m[] = {2, (uint32_t)(&c), 1};
    __asm__("mov r0, #0x05;"
            "mov r1, %[m];"
            "bkpt #0xAB"
            :
            : [m] "r" (m)
            : "r0", "r1", "memory");
//    __asm__("mov r0, #0x03;"
//            "mov r1, %[msg];"
//            "bkpt #0xAB"
//            :
//            : [msg] "r" (&c)
//            : "r0", "r1");
}
