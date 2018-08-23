#include "print.h"
#include "ltp1245.h"

static uint8_t Print_Buffer[PRINT_BUFFER_LINES * LTP1245_LINE_BYTES];

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

// Width needs to be divisible by 8
void Print_Image(const uint8_t *data, int width, int height, int scale)
{
    int currentline = 0;
    while(height)
    {
        int lines = PRINT_BUFFER_LINES / scale;
        if(height < lines)
        {
            lines = height;
        }

        memset(Print_Buffer, 0, sizeof(Print_Buffer));

        for(int i = 0; i < lines * scale; i++)
        {
            for(int j = 0; j < width * scale; j++)
            {
                int black = data[(currentline / scale * width + j / scale) / 8]
                    & (0x80 >> ((j / scale) % 8));
                if(black)
                {
                    int x = j;
                    x += (LTP1245_LINEWIDTH - width * scale) / 2;
                    Print_Buffer[i * LTP1245_LINE_BYTES + x / 8] |=
                    (0x80 >> (x % 8));
                }
            }
            currentline++;
        }

        LTP1245_Print(Print_Buffer, lines * scale);

        height -= lines;
    }
}