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


    char buff[30] = "Build No. ";
    itoa(BUILD_NUMBER, buff + strlen(buff), 10);
    Print_Text(buff, &Messe_Duesseldorf_39);

    LTP1245_FeedPaper(2);

    strcpy(buff, "Line count: ");
    extern int LineCount;
    itoa(LineCount, buff + strlen(buff), 10);
    Print_Text(buff, &Hannover_Messe_Serif_26);
    
    LTP1245_FeedPaper(10);
    // LTP1245_FeedPaper(10);
    // LTP1245_Cut();

    for(;;)
    {
    }
}

