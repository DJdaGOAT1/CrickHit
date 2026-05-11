#include "../inc/ST7735.h"
#include "HindiFont.h"

// Stroke encoding: 3 bytes per stroke
// byte0 bits 7-6: 01=hline, 10=vline, 11=pixel, 00=end
// byte0 bits 5-0: x offset (0-12)
// byte1: y offset (0-15)
// byte2: length (for lines, 0 for pixel/end)
#define _H(x,y,l) (0x40|(x)),(y),(l)
#define _V(x,y,l) (0x80|(x)),(y),(l)
#define _P(x,y)   (0xC0|(x)),(y),0
#define _END       0

// Most consonants share: shirorekha at y=2 full width, left danda x=0
// Character bodies in x=4..11, y=3..12

static const uint8_t g_A[] = { // अ
  _H(0,2,12), _V(0,2,11),
  _V(5,3,8),
  _H(6,3,4), _V(9,3,3),
  _H(6,6,3),
  _V(9,7,3), _H(6,9,4),
  _END};

static const uint8_t g_I[] = { // इ
  _H(0,2,8), _V(0,2,2),
  _V(6,3,2),
  _P(7,5), _P(8,6), _P(9,7),
  _P(8,8), _P(7,9), _P(6,10),
  _P(5,11),
  _END};

static const uint8_t g_U[] = { // उ
  _H(0,2,8), _V(0,2,2),
  _V(5,3,2),
  _H(4,5,4), _V(4,5,4), _V(7,5,4),
  _H(4,8,4),
  _V(5,9,2),
  _END};

static const uint8_t g_E[] = { // ए
  _H(0,2,12),
  _V(10,3,3), _P(9,6), _V(10,7,3),
  _H(0,10,11),
  _END};

static const uint8_t g_KA[] = { // क
  _H(0,2,12), _V(0,2,11),
  _H(5,3,5), _V(9,3,3), _P(10,4),
  _H(5,5,4), _P(8,5),
  _V(5,6,6),
  _END};

static const uint8_t g_KHA[] = { // ख
  _H(0,2,12), _V(0,2,11),
  _P(5,3), _P(9,3),
  _P(6,4), _P(8,4),
  _P(7,5),
  _P(6,6), _P(8,6),
  _P(5,7), _P(9,7),
  _P(4,8), _P(10,8),
  _V(5,9,3),
  _END};

static const uint8_t g_GA[] = { // ग
  _H(0,2,12), _V(0,2,11),
  _P(10,3), _P(9,4), _P(8,5),
  _P(7,6), _P(6,7),
  _V(5,8,4),
  _END};

static const uint8_t g_CHA[] = { // च
  _H(0,2,12), _V(0,2,11),
  _H(5,3,6), _H(5,8,6),
  _V(5,3,6), _V(10,3,6),
  _END};

static const uint8_t g_JA[] = { // ज
  _H(0,2,12), _V(0,2,11),
  _H(5,3,6),
  _P(10,4), _P(9,5), _P(8,6),
  _H(5,6,3),
  _V(5,7,5),
  _END};

static const uint8_t g_TA[] = { // ट
  _P(5,0), _P(5,1),
  _H(0,2,12),
  _P(10,3), _P(9,4), _P(8,5),
  _P(7,6), _P(6,7), _P(5,8),
  _P(4,9),
  _V(3,10,2),
  _END};

static const uint8_t g_DA[] = { // ड
  _P(5,0), _P(5,1),
  _H(0,2,12),
  _P(10,3), _P(9,4), _P(8,5),
  _P(7,6), _P(6,7), _P(5,8),
  _P(4,9),
  _V(3,10,2), _P(4,11),
  _END};

static const uint8_t g_TA2[] = { // त
  _H(0,2,12), _V(0,2,11),
  _V(5,3,2), _P(4,4),
  _H(5,5,6), _V(10,5,4),
  _H(5,8,6),
  _END};

static const uint8_t g_DA2[] = { // द
  _H(0,2,12), _V(0,2,11),
  _H(5,3,6), _H(5,8,6),
  _V(5,3,6), _V(10,3,6),
  _END};

static const uint8_t g_DHA[] = { // ध
  _H(0,2,12), _V(0,2,11),
  _H(5,3,5), _H(5,8,5),
  _V(5,3,6), _V(9,3,6),
  _V(11,2,10),
  _END};

static const uint8_t g_NA[] = { // न
  _H(0,2,12), _V(0,2,11),
  _V(5,3,5), _V(9,3,5),
  _H(5,8,5),
  _END};

static const uint8_t g_BA[] = { // ब
  _H(0,2,12), _V(0,2,11),
  _H(5,3,5), _H(5,7,5),
  _V(5,3,5), _V(9,3,5),
  _V(7,8,3),
  _END};

static const uint8_t g_BHA[] = { // भ
  _H(0,2,12), _V(0,2,11),
  _H(5,3,2), _H(8,3,2),
  _V(5,3,5), _V(9,3,5),
  _H(5,7,5),
  _V(7,8,3),
  _END};

