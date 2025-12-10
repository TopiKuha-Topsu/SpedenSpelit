#ifndef BUTTONS_H  //Estää sen, että tätä tiedostoa ei sisällytetä ohjelmaan kahdesti
#define BUTTONS_H
 
#include <arduino.h>  //Tämä tarvitaan Arduino-funktioita varten, kuten esim. pinMode tai millis
#include <avr/io.h>  //AVR-rekistereitä varten, kuten PCICR tai PCMSK2
#include <avr/interrupt.h>  //Keskeytysten käsittelyyn esim. ISR
 
//Ilmoitus muuttujasta, joka kertoo milloin viime keskeytys tapahtui (debouncea varten)
//Extern eli muuttuja määritellään jossain muualla eikä tässä
//Volatile meinaa tässä sitä, että keskeytys voi muuttaa arvoa, joten kääntäjä ei saa optimoida sitä pois
extern volatile unsigned long lastInterruptTime;
 
 
//Ensimmäinen ja viimeinen nappipinni, joita käytetään keskeytyksen kanssa
const byte firstPin = 2;  //Eka nappi
const byte lastPin  = 5;  //Vika nappi
 
 
//extern = muuttuja on olemassa jossain muualla
//volatile = arvoa voi muuttaa keskeytys taustalla, joten Arduino ei saa optimoida sitä pois
 
extern volatile byte buttonNumber;  //Mikä nappi painettiin
extern volatile byte userPresses;   //Kuinka monta nappia käyttäjä on painanut
 
 
//Funktio, joka alustaa napit ja ottaa pin-change-keskeytykset käyttöön
void initButtonsAndButtonInterrupts(void);
 
 
//ISR = Interrupt Service Routine
//Tämä funktio käynnistyy ei loop -funktiosta, vaan automaattisesti aina kun jokin valituista napeista vaihtaa tilaansa HIGH:sta LOW:n
ISR(PCINT2_vect);
 
#endif
 