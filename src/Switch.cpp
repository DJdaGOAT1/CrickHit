/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
  IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA26INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA25INDEX] = 0x00040081; // input, no pull
  IOMUX->SECCFG.PINCM[PA24INDEX] = 0x00040081; // input, no pull
}
// return current state of switches
uint32_t Switch_In(void){
  uint32_t data = GPIOA->DIN31_0;
  data = (data>>24) & 0xF;
  return data; // return 0; //replace this your code
}
