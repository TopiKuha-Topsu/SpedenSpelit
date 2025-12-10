#include "buttons.h"
#include "SpedenSpelit.h"
 
 
//Tämä on taulukko, jossa on 6 aikaleimaa ja jokainen kohta vastaa yhtä nappia
//unsigned long = iso numero, sopii millis()-ajoille
//volatile = arvo voi muuttua keskeytyksessä, joten sitä ei saa optimoida pois
volatile unsigned long lastChangeTime[6];
 
 
//Muistaa napin edellisen tilan HIGH tai LOW
//Jotta tiedetään painettiinko nappia HIGH -> LOW vai vapautettiinko se
volatile byte previousState[6];
 
 
//Kuinka monta painallusta käyttäjä on antanut
//volatile = arvo voi muuttua keskeytyksessä, joten sitä ei saa optimoida pois
volatile byte userPresses = 0;
 
//Mikä nappi painettiin viimeksi.
volatile byte buttonNumber = 0;
 
 
//Kaksi napin tilaa:
//WAIT_FOR_PRESS eli odotetaan, että nappia painetaan
//WAIT_FOR_RELEASE eli odotetaan, että nappi päästetään ylös
//Jokaiselle napille on oma tila taulukossa
enum ButtonState { WAIT_FOR_PRESS, WAIT_FOR_RELEASE };
volatile ButtonState state[6];
 
 
//Debounce-viive eli kuinka kauan napin pitää olla vakaassa tilassa, ennen kuin muutos hyväksytään
const unsigned long debounceDelay = 20;
 
 
//Nappien alustus + Pin Change -keskeytysten päälle laitto
void initButtonsAndButtonInterrupts()
{
  //Käydään jokainen nappipinni läpi
  for (byte pin = firstPin; pin <= lastPin; pin++)
  {
    pinMode(pin, INPUT_PULLUP);   //Nappi on normaalisti HIGH, painettaessa LOW
    previousState[pin] = HIGH;    //Alussa nappi ei ole painettu
    state[pin] = WAIT_FOR_PRESS;  //Odotetaan painallusta
  }
 
  //PCICR = Pin Change Interrupt Control Register
  //Otetaan käyttöön keskeytysryhmä PCIE2 (kattaa pinnit D0–D7)
  PCICR |= (1 << PCIE2);
 
  //PCMSK2 = Pin Change Mask Register
  //Määritetään mitkä yksittäiset pinnit aiheuttavat keskeytyksen.
  //Tässä: pinnit 2, 3, 4 ja 5.
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21);
 
  interrupts();   //Sallitaan keskeytykset
}
 
 
//Tämä funktio ajautuu automaattisesti, kun jokin pinnistä 2–5 vaihtaa tilaansa HIGH -> LOW tai LOW -> HIGH
ISR(PCINT2_vect)
{
  unsigned long now = millis();   //Tallennetaan nykyinen aika (ms)
 
  //Käydään jokainen nappipinni läpi
  for (byte pin = firstPin; pin <= lastPin; pin++)
  {
    byte cur = digitalRead(pin);      //Tämänhetkinen tila: HIGH vai LOW
    byte prev = previousState[pin];   //Mitä se oli viime kerralla
    bool changed = (cur != prev);     //Muuttuiko tila ylipäätään
 
 
 
    //Odottaa että nappia painetaan
    if (state[pin] == WAIT_FOR_PRESS)
    {
      //Painallus = HIGH -> LOW
      if (prev == HIGH && cur == LOW &&
          (now - lastChangeTime[pin] >= debounceDelay)) //Debounce
      {
        //Pin 2 -> nappi 1, pin 3 -> nappi 2 jne.
        buttonNumber = pin - 1;
 
        //Nämä liittyvät pelimoottoriin (SpedenSpelit.cpp)
        if (startGame)
        {
          userNumbers[userPresses] = buttonNumber;  //Tallennetaan käyttäjän painallus
          timeToCheckGameStatus = true;             //Kerrotaan pelille että tarkastus tarvitaan
        }
 
        //Nyt odotetaan napin vapautusta
        state[pin] = WAIT_FOR_RELEASE;
      }
    }
 
 
    //Odottaa, että nappi päästetään ylös
    else
    {
      //Vapautus = LOW -> HIGH
      if (prev == LOW && cur == HIGH &&
          (now - lastChangeTime[pin] >= debounceDelay)) //Debounce
      {
        //Palataan odottamaan seuraavaa painallusta
        state[pin] = WAIT_FOR_PRESS;
      }
    }
 
 
    //Jos napin tila muuttui, päivitetään muutoksen aikaleima
    if (changed)
      lastChangeTime[pin] = now;
 
    //Päivitetään napin edellinen tila seuraavaa keskeytystä varten
    previousState[pin] = cur;
  }
}