#include "print.h"
#include "ltp1245.h"

static uint8_t Print_Buffer[64 * LTP1245_LINE_BYTES];

void Print_Text(const char *text, const Font_t *font)
{
    int height = font->height;
    memset(Print_Buffer, 0, LTP1245_LINE_BYTES * height);
    char c;
    int xpos = 0;
    while((c = *text++) != 0)
    {
        if(c > font->glyphcount + font->charoffset
            || c < font->charoffset)
        {
            continue;
        }

        int width = font->glyphs[c - font->charoffset].width;
        const uint8_t *bitmap = font->glyphs[c - font->charoffset].bitmap;

        for(int i = 0; i < width * height; i++)
        {
            int x = i / height + xpos;
            int y = i % height;
            if(bitmap[i / 8] & (0x80 >> (i % 8)))
            {
                // Pixel is black
                Print_Buffer[y * LTP1245_LINE_BYTES + x / 8] |=
                    0x80 >> (x % 8);
            }
        }

        xpos += width;
    }

    LTP1245_Print(Print_Buffer, height);
}