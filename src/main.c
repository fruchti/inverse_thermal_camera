#include "main.h"

int main(void)
{
    USB_Init();
    LTP1245_Init();

    LTP1245_FeedPaper(10);

    uint8_t buff[LTP1245_LINEWIDTH / 8 * 16];
    for(int i = 0; i < sizeof(buff); i++)
    {
        int shift = (i * 8 / (64 * 6)) % 8;
        buff[i] = (0x11 << shift) | (0x11 >> (8 - shift));
    }

    LTP1245_Print(buff, 16);
    LTP1245_Print(buff, 5);
    LTP1245_FeedPaper(10);

    for(;;)
    {
    }
}