static const uint8_t g_MA[] = { // म
  _H(0,2,12), _V(0,2,11),
  _V(5,3,5), _V(9,3,5),
  _H(5,8,5),
  _P(9,9), _P(10,10), _P(11,11),
  _END};

static const uint8_t g_RA[] = { // र
  _H(0,2,7), _V(0,2,2),
  _V(5,3,9),
  _END};

static const uint8_t g_LA[] = { // ल
  _H(0,2,12), _V(0,2,11),
  _V(5,3,6),
  _P(6,9), _P(7,10), _P(8,11),
  _END};

static const uint8_t g_VA[] = { // व
  _H(0,2,12), _V(0,2,11),
  _H(5,3,5), _H(5,7,5),
  _V(5,3,5), _V(9,3,5),
  _END};

static const uint8_t g_SHA[] = { // ष
  _H(0,2,12), _V(0,2,11),
  _V(5,3,6), _V(9,3,6),
  _H(5,6,5),
  _END};

static const uint8_t g_SA[] = { // स
  _H(0,2,12), _V(0,2,11),
  _H(5,3,6),
  _V(5,3,3), _V(10,6,3),
  _H(5,6,6),
  _V(5,6,3), _V(10,3,3),
  _H(5,9,6),
  _END};

static const uint8_t g_HA[] = { // ह
  _H(0,2,12), _V(0,2,11),
  _V(5,3,3),
  _H(5,6,5), _V(9,6,3),
  _H(5,9,4), _P(6,8),
  _V(5,10,2),
  _END};

static const uint8_t g_HI[] = { // हि (precomposed)
  _H(0,2,12),
  _P(1,3), _P(2,4), _P(1,5), // ि mark on left
  _V(4,3,3),
  _H(4,6,5), _V(8,6,3),
  _H(4,9,4), _P(5,8),
  _V(4,10,2),
  _END};

static const uint8_t g_YA[] = { // य
  _H(0,2,12), _V(0,2,11),
  _H(5,3,5), _H(5,7,5),
  _V(5,3,5), _V(9,3,5),
  _P(9,8), _P(10,9), _P(11,10),
  _END};

static const uint8_t g_UU[] = { // ऊ
  _H(0,2,8), _V(0,2,2),
  _V(5,3,2),
  _H(4,5,4), _V(4,5,4), _V(7,5,4),
  _H(4,8,4),
  _V(5,9,3), _P(6,12),
  _END};

static const uint8_t g_PA[] = { // प
  _H(0,2,12), _V(0,2,11),
  _H(5,3,6),
  _V(5,3,8), _V(10,3,6),
  _END};

static const uint8_t g_SH[] = { // श
  _H(0,2,12),
  _V(1,3,5), _V(5,3,5), _V(9,3,5),
  _H(1,8,9),
  _V(9,8,3),
  _END};

static const uint8_t g_VI[] = { // वि (precomposed)
  _H(0,2,12),
  _P(1,3), _P(2,4), _P(1,5),
  _H(4,3,5), _H(4,7,5),
  _V(4,3,5), _V(8,3,5),
  _END};

static const uint8_t g_THA[] = { // ठ
  _P(5,0), _P(5,1),
  _H(0,2,12),
  _H(7,3,3), _V(9,3,3), _H(7,5,3),
  _P(8,6), _P(7,7), _P(6,8),
  _P(5,9), _P(4,10),
  _V(3,11,2),
  _END};

static const uint8_t* const glyph_table[HINDI_GLYPH_COUNT] = {
  g_A, g_I, g_U, g_E,
  g_KA, g_KHA, g_GA, g_CHA, g_JA,
  g_TA, g_DA, g_TA2, g_DA2, g_DHA,
  g_NA, g_BA, g_BHA, g_MA,
  g_RA, g_LA, g_VA, g_SHA, g_SA,
  g_HA, g_HI, g_YA,
  g_UU, g_PA, g_SH, g_VI, g_THA
};

static void DrawGlyph(int16_t x, int16_t y, uint8_t idx, uint16_t color) {
    if(idx >= HINDI_GLYPH_COUNT) return;
    const uint8_t* s = glyph_table[idx];
    int i = 0;
    while(s[i]) {
        uint8_t type = s[i] & 0xC0;
        uint8_t ox = s[i] & 0x3F;
        uint8_t oy = s[i+1];
        uint8_t len = s[i+2];
        if(type == 0x40)      ST7735_DrawFastHLine(x+ox, y+oy, len, color);
        else if(type == 0x80) ST7735_DrawFastVLine(x+ox, y+oy, len, color);
        else                  ST7735_DrawPixel(x+ox, y+oy, color);
        i += 3;
    }
}

static int16_t MatraWidth(uint8_t m) {
    if(m == M_AA || m == M_II || m == M_O) return 3;
    return 0;
}

int16_t HindiWidth(const uint8_t* str) {
    int16_t w = 0;
    for(int i = 0; str[i]; i++) {
        uint8_t ch = str[i];
        if(ch == ' ')       w += 5;
        else if(ch < 0x80)  w += HINDI_W;
        else                w += MatraWidth(ch);
    }
    return w;
}

