// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Devansh Joshi and Calum Cheah
// Last Modified: January 12, 2026
//
// File layout:
//   GameDefs.h   – all #defines, Ball/Batsman struct definitions
//   Renderer.h/cpp – ComposeAndDraw, EraseToBackground
//   Screens.h/cpp  – HomeScreen, InstructionScreen, language data
//   Lab9HMain.cpp  – globals, ISR handlers, game logic, main()

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "../images/images.h"
#include "GameDefs.h"
#include "Renderer.h"
#include "Screens.h"
#include "Sprites.h"
extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG0_IRQHandler(void);
extern "C" void TIMG12_IRQHandler(void);

void PLL_Init(void){
  // Clock_Init40MHz();
  Clock_Init80MHz(0);
}

SlidePot Sensor(1587, 192);

// Random number generator 
uint32_t M = 1;
uint32_t Random32(void){
  M = 1664525*M + 1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32() >> 16) % n;
}

// Use central limit theorem to approximate a gaussian distribution by averaging 2 random samples
int16_t GaussianRandom(int16_t center, int16_t range) {
  int32_t sum = (int32_t) Random(range) + Random(range);
  sum -= (int32_t)(range); // Shift center back to zero after summing the samples

  return center + (int16_t)(sum >> 1); // divide sum by 2 to average, add variance to center
}

// Pointers to image arrays for animations
const uint16_t* bowler_frames[] = {bowler1, bowler2, bowler3, bowler4, bowler5, bowler6, bowler7};
const uint16_t* stump_frames[] = {stump1, stump2};
const uint16_t* left_swing_frames[] = {batsman_idle, left_batsman1, left_batsman1, left_batsman2,
  left_batsman3, left_batsman3, left_batsman3, right_batsman1};
const uint16_t* right_swing_frames[] = {batsman_idle, right_batsman1, right_batsman1, right_batsman1,
    right_batsman2, right_batsman3, right_batsman4, right_batsman4,
    right_batsman4, right_batsman1};

// Global class instances
Ball cricket_ball;
Bowler bowler(bowler_frames);
Stump stump(stump_frames);
Batsman batsman(batsman_idle, left_swing_frames, right_swing_frames, 8, 10);

// Game state globals
volatile bool flag        = false;
volatile bool game_paused = false;
uint8_t stump_timer = 0;
bool has_swung = false;
uint32_t swing_time = 0;

// flags for game/sound events
bool event_wicket = false;
bool event_hit    = false;
bool event_wide   = false;
bool wide_called  = false;
bool wide_scored  = false;

uint32_t score             = 0;
uint32_t score_prev_digits = 1;

//TExaS logic analyser probe 
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80 | ((GPIOB->DOUT31_0 >> 26) & 0x03));
}

