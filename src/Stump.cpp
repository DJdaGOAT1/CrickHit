#include "GameDefs.h"
#include "Sprites.h"

Stump::Stump(const uint16_t** anim)
    : Sprite(STUMP_X, STUMP_Y, STUMP_W, STUMP_H, anim[0]), frame_idx(0), frames(anim) {}

void Stump::update() {
    image = frames[frame_idx];
    dirty = true;
}

void Stump::hit() {
    frame_idx = 1;
    dirty = true;
}

void Stump::reset() {
    frame_idx = 0;
    dirty = true;
}

bool Stump::isBroken() {
    return frame_idx == 1;
}