#include "main.h"

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH = (GPIOC->CRH
        & ~(0x0f << (4 * PIN_LED - 32)))
        | (0x01 << (4 * PIN_LED - 32))      // Output, max. 10 MHz
        ;

    Camera_Init();
    LTP1245_Init();
    
    LTP1245_FeedPaper(100);
    LTP1245_FeedPaper(10);
    // LTP1245_Cut();

    while(!Camera_Captured);

    extern uint8_t ImageBuffer[CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT / 8];
    Print_Image(ImageBuffer, CAMERA_IMAGE_WIDTH, CAMERA_IMAGE_HEIGHT, 2);

    LTP1245_FeedPaper(100);
    LTP1245_FeedPaper(10);

    for(;;)
    {
        __WFI();
    }
}

