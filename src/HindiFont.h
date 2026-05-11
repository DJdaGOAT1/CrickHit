#ifndef HINDI_FONT_H
#define HINDI_FONT_H

#include <stdint.h>

#define HINDI_W 12
#define HINDI_H 16

#define H_A    0
#define H_I    1
#define H_U    2
#define H_E    3
#define H_KA   4
#define H_KHA  5
#define H_GA   6
#define H_CHA  7
#define H_JA   8
#define H_TA   9
#define H_DA   10
#define H_TA2  11
#define H_DA2  12
#define H_DHA  13
#define H_NA   14
#define H_BA   15
#define H_BHA  16
#define H_MA   17
#define H_RA   18
#define H_LA   19
#define H_VA   20
#define H_SHA  21
#define H_SA   22
#define H_HA   23
#define H_HI   24
#define H_YA   25
#define H_UU   26
#define H_PA   27
#define H_SH   28
#define H_VI   29
#define H_THA  30
#define HINDI_GLYPH_COUNT 31

#define M_AA   0x80
#define M_II   0x81
#define M_U    0x82
#define M_E    0x83
#define M_AI   0x84
#define M_O    0x85
#define M_AN   0x86
#define M_CH   0x87

extern const uint8_t hn_score[];
extern const uint8_t hn_wide[];
extern const uint8_t hn_out[];
extern const uint8_t hn_paused[];
extern const uint8_t hn_continue[];
extern const uint8_t hn_home[];
extern const uint8_t hn_howtoplay[];
extern const uint8_t hn_select_lang[];
extern const uint8_t hn_play_hindi[];
extern const uint8_t hn_hindi[];
extern const uint8_t hn_move[];
extern const uint8_t hn_left[];
extern const uint8_t hn_right[];
extern const uint8_t hn_fast[];
extern const uint8_t hn_slow[];
extern const uint8_t hn_scoring[];
extern const uint8_t hn_start[];
extern const uint8_t hn_resume[];
extern const uint8_t hn_top[];
extern const uint8_t hn_bottom[];
extern const uint8_t hn_sahi[];
extern const uint8_t hn_maar[];
extern const uint8_t hn_wicket[];
extern const uint8_t hn_reset[];
extern const uint8_t hn_theek[];

int16_t DrawHindi(int16_t x, int16_t y, const uint8_t* str, uint16_t color);
int16_t HindiWidth(const uint8_t* str);

#endif
