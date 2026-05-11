#include "GameDefs.h"
#include "Sprites.h"

Bowler::Bowler(const uint16_t** anim)
    : Sprite(BOWLER_X, BOWLER_Y, BOWLER_W, BOWLER_H, anim[0]), frame_idx(0), tick(0), frames(anim) {}

void Bowler::update() {
    tick++;
    if (tick >= 4) { // Bowler animation at ~8 fps for 30Hz game loop
        tick = 0;
        frame_idx = (frame_idx + 1) % BOWLER_FRAMES;
        image = frames[frame_idx];
        dirty = true;
    }
}

void Bowler::reset() {
    frame_idx = 0;
    tick = 0;
    dirty = true;
}

bool Bowler::isReleaseFrame() {
    return frame_idx == BOWLER_RELEASE_FRAME && tick == 0;
}