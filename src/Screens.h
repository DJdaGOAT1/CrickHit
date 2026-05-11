// Screens.h
// Home screen, instruction screen, and language selection for CricHit.
#ifndef SCREENS_H
#define SCREENS_H

// Language selection (set by HomeScreen, read by InstructionScreen and main)
typedef enum { English, Spanish, Portuguese, French } Language_t;
extern Language_t myLanguage;

// Language phrase table (3 phrases x 4 languages); used by main1 test
extern const char *Phrases[3][4];

// Stadium home screen with language buttons.
// Plays IPL stadium audio immediately on entry.
// Blocks until Top switch (English) or Bottom switch (Spanish) is pressed.
void HomeScreen(void);

// "How to Play" screen in the selected language.
// Blocks until any switch is pressed then released.
void InstructionScreen(void);

typedef enum { OUT_CONTINUE, OUT_HOME } OutChoice_t;

void DrawPausePopup(void);
OutChoice_t OutScreen(void);

#endif // SCREENS_H
