#include "music.h"
#include "SpedenSpelit.h"
#include "pitches.h"
 
#define BUZZER_PIN 13                         // Määrittää pinnin johon summeri on kytketty
 
 
int WinMelody[] =
{
    NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6,
    NOTE_G5, NOTE_C6, NOTE_E6,
    NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6,
    NOTE_G5, NOTE_C6, NOTE_E6
};
 
int winNoteDurations[] =
{
  8, 8, 8, 4,
  8, 4, 2,
  8, 8, 8, 2,
  8, 4, 2
};
 
 
 
int loseMelody[] = {
    NOTE_E5, NOTE_F5, NOTE_DS5, NOTE_FS5, NOTE_D5,
    NOTE_F5, NOTE_CS5, NOTE_E5, NOTE_C5, NOTE_DS5,
    NOTE_B4, NOTE_C5, NOTE_A4
  };
 
  int loseNoteDurations[] = {
    16,16,16,16,16,
    16,16,16,16,16,
    16,16,8
  };
 
  int melody[] =
{                                             // Taulukko joka sisältää nuotit jotka soitetaan
  NOTE_E5, NOTE_E5, NOTE_F5, NOTE_G5,
  NOTE_G5, NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_E5, NOTE_D5, NOTE_D5
};
 
int noteDurations[] =                         // Taulukko kertoo kuinka pitkään nuotti soi (8 = kahdeksasosanuottia)                
{
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8
};
 
unsigned long previousMillis = 0;             // Tallentaa ajan, jolloin viime nuotti aloitettiin.
int noteIndex = 0;                            // Kertoo mikä nuotti melody taulukosta soitetaan seuraavaksi
bool isPlaying = false;                       // Kertoo soiko nuotti sillä hetkellä
 
float tempo = 1.0;                            // Tempo määrittelee nykyisen nopeuden
volatile bool tempoChange = false;            // Tempo pienenee 2% jokaisen nuotin jälkeen
float minimiTempo = 0.15;                     // Tempon alaraja. Ei nopeudu tätä enempää
 
bool musicFinished = false;
 
void playMusic()
{
  unsigned long currentMillis = millis();                             // currentMillis tallentaa nykyisen ajan millisekunteina
 
  if (isPlaying == false)
  {
    int duration = (6000 / noteDurations[noteIndex]) * tempo;         // Laskee nuotin keston millisekunteina
    tone(BUZZER_PIN, melody[noteIndex], duration);                    // Soittaa nuotin tone() funktiolla
    previousMillis = currentMillis;                                   // Tallentaa milloin nuotti aloitettiin
    isPlaying = true;                                                 // Kertoo onko nuotti käynnissä sillä hetkellä
  }
 
  int duration = (6000 / noteDurations[noteIndex]) * tempo;           // Laskee nuotin keston uudestaan ja vertailee kulunutta aikaa
  if (currentMillis - previousMillis >= duration * 1.30)
  {
    noTone(BUZZER_PIN);                                               // Sammuttaa summerin
    noteIndex++;                                                      // Siirtyy seuraavaan nuottiin
    isPlaying = false;
 
 if(tempoChange)
 {
  tempoChange = false;
  tempo = tempo * 0.8;
 }
    if (noteIndex >= sizeof(noteDurations) / 2)
    {
      noteIndex = 0;
    }
  }
}
 
 
void playLoseMusic()
{
  if (musicFinished) // jos musiikki on jo soitettu kerran
  {
    return; 
  }
  unsigned long currentMillis = millis(); // otetaan nykyinen aika tähän muuttujaan
 
  if (!isPlaying) // jos mikään nuotti ei soi
  {
    int duration = (6000 / noteDurations[noteIndex]) * 0.5;  // lasketaan nuotin pituus
    tone(BUZZER_PIN, loseMelody[noteIndex], duration);  // soitetaan nuotti
    previousMillis = currentMillis; // merkataan tähän että nuotti alkoi soimaan tässä ajassa
    isPlaying = true; // merkataan että jotain nuottia soitetaan
  }
 
  int duration = (6000 / loseNoteDurations[noteIndex]) * 0.5; 
 
  if (currentMillis - previousMillis >= duration * 1.30) // jos aikaa on kulunut tarpeksi
  {
    noTone(BUZZER_PIN); // lopetetaan nuotin soitto
    noteIndex++; // siirrytään seuraavaan nuottiin
    isPlaying = false; // merkataan että nuottia ei soiteta
 
   
 
    // jos kaikki nuotit on soitettu
    if (noteIndex >= (sizeof(loseNoteDurations) / sizeof(loseNoteDurations[0])))
    {
      musicFinished = true;  // merkataan valmiiksi
      noTone(BUZZER_PIN);    // sammutetaan summeri
    }
  }
}
 
 
void playWinMusic()
{
  unsigned long currentMillis = millis();
 
  if (isPlaying == false)
  {
    int duration = (1000 / noteDurations[noteIndex]);
    tone(BUZZER_PIN, WinMelody[noteIndex], duration);
    previousMillis = currentMillis;
    isPlaying = true;
  }
 
    int duration = (1000 / winNoteDurations[noteIndex]);
    if (currentMillis - previousMillis >= duration * 1.3)
    {
      noTone(BUZZER_PIN);
      noteIndex++;
      isPlaying = false;
     
 
 
      if (noteIndex >= (sizeof(winNoteDurations) / sizeof(winNoteDurations[0])))
      {
        musicFinished = true;
        noTone(BUZZER_PIN);
        gameWin = false;
      }
      
    }
}