// Renderer.h
// Background-compositing sprite renderer for CricHit.
#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

// Draw a sprite over the correct pitch/outfield background in one SPI burst.
// Pixels matching BG_COLOR are treated as transparent.
// x = left edge, y = bottom edge  (same convention as ST7735_DrawBitmap).
void ComposeAndDraw(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h);

// Erase a rectangle back to pitch/outfield background.
void EraseToBackground(int16_t x, int16_t y_top, int16_t w, int16_t h);

#endif // RENDERER_H
