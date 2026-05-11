#include "../inc/ST7735.h"
#include "../images/images.h"
#include "LED.h"
#include "GameDefs.h"
#include "Sprites.h"

Ball::Ball() : Sprite(-10, -10, BALL_W, BALL_H, ball), vx(0), vy(0), ax(0) {
    // Convert position to fixed point
    fx = x << BALL_FIXED_POINT;
    fy = y << BALL_FIXED_POINT;
}

void Ball::bowl(int16_t start_x, int16_t start_y, int16_t initial_vx, int16_t initial_vy, int16_t curve) {
    state = BALL_BOWLING;

    // Convert position to fixed point
    fx = start_x << BALL_FIXED_POINT;
    fy = start_y << BALL_FIXED_POINT;

    // velocities and accel are taken as fixed point
    vx = initial_vx;
    vy = initial_vy;
    ax = curve;
}

void Ball::hit(bool left, int8_t err) {
    int16_t speed_increase = 5 / (err + 1); // calculation for increased speed after hit
    if (vx < 0) { // take abs value of vx before applying speed increase
        vx *= -1;
    }

    // Set state and vx depending on direction of hit
    if (left) {
        state = BALL_HIT_LEFT;
        vx = -(speed + speed_increase);
    } else {
        state = BALL_HIT_RIGHT;
        vx = (speed + speed_increase);
    }
    vy = -((speed + speed_increase)>>1);
    ax = 0;
}

void Ball::update() {
    if (state == BALL_IDLE) {
      return;
    }

    // Handle delay after ball reset
    if(state == BALL_RESET){
        if(reset_timer > 0){
            LED_Off(0x07); // Turn off LEDs once ready for next delivery
            reset_timer--;
        } else {
            state = BALL_IDLE;
        }
        return;
    }

    dirty = true;

    prev_x = x;
    prev_y = y;

    vx += ax;
    
    fx += vx;
    fy += vy;

    x = fx >> BALL_FIXED_POINT;
    y = fy >> BALL_FIXED_POINT;

    // Check if ball hits screen boundaries
    if (y > SCREEN_H || y < -BALL_H || x < -BALL_W || x > SCREEN_W) {
        reset();
    }
}

void Ball::reset() {
    x = -10;
    y = -10;
    vx = 0;
    vy = 0;
    ax = 0;
    state = BALL_RESET;
    reset_timer = 15;
    dirty = true;
}

void Ball::draw() {
    if (x >= 0 && x < SCREEN_W && y >= BALL_H && y < SCREEN_H) {
        ST7735_DrawBitmap(x, y, image, w, h);
        prev_x = x;
        prev_y = y;
        dirty = false;
    }
}