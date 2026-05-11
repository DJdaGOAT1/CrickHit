// Screens.cpp

#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "Switch.h"
#include "Sound.h"
#include "Screens.h"

extern volatile bool flag;

// Language data
Language_t myLanguage = English;

typedef enum { HELLO, GOODBYE, LANGUAGE } phrase_t;

const char Hello_English[]       = "Hello";
const char Hello_Spanish[]       = "\xADHola!";
const char Hello_Portuguese[]    = "Ol\xA0";
const char Hello_French[]        = "All\x83";
const char Goodbye_English[]     = "Goodbye";
const char Goodbye_Spanish[]     = "Adi\xA2s";
const char Goodbye_Portuguese[]  = "Tchau";
const char Goodbye_French[]      = "Au revoir";
const char Language_English[]    = "English";
const char Language_Spanish[]    = "Espa\xA4ol";
const char Language_Portuguese[] = "Portugu\x88s";
const char Language_French[]     = "Fran\x87" "ais";

const char *Phrases[3][4] = {
  { Hello_English,    Hello_Spanish,    Hello_Portuguese,    Hello_French    },
  { Goodbye_English,  Goodbye_Spanish,  Goodbye_Portuguese,  Goodbye_French  },
  { Language_English, Language_Spanish, Language_Portuguese, Language_French }
};

// HomeScreen 
void HomeScreen(void) {
  Sound_IPLStadium();   // play crowd ambience as the title screen appears

  uint16_t sky      = ST7735_Color565(12, 18, 68);
  uint16_t stands   = ST7735_Color565(35, 35, 82);
  uint16_t field    = ST7735_Color565(10, 60, 15);
  uint16_t pole_col = ST7735_Color565(80, 80, 80);
  uint16_t glow     = ST7735_Color565(255, 255, 140);
  uint16_t dim      = ST7735_Color565(140, 140, 180);

  // Stadium layers
  ST7735_FillScreen(sky);
  ST7735_FillRect(0, 62, 128,  8, stands);
  ST7735_FillRect(0, 70, 128, 90, field);

  // Left floodlight
  ST7735_DrawFastVLine(9, 0, 40, pole_col);
  ST7735_FillRect(3, 0, 14, 3, pole_col);
  for(int x = 3; x < 17; x += 2) ST7735_DrawPixel(x, 0, glow);

  // Right floodlight
  ST7735_DrawFastVLine(118, 0, 40, pole_col);
  ST7735_FillRect(111, 0, 14, 3, pole_col);
  for(int x = 111; x < 125; x += 2) ST7735_DrawPixel(x, 0, glow);

  // Title "CRICKHIT" size=2: 8 chars x 12px = 96px, centred in 128px -> start_x=16
  const char *ttl = "CRICKHIT";
  uint16_t t_shadow = ST7735_Color565(120, 55, 0);
  uint16_t t_main   = ST7735_Color565(255, 210, 30);
  for(int i = 0; ttl[i]; i++)
    ST7735_DrawChar(17 + i*12, 13, ttl[i], t_shadow, sky, 2);
  for(int i = 0; ttl[i]; i++)
    ST7735_DrawChar(16 + i*12, 12, ttl[i], t_main, sky, 2);

  ST7735_DrawString(3, 4, (char*)"Select Language", dim);

  // English button (y=73..102)
  uint16_t e_shadow = ST7735_Color565(0,  30, 100);
  uint16_t e_fill = ST7735_Color565(18, 90, 200);
  uint16_t e_hi = ST7735_Color565(80, 155, 255);
  uint16_t e_edge = ST7735_Color565(10,  55, 130);
  ST7735_FillRect(9, 76, 114, 30, e_shadow);
  ST7735_FillRect(7, 73, 114, 30, e_fill);
  ST7735_DrawFastHLine(7, 73, 114, e_hi);
  ST7735_DrawFastVLine(7, 73,  30, e_hi);
  ST7735_DrawFastHLine(7, 102, 114, e_edge);
  ST7735_DrawFastVLine(120, 73,  30, e_edge);
  { const char *b = "PLAY (ENGLISH)"; int n = 14, px = 7 + (114 - n*6)/2;
    for(int i = 0; b[i]; i++) ST7735_DrawChar(px+i*6, 84, b[i], ST7735_WHITE, e_fill, 1); }

  // Spanish button (y=110..139)
  uint16_t s_shadow = ST7735_Color565(90,  65,   0);
  uint16_t s_fill = ST7735_Color565(195, 155,  0);
  uint16_t s_hi = ST7735_Color565(255, 225, 50);
  uint16_t s_edge = ST7735_Color565(120, 100,  0);
  ST7735_FillRect(9,  113, 114, 30, s_shadow);
  ST7735_FillRect(7,  110, 114, 30, s_fill);
  ST7735_DrawFastHLine(7,  110, 114, s_hi);
  ST7735_DrawFastVLine(7,  110,  30, s_hi);
  ST7735_DrawFastHLine(7,  139, 114, s_edge);
  ST7735_DrawFastVLine(120, 110,  30, s_edge);
  { const char *b = "JUGAR (Espa\xA4ol)"; int n = 15, px = 7 + (114 - n*6)/2;
    for(int i = 0; b[i]; i++) ST7735_DrawChar(px+i*6, 121, b[i], ST7735_BLACK, s_fill, 1); }

  ST7735_DrawString(1, 15, (char*)"Top=English Bot=Esp", dim);

  // Wait for rising-edge: Top (PA24,bit0)=English, Bottom (PA26,bit2)=Spanish
  uint32_t prev = Switch_In();
  while(1){
    Clock_Delay1ms(10);
    uint32_t in = Switch_In();
    if((in & 1) && !(prev & 1)){ myLanguage = English; break; }
    if((in & 4) && !(prev & 4)){ myLanguage = Spanish; break; }
    prev = in;
  }
  while(Switch_In() & 0xF) Clock_Delay1ms(10);
}

