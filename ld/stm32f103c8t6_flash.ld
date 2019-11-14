MEMORY
{
    flash (rx)                  : ORIGIN = 0x08000000, LENGTH = 64K
    sram (xrw)                  : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE ld/common.ld