// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "../sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

#define PB12INDEX 28
#define SOUND_PERIOD 7256


void SysTick_IntArm(uint32_t period, uint32_t priority) {
  SysTick->CTRL = 0;
  SysTick->LOAD = period - 1;
  SCB->SHP[1] = (SCB->SHP[1] & (~0xC0000000)) | priority << 30;
  SysTick->VAL = 0;
  SysTick->CTRL = 0x07;
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
  IOMUX->SECCFG.PINCM[PB12INDEX] = (uint32_t) 0x81;
  GPIOB-> DOE31_0 |= (1<<12);
  DAC5_Init();
  SysTick_IntArm(SOUND_PERIOD, 2);
 
}
static const uint8_t *sound_pt = nullptr;
static uint32_t sound_index = 0;
static uint32_t sound_length = 0;

extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  if(sound_pt != nullptr && sound_index < sound_length){
    DAC5_Out(sound_pt[sound_index]);
    sound_index++;
  } else {
    DAC5_Out(16); // mid-rail silence
    sound_pt = nullptr;
  }
}

//******* Sound_Start ************
// Sets the active sound pointer and length so the SysTick ISR will play
// it once and stop.  Calling this while a sound is playing interrupts it.
// Input: pt    pointer to array of DAC output values
//        count number of samples in the array
void Sound_Start(const uint8_t *pt, uint32_t count){
  sound_index = 0;
  sound_length = count;
  sound_pt = pt;  // assign last — ISR checks this pointer first
}

void Speaker_Enable() {
  GPIOB->DOUTCLR31_0 = (1<<12);
}

void Speaker_Disable() {
  GPIOB->DOUTSET31_0 = (1<<12);
}

void Speaker_Toggle() {
  GPIOB->DOUTTGL31_0 = (1<<12);
}

void Sound_BatHit(void){
  Sound_Start(bat_hit, 2239);
}


void Sound_StumpHit(void){
  Sound_Start(stump, 3592);
}

void Sound_IPLStadium(void){
  Sound_Start(ipl_stadium, 28179);
}

