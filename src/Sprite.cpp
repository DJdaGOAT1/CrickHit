#include "../inc/ST7735.h"
#include "GameDefs.h"
#include "Renderer.h"
#include "Sprites.h"

Sprite::Sprite(int16_t x_pos, int16_t y_pos, uint8_t width, uint8_t height, const unsigned short* img)
    : x(x_pos), y(y_pos), prev_x(x_pos), prev_y(y_pos), w(width), h(height), image(img), dirty(true) {}

void Sprite::draw() {
    ComposeAndDraw(x, y, image, w, h);
    prev_x = x;
    prev_y = y;
    dirty = false;
}

void Sprite::erasePrev() {
    int16_t y_top = prev_y - h + 1;
    EraseToBackground(prev_x, y_top, w, h);
}

bool Sprite::wasVisible() {
    return (prev_x >= 0 && prev_x < SCREEN_W && prev_y >= BALL_H && prev_y < SCREEN_H);
}

bool Sprite::overlaps(const Sprite& other) {
    return (x < other.x + other.w && x + w > other.x && y > other.y - other.h && y - h < other.y);
}

bool Sprite::prevOverlaps(const Sprite& other) {
    return (prev_x < other.x + other.w && prev_x + w > other.x && prev_y > other.y - other.h && prev_y - h < other.y);
}