// InstructionScreen 
void InstructionScreen(void){
  bool es = (myLanguage == Spanish);
  uint16_t sky = ST7735_Color565(12, 18, 68);
  uint16_t hdr_bg = ST7735_Color565(20, 20, 90);
  uint16_t lbl = ST7735_CYAN;
  uint16_t desc = ST7735_WHITE;
  uint16_t grn = ST7735_Color565(80, 220, 80);
  uint16_t red = ST7735_Color565(255, 80, 80);
  uint16_t orng = ST7735_Color565(255, 165, 0);
  uint16_t dim = ST7735_Color565(140, 140, 180);

  ST7735_FillScreen(sky);
  ST7735_FillRect(0, 0, 128, 18, hdr_bg);
  ST7735_DrawString(5, 0, es ? (char*)"COMO JUGAR" : (char*)"HOW TO PLAY", ST7735_YELLOW);
  ST7735_DrawFastHLine(0, 18, 128, ST7735_WHITE);

  ST7735_DrawString(0, 2, (char*)"POT :", lbl);  ST7735_DrawString(5, 2, es ? (char*)"Mover bateador" : (char*)"Move batsman", desc);
  ST7735_DrawString(0, 3, (char*)"L SW:", lbl);  ST7735_DrawString(5, 3, es ? (char*)"Batear izq"     : (char*)"Swing left",  desc);
  ST7735_DrawString(0, 4, (char*)"R SW:", lbl);  ST7735_DrawString(5, 4, es ? (char*)"Batear der"     : (char*)"Swing right", desc);
  ST7735_DrawString(0, 5, (char*)"TOP :", lbl);  ST7735_DrawString(5, 5, es ? (char*)"Lanzar r\xA0pido"  : (char*)"Bowl faster", desc);
  ST7735_DrawString(0, 6, (char*)"BOT :", lbl);  ST7735_DrawString(5, 6, es ? (char*)"Lanzar lento"   : (char*)"Bowl slower", desc);

  ST7735_DrawFastHLine(0, 69, 128, ST7735_WHITE);
  ST7735_DrawString(1, 7, es ? (char*)"PUNTAJE:" : (char*)"SCORING:", ST7735_YELLOW);
  ST7735_DrawString(0, 8, es ? (char*)" Perfecto+6 Ancha+1"  : (char*)" Perfect +6  Wide +1", grn);
  ST7735_DrawString(0, 9, es ? (char*)" Bien +4  Golpe +2"   : (char*)" Good +4  Hit +2",     grn);
  ST7735_DrawString(0,10, es ? (char*)"Wicket: \xAD" "Fuera!" : (char*)" Wicket: you're out!", red);

  ST7735_DrawFastHLine(0, 111, 128, ST7735_WHITE);
  ST7735_DrawString(0,12, es ? (char*)"TOP+BOT = Men\xA3 Inicio" : (char*)"TOP+BOT = Home Screen", orng);
  ST7735_DrawString(0,13, es ? (char*)"L+R     = Pausar"      : (char*)"L+R     = Pause Game",  orng);

  ST7735_DrawFastHLine(0, 141, 128, ST7735_WHITE);
  ST7735_DrawString(0,15, es ? (char*)">> Presiona inicio <<" : (char*)">> Press to start <<", dim);

  uint32_t prev = Switch_In();
  while(1){
    Clock_Delay1ms(10);
    uint32_t in = Switch_In();
    if(in & ~prev & 0xF) break;
    prev = in;
  }
  while(Switch_In() & 0xF) Clock_Delay1ms(10);
}

