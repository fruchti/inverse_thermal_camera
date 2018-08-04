#include "main.h"

int main(void)
{
    LTP1245_Init();

    // LTP1245_FeedPaper(10);

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

