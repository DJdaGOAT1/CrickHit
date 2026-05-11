// Renderer.cpp
// Background-compositing sprite renderer for CricHit.
// Handles transparent-pixel blending against the pitch bitmap and outfield.

#include "../inc/ST7735.h"
#include "../images/images.h"
#include "GameDefs.h"
#include "Renderer.h"

// Sized for the largest sprite: batsman 34x46 = 1564 pixels
static uint16_t compose_buf[BATSMAN_W * BATSMAN_H];

// Returns the correct background pixel at screen coordinate (sx, sy).
// Inside the pitch rectangle: returns the corresponding pitch bitmap pixel.
// Outside: returns BG_COLOR (light-green outfield).
static inline uint16_t BackgroundAt(int16_t sx, int16_t sy) {
  if(sx >= PITCH_X && sx < PITCH_X + PITCH_W &&
     sy >= PITCH_Y && sy < PITCH_Y + PITCH_H) {
    int16_t bmp_row = (PITCH_Y + PITCH_H - 1) - sy;
    return pitch[bmp_row * PITCH_W + (sx - PITCH_X)];
  }
  return BG_COLOR;
}

// Compose sprite + correct background into compose_buf, then draw as one
// SPI burst — same throughput as ST7735_DrawBitmap with zero per-pixel overhead.
void ComposeAndDraw(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h) {
  for(int16_t r = 0; r < h; r++) {
    int16_t sy = y - r;

    // Find the leftmost and rightmost opaque pixels in this row.
    // Transparent pixels *between* them are interior (e.g. between
    // legs) and should not reveal crease lines through the sprite.
    int16_t left = -1, right = -1;
    for(int16_t c = 0; c < w; c++) {
      if(image[r * w + c] != BG_COLOR) {
        if(left < 0) left = c;
        right = c;
      }
    }

    for(int16_t c = 0; c < w; c++) {
      uint16_t pix = image[r * w + c];
      if(pix == BG_COLOR) {
        uint16_t bg = BackgroundAt(x + c, sy);
        // Only mask crease for interior transparent pixels so the
        // crease still shows around the sprite outline.
        if(left >= 0 && c > left && c < right && bg == CREASE_COLOR)
          bg = PITCH_SURFACE_COLOR;
        compose_buf[r * w + c] = bg;
      } else {
        compose_buf[r * w + c] = pix;
      }
    }
  }
  ST7735_DrawBitmap(x, y, compose_buf, w, h);
}

// Erase a rectangle back to pitch/outfield background.
// Uses FillRect for the off-pitch area, then restores pitch pixels where
// the rectangle overlaps the pitch bitmap.
void EraseToBackground(int16_t x, int16_t y_top, int16_t w, int16_t h) {
  ST7735_FillRect(x, y_top, w, h, BG_COLOR);
  // Clamp intersection with pitch rectangle
  int16_t ix1 = x         > PITCH_X             ? x             : PITCH_X;
  int16_t iy1 = y_top     > PITCH_Y             ? y_top         : PITCH_Y;
  int16_t ix2 = (x + w)   < (PITCH_X + PITCH_W) ? (x + w)      : (PITCH_X + PITCH_W);
  int16_t iy2 = (y_top+h) < (PITCH_Y + PITCH_H) ? (y_top + h)  : (PITCH_Y + PITCH_H);
  if(ix1 >= ix2 || iy1 >= iy2) return;
  int16_t pitch_bottom = PITCH_Y + PITCH_H - 1;
  for(int16_t sy = iy1; sy < iy2; sy++) {
    int16_t bmp_row = pitch_bottom - sy;
    for(int16_t sx = ix1; sx < ix2; sx++) {
      ST7735_DrawPixel(sx, sy, pitch[bmp_row * PITCH_W + (sx - PITCH_X)]);
    }
  }
}
