#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    int width;
    const uint8_t *bitmap;
} FontGlyph_t;

typedef struct
{
    int height;
    int charoffset;
    int glyphcount;
    const FontGlyph_t *glyphs;
} Font_t;