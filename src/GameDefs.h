// GameDefs.h
// Shared constants, struct definitions, and enums for CricHit.
// Include this wherever game objects or layout constants are needed.
#ifndef GAME_DEFS_H
#define GAME_DEFS_H

#include <stdint.h>

// Screen
#define SCREEN_W 128
#define SCREEN_H 160

// Outfield background colour  (= ST7735_Color565(144, 238, 144))
#define BG_COLOR  ((uint16_t)0x9772)

// Pitch crease colour and main surface colour (used to hide crease
// bleed-through when compositing sprites over the pitch)
#define CREASE_COLOR        ((uint16_t)0xFFFF)
#define PITCH_SURFACE_COLOR ((uint16_t)0xB73D)

// Pitch bitmap (top-left origin; DrawBitmap uses bottom edge) 
// NOTE: ST7735_DrawBitmap(x, y, ...) uses x=left edge, y=BOTTOM edge.
#define PITCH_X   43
#define PITCH_Y   18
#define PITCH_W   35
#define PITCH_H   140

// Bowler 
#define BOWLER_X             58
#define BOWLER_Y             45
#define BOWLER_W             16
#define BOWLER_H             33
#define BOWLER_FRAMES         7
#define BOWLER_RELEASE_FRAME  4  // animation frame that releases the ball

// Ball 
#define BALL_W              6
#define BALL_H              6
#define BALL_RELEASE_X      (BOWLER_X + 4)          // ~62, near stump centre
#define BALL_RELEASE_Y      (BOWLER_Y - 20)         // y-bottom at release
#define BALL_FIXED_POINT    6
#define BALL_MAX_VX         (3 << BALL_FIXED_POINT) // Implicit minimum vx of 0 (straight bowl)
#define BALL_MAX_VY         (15 << BALL_FIXED_POINT)
#define BALL_MIN_VY         (5 << BALL_FIXED_POINT)
#define BALL_TARGET_X       ((STUMP_HIT_X1 + STUMP_HIT_X2)>>1)  // Middle x of stump hitbox
#define BALL_TARGET_Y       ((STUMP_Y + STUMP_HIT_Y) >> 1)      // Middle y of stump hitbox
#define BALL_TARGET_VAR     64 // Range in px around the stump where the targetX will be randomly selected for the ball's trajectory

// Stumps 
#define STUMP_X           55
#define STUMP_Y          159
#define STUMP_W           13
#define STUMP_H           13
#define STUMP_HIT_X1      55
#define STUMP_HIT_X2      68
#define STUMP_HIT_Y      146  // ball.y >= this means it reached stumps
#define STUMP_BROKEN_FRAMES 30  // frames (~1 s) to show broken stump

// Batsman 
#define BATSMAN_Y        145
#define BATSMAN_W         34
#define BATSMAN_H         46
#define STUMP_CENTER     (STUMP_X + (STUMP_W / 2))           // 61
#define BATSMAN_CENTER_X (STUMP_CENTER - (BATSMAN_W / 2))    // 44
#define BATSMAN_RANGE     40
#define BATSMAN_X_MIN    (BATSMAN_CENTER_X - BATSMAN_RANGE)  //  4
#define BATSMAN_X_MAX    (BATSMAN_CENTER_X + BATSMAN_RANGE)  // 84
#define SWING_COOLDOWN   1500  // ms between allowed swings

// Bat contact window (screen-absolute Y range)
#define BAT_HIT_Y1  99
#define BAT_HIT_Y2  145

// Ranges for number of runs awarded in pixels
#define SIX_THRESHOLD       3
#define FOUR_THRESHOLD      8
#define TWO_THRESHOLD       14
#define SWING_SIDE_PENALTY  2 // Threshold penalty if you swing on opposite side of ball

// Wide thresholds
#define WIDE_THRESHOLD   15  // px gap past batsman's right edge → off-wide
#define LEG_WIDE_MARGIN   3  // batsman must be this far right of ball → leg-wide

#endif // GAME_DEFS_H