void checkCollisions() {
  if (cricket_ball.state != BALL_BOWLING) {
    return;
  }

  bool in_bat_x = (cricket_ball.x >= batsman.x && cricket_ball.x <= batsman.x + BATSMAN_W);
  bool in_bat_y = (cricket_ball.y >= BAT_HIT_Y1 && cricket_ball.y <= BAT_HIT_Y2);

  // Wide detection (fires once per delivery, only when ball reaches batsman)
  if(cricket_ball.y >= BATSMAN_Y && !wide_called){
    wide_called = true;
    bool leg_wide = (cricket_ball.x < PITCH_X) && ((batsman.x - cricket_ball.x) > LEG_WIDE_MARGIN);
    bool off_wide = (cricket_ball.x - (batsman.x + BATSMAN_W)) >= WIDE_THRESHOLD;
    if(leg_wide || off_wide){
      event_wide  = true;
      wide_scored = true;
      score += 1;
    }
  }

  // Bat contact
  if(in_bat_x && in_bat_y && batsman.isSwinging()){
    if(wide_scored){ score -= 1; wide_scored = false; event_wide = false; }
    event_hit = true;
    int8_t err = ((BAT_HIT_Y1 + BAT_HIT_Y2) / 2) - cricket_ball.y;
    if(err < 0) err = -err;
    cricket_ball.hit(batsman.isSwingingLeft(), err);

    // Add speed-proportional jitter to break quantisation at high speeds.
    // At fast ball speeds the large step size means only a few Y positions
    // are reachable in the hit zone, locking scores to one value.
    // Jitter range scales with speed so all scores (0-6) stay possible.
    int16_t speed_px = cricket_ball.speed >> BALL_FIXED_POINT;
    int8_t jitter = (int8_t)(Random(speed_px + 1)) - (speed_px >> 1);
    int8_t adj_err = err + jitter;
    if(adj_err < 0) adj_err = -adj_err;

    // Check if ball position relative to batsman matches the swing side
    int16_t x_err = (cricket_ball.x + (BALL_W >> 1)) - (batsman.x + (BATSMAN_W >> 1));

    // Add penalty if ball is not on same side as swing direction (centered ball is not penalized)
    int8_t threshold_modifier = 0;
    if (x_err != 0 && (x_err < 0) != batsman.isSwingingLeft()) {
      threshold_modifier -= SWING_SIDE_PENALTY;
    }

    if(adj_err <=  SIX_THRESHOLD + threshold_modifier){ score += 6; LED_On(0x04); }
    else if(adj_err <=  FOUR_THRESHOLD + threshold_modifier){ score += 4; LED_On(0x02); }
    else if(adj_err <= TWO_THRESHOLD + threshold_modifier){ score += 2; LED_On(0x01); }
  }

  // Stump collision
  bool in_stump_x = (cricket_ball.x >= STUMP_HIT_X1 && cricket_ball.x <= STUMP_HIT_X2);
  if(in_stump_x && cricket_ball.y >= STUMP_HIT_Y) {
    if(wide_scored){ score -= 1; wide_scored = false; event_wide = false; }
    event_wicket = true;
    stump.hit();
    stump_timer = STUMP_BROKEN_FRAMES;
    cricket_ball.reset();
    cricket_ball.reset_timer = STUMP_BROKEN_FRAMES;
  }
}

// Swing-cooldown ISR (1 kHz, TimerG0) 
void TIMG0_IRQHandler(void){
  if((TIMG0->CPU_INT.IIDX) == 1){
    swing_time++;
    if(swing_time > SWING_COOLDOWN){
      has_swung  = false;
      swing_time = SWING_COOLDOWN;
    }
  }
}

// 30 Hz game-tick ISR (TimerG12) 
void TIMG12_IRQHandler(void){
  if((TIMG12->CPU_INT.IIDX) == 1){
    GPIOB->DOUTTGL31_0 = GREEN;
    GPIOB->DOUTTGL31_0 = GREEN;

    if(!game_paused){
      // Batsman x position from PB18 slide pot
      batsman.updatePos(Sensor.In());

      // Update all objects
      batsman.update();
      cricket_ball.update();
      stump.update();

      // Bowler animation & ball release
      if(cricket_ball.state == BALL_IDLE){
        bowler.update();
        if(bowler.isReleaseFrame()){
          int16_t vx = (int16_t)((Random((BALL_MAX_VX<<1) + 1)) - BALL_MAX_VX); // Get random vx

          // Vary targetX to get occasional misses
          int16_t variance = BALL_TARGET_VAR - (cricket_ball.speed >> BALL_FIXED_POINT);
          if (variance < 20) variance = 20; // Put minimum on spread
          int16_t targetX = GaussianRandom(BALL_TARGET_X, variance) << BALL_FIXED_POINT;

          // Adjust ax to curve towards stump
          // ax = (2*((targetX-initialX) - vx*t)) / (t*t)
          // t = number of frames it will take for ball to hit stump
          int16_t frames = (int16_t) ((BALL_TARGET_Y - BALL_RELEASE_Y) / (cricket_ball.speed >> BALL_FIXED_POINT));
          int16_t ax = (2*((targetX - (BALL_RELEASE_X<<BALL_FIXED_POINT)) - (vx*frames))) / (frames*frames);

          cricket_ball.bowl(BALL_RELEASE_X, BALL_RELEASE_Y, vx, cricket_ball.speed, ax);
          wide_called = false;
          wide_scored = false;
        }
      }

      // Stump broken-frame timer
      if(stump.isBroken() && stump_timer > 0){
        if(--stump_timer == 0) stump.reset();
      }

      checkCollisions();

      GPIOB->DOUTTGL31_0 = GREEN;
    } // end if(!game_paused)

    flag = true;  // always signal main loop, even while paused
  }
}