void DrawPausePopup(void){
  bool es = (myLanguage == Spanish);
  ST7735_FillRect(18, 58, 92, 44, ST7735_BLACK);
  ST7735_DrawFastHLine(18,  58, 92, ST7735_WHITE);
  ST7735_DrawFastHLine(18, 101, 92, ST7735_WHITE);
  ST7735_DrawFastVLine(18,  58, 44, ST7735_WHITE);
  ST7735_DrawFastVLine(109, 58, 44, ST7735_WHITE);
  ST7735_DrawString(5, 7, es ? (char*)"PAUSADO" : (char*)"PAUSED", ST7735_YELLOW);
  ST7735_DrawString(2, 8, es ? (char*)"L+R: Reanudar" : (char*)"L+R: Resume", ST7735_WHITE);
  ST7735_DrawString(2, 9, es ? (char*)"T+B: Inicio"   : (char*)"T+B: Home",   ST7735_WHITE);
}

OutChoice_t OutScreen(void){
  bool es = (myLanguage == Spanish);
  uint16_t red = ST7735_Color565(255, 50, 50);
  uint16_t grey = ST7735_Color565(100, 100, 100);

  ST7735_FillRect(10, 30, 108, 105, ST7735_BLACK);
  ST7735_DrawFastHLine(10,  30, 108, ST7735_WHITE);
  ST7735_DrawFastHLine(10, 134, 108, ST7735_WHITE);
  ST7735_DrawFastVLine(10,  30, 105, ST7735_WHITE);
  ST7735_DrawFastVLine(117, 30, 105, ST7735_WHITE);
  ST7735_DrawFastHLine(12,  32, 104, red);
  ST7735_DrawFastHLine(12, 132, 104, red);
  ST7735_DrawFastVLine(12,  32, 101, red);
  ST7735_DrawFastVLine(115, 32, 101, red);

  ST7735_DrawString(es ? 7 : 9, 5, es ? (char*)"\xAD" "FUERA!" : (char*)"OUT!", red);
  ST7735_DrawFastHLine(20, 60, 88, grey);

  if(es){
    ST7735_DrawString(3, 8,  (char*)"Arr: Seguir",  ST7735_YELLOW);
    ST7735_DrawString(3, 11, (char*)"Abj: Inicio",  ST7735_WHITE);
  } else {
    ST7735_DrawString(3, 8,  (char*)"Top: Continue", ST7735_YELLOW);
    ST7735_DrawString(3, 11, (char*)"Bot: Home",     ST7735_WHITE);
  }

  uint32_t prev = Switch_In();
  while(1){
    while(flag == 0);
    flag = false;
    uint32_t in = Switch_In();
    if((in & 1) && !(prev & 1)) return OUT_CONTINUE;
    if((in & 4) && !(prev & 4)) return OUT_HOME;
    prev = in;
  }
}
