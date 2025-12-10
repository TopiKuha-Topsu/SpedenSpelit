#include "leds.h"
#include "SpedenSpelit.h"

void initializeLeds()
{
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
}

void setLed(byte ledNumber)
{
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
  int ledPin;
switch(ledNumber)
{
  case 1:
    ledPin = A2;
    break;
  case 2:
    ledPin = A3;
    break;
  case 3:
    ledPin = A4;
    break;
  case 4:
    ledPin = A5;
    break;
}
  digitalWrite(ledPin,HIGH);
}


void clearAllLeds()
{
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
 
}

void setAllLeds()
{
  digitalWrite(A2,HIGH);
  digitalWrite(A3,HIGH);
  digitalWrite(A4,HIGH);
  digitalWrite(A5,HIGH);
}


static unsigned long show1_previousMillis = 0;
static int show1_num = 0;


void resetShow1()
{
    show1_num = 0;
    show1_previousMillis = millis();
}

// --- Varsinainen show1()
void show1()
{
  const int ledPins[4] = {A2, A3, A4, A5};

  unsigned long currentMillis = millis();

  // 0–15
  if (currentMillis - show1_previousMillis >= 800)
  {
    show1_previousMillis = currentMillis;

    for (int i = 0; i < 4; i++)
    {
      int bit = (show1_num >> (3 - i)) & 1;
      if (bit == 1) 
      {
      digitalWrite(ledPins[i], HIGH);
      }     
      else 
      {
      digitalWrite(ledPins[i], LOW);
      }
    }
    show1_num++;
  }

}
int showStep = 0;          // tähän merkataan shown vaihe
int ledIndex = 0;
unsigned long showPrev = 0;
int nopeus = 80;
int showCycles = 0;

void startShow2() // aloitetaan show2
{
  showStep = 0; // resetataan kaikki muuttujat
  ledIndex = 0;
  nopeus = 80;
    showCycles = 0; 
}

void updateShow2()
{
  const int ledPins[4] = {A2, A3, A4, A5};
  unsigned long now = millis();
if (now - showPrev > nopeus) 
{
  showPrev = now;

  
  if (showStep == 0)
  {
    nopeus = nopeus * 0.9;   // kasvatetaan nopeutta pienentämällä aikaväliä jolloin ledit syttyy
    showStep = 1;  // tämä siirtyy ledien sytytys vaiheeseen
    ledIndex = 0;
  }


  else if (showStep == 1) // ledien sytytysvaihe
  {
    if (ledIndex < 4)  // sytytetään kaikki 4 lediä peräkkäin
    {
      digitalWrite(ledPins[ledIndex], HIGH);
      ledIndex++;
    }
    else                     // kun ledejä on sytytetty 4
    {
      showStep = 2;         // siirrytään ledien sammutusvaiheeseen
      ledIndex = 0;         // nollataan ledIndex, jotta ledien sammutus aloittaa samasta ledistä kun sytytys alkoi
    }
  }

  else if (showStep == 2) // ledien sammutusvaihe
{
  if (ledIndex < 4)      // sammutetaan kaikki 4 lediä peräkkäin
  {
    digitalWrite(ledPins[ledIndex], LOW);
    ledIndex++;
  }
  else
  {
    showCycles++;  // kasvatetaan kierrosmäärää

    if (showCycles >= 4)  // jos kierroksia on mennyt 4, lopetetaan show.
    {
      gameLose = false; // tämän avulla aloitetaan show1, kun tämä show päättyy ja tämän ollessa false loop lopettaa tämän shown näyttämisen
      gameWin = false;  // tämän avulla aloitetaan show1, kun tämä show päättyy ja tämän ollessa false loop lopettaa tämän shown näyttämisen
      loseShowStarted = false;  // loopissa kutsutaan show2 jos tämä on false silloin kun peli on päättynyt. Laitetaan tämä false, jotta show alkaa aina joka pelin lopun jälkeen uudelleen
      PCICR |= (1 << PCIE2);  // sallitaan nappikeskeytys, koska nappikesekeytykset on otettu pois show2 ajaksi

    }

    
    showStep = 0; // aloitetaan uusi kierros
  }
}
}
}