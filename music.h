#ifndef MUSIC_H
#define MUSIC_H
#include <arduino.h>
void playMusic(void);
void playLoseMusic(void);
extern int noteIndex;
extern volatile bool tempoChange;
extern float tempo;
extern bool musicFinished;
void playWinMusic(void);

#endif;