// Test mains (rename main1/2/3/4 - main to use) 
int main1(void){
    char l;
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
  ST7735_FillScreen(0x0000);
  for(int myPhrase = 0; myPhrase <= 2; myPhrase++){
    for(int myL = 0; myL <= 3; myL++){
      ST7735_OutString((char *)Phrases[myLanguage][myL]);
      ST7735_OutChar(' ');
      ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j = 0; j < 3; j++){
      for(int i = 0; i < 16; i++){
        ST7735_SetCursor(7*j+0, i); ST7735_OutUDec(l);
        ST7735_OutChar(' '); ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4, i); ST7735_OutChar(l);
        l++;
      }
    }
  }
}

int main2(void){
  __disable_irq(); PLL_Init(); LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18, 8);
  ST7735_DrawBitmap(53, 151, Bunker0, 18, 5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18, 8);
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18, 8);
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18, 8);
  ST7735_DrawBitmap( 0,   9, SmallEnemy10pointA, 16, 10);
  ST7735_DrawBitmap(20,   9, SmallEnemy10pointB, 16, 10);
  ST7735_DrawBitmap(40,   9, SmallEnemy20pointA, 16, 10);
  ST7735_DrawBitmap(60,   9, SmallEnemy20pointB, 16, 10);
  ST7735_DrawBitmap(80,   9, SmallEnemy30pointA, 16, 10);
  for(uint32_t t = 500; t > 0; t -= 5){
    SmallFont_OutVertical(t, 104, 6);
    Clock_Delay1ms(50);
  }
  ST7735_FillScreen(0x0000);
  ST7735_SetCursor(1, 1); ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2); ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3); ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4); ST7735_OutUDec(1234);
  while(1){}
}

int main3(void){
  __disable_irq(); PLL_Init(); LaunchPad_Init();
  Switch_Init(); LED_Init();
  while(1){
    uint32_t in = Switch_In();
    if(in & 2) LED_On(0x1); else LED_Off(0x1);
    if(in & 8) LED_On(0x2); else LED_Off(0x2);
  }
}

int main4(void){ uint32_t last = 0, now;
  __disable_irq(); PLL_Init(); LaunchPad_Init();
  Switch_Init(); LED_Init(); Sound_Init();
  TExaS_Init(ADC0, 6, 0);
  __enable_irq();
  while(1){
    now = Switch_In();
    if((last==0)&&(now==1)) Sound_IPLStadium();
    if((last==0)&&(now==2)) Sound_StumpHit();
    if((last==0)&&(now==4)) Sound_BatHit();
    last = now;
  }
}

// ResetGameState 
static void ResetGameState(void){
  score = 0; score_prev_digits = 1;
  cricket_ball.reset();
  cricket_ball.state = BALL_IDLE;
  cricket_ball.speed = BALL_MIN_VY;
  cricket_ball.reset_timer = 0;
  bowler.reset();
  batsman.reset();
  stump.reset();
  stump_timer = 0;
  event_wicket = false; event_hit = false; event_wide = false;
  wide_called = false; wide_scored = false; has_swung = false;
  swing_time = 0; flag = false; game_paused = false;
}

