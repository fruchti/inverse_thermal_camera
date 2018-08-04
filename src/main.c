#include "main.h"

int main(void)
{
    LTP1245_Init();

    // LTP1245_FeedPaper(10);
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH = (GPIOC->CRH
        & ~(0x0f << (4 * PIN_LED - 32)))
        | (0x01 << (4 * PIN_LED - 32))      // Output, max. 10 MHz
        ;

    char buff[30];
    itoa(BUILD_NUMBER, buff, 10);
    Print_Text(buff, &Messe_Duesseldorf_39);

    // LTP1245_FeedPaper(10);
    // Print_Text("Testy McTestFace", &Arpegius_32);
    // Print_Text("123555bcD", &Messe_Duesseldorf_39);
    Print_Text("This is a long line with very much text!", &Hannover_Messe_Serif_26);
    LTP1245_FeedPaper(10);
    LTP1245_Cut();

    for(;;)
    {
    }
}

