#include "../images/images.h"
#include "GameDefs.h"
#include "Sprites.h"

Batsman::Batsman(const uint16_t* idle, const uint16_t** l_swing, const uint16_t** r_swing, uint8_t l_frames, uint8_t r_frames)
    : Sprite(BATSMAN_CENTER_X, BATSMAN_Y, BATSMAN_W, BATSMAN_H, idle), swinging(false), swing_left(false), frame_idx(0),
      left_swing(l_swing), right_swing(r_swing), left_frames(l_frames), right_frames(r_frames) {}

void Batsman::updatePos(uint32_t pot_val) {
    x = (int16_t)(BATSMAN_X_MIN + (int32_t)pot_val * (BATSMAN_X_MAX - BATSMAN_X_MIN) / 4095);
    dirty = true;
}

void Batsman::startSwing(bool left) {
    if (!swinging) {
        swinging = true;
        swing_left = left;
        frame_idx = 0;
    }
}

void Batsman::update() {
    if (swinging) {
        frame_idx++;
        uint8_t max_idx = swing_left ? left_frames : right_frames;
        if (frame_idx >= max_idx) {
            swinging = false;
            image = batsman_idle;
            frame_idx = 0;
        } else {
            image = swing_left ? left_swing[frame_idx] : right_swing[frame_idx];
        }
        dirty = true;
    }
}

void Batsman::reset() {
    x = BATSMAN_CENTER_X;
    swinging = false;
    swing_left = false;
    frame_idx = 0;
    image = batsman_idle;
    dirty = true;
}

bool Batsman::isSwinging() {
    return swinging;
}

bool Batsman::isSwingingLeft() {
    return swing_left;
}