#include <stdint.h>

#ifndef SPRITES_H
#define SPRITES_H

class Sprite {
protected:
  int16_t prev_x, prev_y;
  uint8_t w, h;             // sprite width, height

public:
  int16_t x, y;             // current draw position
  const uint16_t* image;    // ptr to current image
  bool dirty;               // true if sprite needs to be redrawn

  Sprite(int16_t, int16_t, uint8_t, uint8_t, const unsigned short*);
  virtual void draw();
  virtual void update() = 0; // pure virtual function
  virtual void reset() = 0;
  void erasePrev();
  bool wasVisible();
  bool overlaps(const Sprite&);
  bool prevOverlaps(const Sprite&);
};

// Ball state machine
typedef enum {
  BALL_IDLE,       // hidden, waiting for bowler release frame
  BALL_BOWLING,    // travelling down the pitch
  BALL_HIT_LEFT,   // batted left
  BALL_HIT_RIGHT,  // batted right
  BALL_RESET       // short pause before next delivery
} BallState_t;

class Ball : public Sprite {
protected:
  // Positional variables using 4 fractional bit fixed point
  int16_t fx, fy;   // Fixed-point position
  int16_t vx, vy;   // Velocity
  int16_t ax;       // x acceleration for curve balls

public:
  BallState_t state;
  int16_t speed;    // ball speed (for y direction) in 4 bit fixed point
  uint8_t reset_timer;

  Ball();
  void bowl(int16_t, int16_t, int16_t, int16_t, int16_t);
  void hit(bool, int8_t);
  void draw();
  void update();
  void reset();
};

class Batsman : public Sprite {
protected:
  bool swinging;
  bool swing_left;
  uint8_t frame_idx;
  const uint16_t* idle_frame;
  const uint16_t** left_swing;
  const uint16_t** right_swing;
  uint8_t left_frames;
  uint8_t right_frames;

public:
  Batsman(const uint16_t*, const uint16_t**, const uint16_t**, uint8_t, uint8_t);
  void updatePos(uint32_t);
  void startSwing(bool);
  void update();
  void reset();
  bool isSwinging();
  bool isSwingingLeft();
};

class Bowler : public Sprite {
protected:
  uint8_t frame_idx;
  uint8_t tick;
  const uint16_t** frames;

public:
  Bowler(const uint16_t**);
  void update();
  void reset();
  bool isReleaseFrame();
};

class Stump : public Sprite {
protected:
  uint8_t frame_idx;
  const uint16_t** frames;

public:
  Stump(const uint16_t**);
  void update();
  void hit();
  void reset();
  bool isBroken();
};

#endif