int16_t DrawHindi(int16_t x, int16_t y, const uint8_t* str, uint16_t color) {
    int16_t cx = x;
    int16_t prev_cx = x;
    int16_t prev_w = 0;

    for(int i = 0; str[i]; i++) {
        uint8_t ch = str[i];

        if(ch == ' ') {
            cx += 5;
            prev_w = 0;
            continue;
        }

        if(ch < 0x80) {
            prev_cx = cx;
            prev_w = HINDI_W;
            DrawGlyph(cx, y, ch, color);
            cx += HINDI_W;
            continue;
        }

        int16_t mid = prev_cx + (prev_w >> 1);

        switch(ch) {
        case M_AA:
            ST7735_DrawFastVLine(cx, y + 2, 10, color);
            cx += 3;
            break;
        case M_II:
            ST7735_DrawFastVLine(cx, y + 2, 9, color);
            ST7735_DrawPixel(cx - 1, y + 10, color);
            ST7735_DrawPixel(cx - 2, y + 11, color);
            cx += 3;
            break;
        case M_U:
            ST7735_DrawPixel(mid - 1, y + 13, color);
            ST7735_DrawPixel(mid, y + 14, color);
            ST7735_DrawPixel(mid + 1, y + 13, color);
            break;
        case M_E:
            ST7735_DrawPixel(mid + 1, y + 1, color);
            ST7735_DrawPixel(mid, y, color);
            ST7735_DrawPixel(mid + 2, y, color);
            break;
        case M_AI:
            ST7735_DrawPixel(mid - 1, y + 1, color);
            ST7735_DrawPixel(mid - 2, y, color);
            ST7735_DrawPixel(mid + 1, y + 1, color);
            ST7735_DrawPixel(mid, y, color);
            ST7735_DrawPixel(mid + 2, y, color);
            break;
        case M_O:
            ST7735_DrawPixel(mid + 1, y + 1, color);
            ST7735_DrawPixel(mid, y, color);
            ST7735_DrawPixel(mid + 2, y, color);
            ST7735_DrawFastVLine(cx, y + 2, 10, color);
            cx += 3;
            break;
        case M_AN:
            ST7735_DrawPixel(mid, y, color);
            ST7735_DrawPixel(mid + 1, y, color);
            ST7735_DrawPixel(mid, y + 1, color);
            break;
        case M_CH:
            ST7735_DrawPixel(mid - 1, y, color);
            ST7735_DrawPixel(mid, y - 1, color);
            ST7735_DrawPixel(mid + 1, y, color);
            break;
        }
    }
    return cx - x;
}

const uint8_t hn_score[]       = {H_A, M_AN, H_KA, 0};
const uint8_t hn_wide[]        = {H_VA, M_AA, H_I, H_DA, 0};
const uint8_t hn_out[]         = {H_A, M_AA, H_U, H_TA, 0};
const uint8_t hn_paused[]      = {H_RA, M_U, H_KA, M_AA, 0};
const uint8_t hn_continue[]    = {H_JA, M_AA, H_RA, M_II, 0};
const uint8_t hn_home[]        = {H_HA, M_O, H_MA, 0};
const uint8_t hn_howtoplay[]   = {H_KA, M_AI, H_SA, M_E, ' ', H_KHA, M_E, H_LA, M_E, M_AN, 0};
const uint8_t hn_select_lang[] = {H_BHA, M_AA, H_SHA, M_AA, ' ', H_CHA, M_U, H_NA, M_E, M_AN, 0};
const uint8_t hn_play_hindi[]  = {H_KHA, M_E, H_LA, M_E, M_AN, 0};
const uint8_t hn_hindi[]       = {H_HI, M_AN, H_DA2, M_II, 0};
const uint8_t hn_move[]        = {H_HI, H_LA, M_AA, H_NA, M_AA, 0};
const uint8_t hn_left[]        = {H_BA, M_AA, H_YA, M_AA, M_CH, 0};
const uint8_t hn_right[]       = {H_DA2, M_AA, H_YA, M_AA, M_CH, 0};
const uint8_t hn_fast[]        = {H_TA2, M_E, H_JA, 0};
const uint8_t hn_slow[]        = {H_DHA, M_II, H_MA, M_AA, 0};
const uint8_t hn_scoring[]     = {H_A, M_AN, H_KA, 0};
const uint8_t hn_start[]       = {H_KHA, M_E, H_LA, M_E, M_AN, 0};
const uint8_t hn_resume[]      = {H_JA, M_AA, H_RA, M_II, 0};
const uint8_t hn_top[]         = {H_UU, H_PA, H_RA, 0};
const uint8_t hn_bottom[]      = {H_NA, M_II, H_CHA, M_E, 0};
const uint8_t hn_sahi[]        = {H_SA, H_HA, M_II, 0};
const uint8_t hn_maar[]        = {H_MA, M_AA, H_RA, 0};
const uint8_t hn_wicket[]      = {H_VI, H_KA, M_E, H_TA, 0};
const uint8_t hn_reset[]       = {H_RA, M_II, H_SA, M_E, H_TA, 0};
const uint8_t hn_theek[]       = {H_THA, M_II, H_KA, 0};