// Main 
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN (or functions called from main)
int main(void){
  __disable_irq();
  PLL_Init();
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);

  Sensor.Init();
  Switch_Init();
  LED_Init();
  Sound_Init();
  TExaS_Init(0, 0, &TExaS_LaunchPadLogicPB27PB26);

  Speaker_Enable();

  TimerG0_IntArm(1000, 40, 0);              // 1 kHz swing cooldown
  TimerG12_IntArm(80000000 / 30, 2);        // 30 Hz game tick
  __enable_irq();

  // Seed RNG
  M = Sensor.In();
  M ^= SysTick->VAL;

  while(1){  // outer loop: home - game - repeat
    HomeScreen();         // plays IPL audio; blocks until language chosen
    InstructionScreen();  // shows controls; blocks until any switch pressed
    ResetGameState();

    ST7735_FillScreen(BG_COLOR);
    ST7735_DrawBitmap(PITCH_X, PITCH_Y + PITCH_H - 1, pitch, PITCH_W, PITCH_H);

    // Per-round locals (re-initialised every cycle)
    int16_t prev_batsman_x = batsman.x;
    const unsigned short* prev_batsman_img = nullptr;
    uint8_t prev_bowler_idx = 0xFF;
    uint8_t prev_stump_idx = 0xFF;
    uint8_t wide_display = 0;
    bool wide_shown = false;
    uint32_t prev_in = 0;
    bool back_to_home = false;
    uint8_t out_delay = 0;        // countdown before showing OUT popup

    while(!back_to_home){

      while(flag == 0);  // wait for 30 Hz tick (set even when paused)
      flag = false;

      // ISR events
      if(event_wicket){
        event_wicket = false;
        Sound_StumpHit();
        out_delay = STUMP_BROKEN_FRAMES;  // start countdown, let game keep rendering
        // Revoke wide display if the ball curved back into the stump
        if(!wide_scored){ wide_display = 0; }
      }
      if(event_hit){
        event_hit = false; Sound_BatHit();
        if(!wide_scored){ wide_display = 0; }
      }
      if(event_wide)  { event_wide = false; wide_display = 60; }

      // Score display
      uint8_t  lbl_len = (myLanguage == Spanish) ? 9 : 7;
      uint32_t score_digs = (score < 10) ? 1 : (score < 100) ? 2 : (score < 1000) ? 3 : 4;
      if(score_digs < score_prev_digits)
        ST7735_FillRect((lbl_len + score_digs) * 6, 0,(score_prev_digits - score_digs) * 6, 10, BG_COLOR);
      score_prev_digits = score_digs;
      ST7735_SetCursor(0, 0);
      if(myLanguage == Spanish) printf("Puntaje: %d", score);
      else printf("Score: %d",   score);

      // Wide / Ancha display
      if(wide_display > 0){
        wide_display--;
        if(!wide_shown){
          ST7735_SetCursor(0, 1);
          if(myLanguage == Spanish) printf("Ancha!"); else printf("Wide!");
          wide_shown = true;
        }
      } else if(wide_shown){
        ST7735_FillRect(0, 10, 36, 10, BG_COLOR);
        wide_shown = false;
      }

      uint32_t in = Switch_In();

      // TOP+BOT = home;  L+R = pause toggle
      bool tb = (in & 0x5) == 0x5;
      bool lr = (in & 0xA) == 0xA;
      bool lr_was = (prev_in & 0xA) == 0xA;

      if(tb){ back_to_home = true; break; }

      if(lr && !lr_was){
        game_paused = !game_paused;
        if(game_paused){
          DrawPausePopup();
        } else {
          ST7735_FillScreen(BG_COLOR);
          ST7735_DrawBitmap(PITCH_X, PITCH_Y + PITCH_H - 1, pitch, PITCH_W, PITCH_H);
          prev_bowler_idx = 0xFF;
          prev_stump_idx = 0xFF;
          prev_batsman_img = nullptr;
          wide_shown = false;
          bowler.dirty = true;
          stump.dirty = true;
          batsman.dirty = true;
          if(cricket_ball.state != BALL_IDLE) cricket_ball.dirty = true;
        }
      }
      uint32_t prev_in_snap = prev_in;
      prev_in = in;

      if(game_paused) continue;

      // Speed controls (TOP alone / BOT alone)
      if((in & 1) && !(prev_in_snap & 1) && cricket_ball.speed < BALL_MAX_VY) cricket_ball.speed+=(1<<BALL_FIXED_POINT);
      if((in & 4) && !(prev_in_snap & 4) && cricket_ball.speed > BALL_MIN_VY) cricket_ball.speed-=(1<<BALL_FIXED_POINT);

      // Swing (guard against L+R combo)
      if(!lr && !batsman.isSwinging() && !has_swung){
        if(in & 2){
          batsman.startSwing(true);
          has_swung = true;
          swing_time = 0;
        }
        else if(in & 8){
          batsman.startSwing(false);
          has_swung = true;
          swing_time = 0;
        }
      }

      // Speaker mute controls
      if(LaunchPad_InS1()==0x00040000 && LaunchPad_InS2()==0) Speaker_Enable();
      if(LaunchPad_InS2()==0x00200000 && LaunchPad_InS1()==0) Speaker_Disable();

      // Snapshot batsman state (avoid ISR race)
      int16_t snap_x = batsman.x;
      const unsigned short* snap_img = batsman.image;

      // Erase ball; mark overlapped sprites dirty
      // Use prevOverlaps because the erase happens at the ball's previous
      // position, not its current one — otherwise sprites that the erase
      // painted over won't be redrawn (background "trail" artifacts).
      if(cricket_ball.wasVisible()){
        cricket_ball.erasePrev();
        if (!bowler.dirty && cricket_ball.prevOverlaps(bowler)) {
          bowler.dirty = true;
        }
        if (!batsman.dirty && cricket_ball.prevOverlaps(batsman)) {
          batsman.dirty = true;
        }
        if (!stump.dirty && cricket_ball.prevOverlaps(stump)) {
          stump.dirty = true;
        }
      }

      if (bowler.dirty){
        bowler.draw();
      }
      if (stump.dirty){
        stump.draw();
      }
      if (batsman.dirty){
        if(snap_x != prev_batsman_x){
          int16_t top = BATSMAN_Y - BATSMAN_H + 1;
          if(snap_x > prev_batsman_x){
            int16_t sw = snap_x - prev_batsman_x; if(sw > BATSMAN_W) sw = BATSMAN_W;
            EraseToBackground(prev_batsman_x, top, sw, BATSMAN_H);
          } else {
            int16_t sw = prev_batsman_x - snap_x; if(sw > BATSMAN_W) sw = BATSMAN_W;
            EraseToBackground(prev_batsman_x + BATSMAN_W - sw, top, sw, BATSMAN_H);
          }
        }
        batsman.draw();
        prev_batsman_x   = snap_x;
        prev_batsman_img = snap_img;
      }

      // Ball drawn last so it appears on top
      cricket_ball.draw();

      // Delayed OUT screen: count down while the game keeps rendering
      // (broken stump visible, batsman swing finishing, etc.)
      // Show popup once timer expires AND batsman is back to idle.
      if(out_delay > 0){
        out_delay--;
        if(out_delay == 0 && !batsman.isSwinging()){
          game_paused = true;

          OutChoice_t choice = OutScreen();

          if(choice == OUT_CONTINUE){
            stump.reset();
            stump_timer = 0;
            score = 0;
            cricket_ball.speed = BALL_MIN_VY;
            cricket_ball.state = BALL_IDLE;
            cricket_ball.reset_timer = 0;
            bowler.reset();
            ST7735_FillScreen(BG_COLOR);
            ST7735_DrawBitmap(PITCH_X, PITCH_Y + PITCH_H - 1, pitch, PITCH_W, PITCH_H);
            bowler.dirty = true;   bowler.draw();
            stump.dirty = true;    stump.draw();
            batsman.dirty = true;  batsman.draw();
            prev_batsman_x = batsman.x;
            prev_batsman_img = batsman.image;
            wide_shown = false;
            game_paused = false;
          } else {
            back_to_home = true;
          }
          if(back_to_home) break;
          continue;
        } else if(out_delay == 0){
          out_delay = 1;
        }
      }

    } // end game loop
  } // end outer loop
}
