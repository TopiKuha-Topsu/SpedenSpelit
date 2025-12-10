#include "display.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"
#include "music.h"

 
 #define BUZZER_PIN 13                         // Määrittää pinnin johon summeri on kytketty


volatile bool newTimerInterrupt = false; // tällä katsotaan loopissa onko uusi timer keskeytys tapahtunut
volatile byte randomNumber; // tähän tallennetaan aina satunnainen numero
byte result = 0; // pistemäärä
int randomNumbers[100]; // taulukko, johon satunnaiset numerot tallennetaan
volatile int userNumbers[100]; // taulukko, johon käyttäjän napinpainallukset tallennetaan
volatile byte randomAmount = 0; // tämä kertoo monta kertaa satunnainen numero on luotu
int lastSpeedUp = 0; // tätä käytetään pelinopeuden muuttamiseen
volatile bool timeToCheckGameStatus = false; // tämä kertoo milloin käyttäjä on painanut jotakin nappia
bool startGame = false; // tämä kertoo onko peli käynnissä
bool gameLose = false;
bool loseShowStarted = false;
//ledin sytytykseen liittyvät muuttujat
unsigned long ledStartTime = 0; // tähän tallennetaan aika, jolloin led syttyy
unsigned long ledDuration = 0; // tähän tallennetaan se aikamäärä, joka ledin pitää palaa
bool ledActive = false; // tämä näyttää onko joku led päällä
//bool waitingForNext = true; // tämä kertoo, kun odotetaan seuraavaa lediä

volatile byte randomShow = 0; //käytetään seuraavan ledin näyttämiseen
bool gameWin = false;

void setup()
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  
   interrupts(); // sallitaan keskeytykset
   initButtonsAndButtonInterrupts(); // tämä funktio löytyy buttons.cpp koodista, jolla alustetaan kaikki nappeihin liittyvät jutut
  randomSeed(analogRead(A0)); // otetaan randomseed A0 pinnistä. Tämä tekee randomista oikeasti satunnaisen
  initializeDisplay(); // alustetaan pistenäytöt. Tämä funktio löytyy display.cpp koodista
  initializeLeds();
  showResult(result); // näytetään heti alussa pistemäärä, joka on aluksi 0
}

void loop() 
{

  if(!startGame && gameWin == false && gameLose == false) // jos peli ei ole alkanut ja kierroksen lopetus show ei ole käynnissä, näytetään show1
  {
    show1();
  }


  

  if(buttonNumber >= 1 && startGame == false) // jos peli ei ole käynnissä ja jotain nappia painetaan, aloitetaan peli
  { 
    result = 0; // nollataan pistemäärä
    showResult(result); // näytetään pistemäärä näytöillä. showResult funktio löytyy display.cpp koodista
    startTheGame(); // kutsutaan pelinaloitus funktiota
  }
  if(startGame == true) // jos peli on käynnissä
  {
    playMusic(); // soitetaan taustamusiikkia
    if(timeToCheckGameStatus == true) // jos jotakin nappia on painettu
    {
      timeToCheckGameStatus = false;  // muutetaan tämä bool falseksi, ettei tätä toisteta heti uudelleen
     checkGame(); // tämä funktio tarkistaa oliko nappi oikea
    }
    else
    {
      if(newTimerInterrupt == true) // jos uusi timer1 keskeytys on tapahtunut
      {
        newTimerInterrupt = false;  //muutetaan tämä bool falseksi, ettei tätä toisteta heti uudelleen
         ledDuration = (unsigned long)(OCR1A / 2.0 * 0.064);            //sytytetään random numeron mukainen ledi niin, että-
         clearAllLeds();                                                  //led on päällä puolet siitä ajasta, kun kestää ennen kuin uusi numero tehdään
          setLed(randomNumbers[randomAmount]);                                               // sytytetään seuraava led. setLed funktio löytyy leds.cpp koodista
         randomAmount++;                                                  // kasvatetaan randomnumeroiden luontimäärää yhdellä
          ledStartTime = millis();                                         //tallennetaan nykyinen aika tähän muuttujaan
         ledActive = true;                                                // muutetaan tämä bool trueksi jotta nähdään että led on päällä
         //waitingForNext = false;                                          // näytetään tällä boolilla että ei odoteta uutta lediä
       
     
  
      }                                                                  // jos led on päällä ja nykyisen ajan ja aikaisemmin 
      if (ledActive == true && millis() - ledStartTime >= ledDuration) //tallennetun ajan erotus on suurempi tai yhtäsuuri kuin ledin päälläoloaika
      {
      clearAllLeds();  // sammutetaan kaikki ledit
      ledActive = false; // tallennetaan tähän booliin että led ei ole päällä
      //waitingForNext = true; // tallennetaan tähän booliin että odotetaan seuraavaa lediä
      }
    }  
  }
  if(gameLose)   // Jos peli on päättynyt
 {
  if (!loseShowStarted)  // Jos loppumisshow ei ole alkanut
  {
     noteIndex = 0; // resetataan lopetusshow
    startShow2(); // aloitetaan show2
    loseShowStarted = true; // asetetaan tämä bool näyttämään että lopetusshow on alkanut. Tämä tehdään siksi ettei showta näytetä kuin vain kerran
  }

  updateShow2();  // päivitetään show2
  playLoseMusic(); // soitetaan häviömusiikkiä

 }
 if(gameWin)
{
   if (!loseShowStarted)  // Jos loppumisshow ei ole alkanut
  {
     noteIndex = 0; // resetataan lopetusshow
    startShow2(); // aloitetaan show2
    loseShowStarted = true; // asetetaan tämä bool näyttämään että lopetusshow on alkanut. Tämä tehdään siksi ettei showta näytetä kuin vain kerran
  }

  updateShow2();  // päivitetään show2
playWinMusic();

 }
}
  

void initializeTimer(void) // tässä alustetaa timer
{  
  TCCR1A = 0; // Nollataan Timer1 ohjausrekisteri A
  TCCR1B = 0; // Nollataan Timer1 ohjausrekisteri B
  TCCR1B |= (1 << WGM12); // tämä Laittaa ajastimen CTC tilaan. CTC tilassa laskuri nollataan, kun se saavuttaa OCR1A arvon.
  OCR1A = 15624; // Asetetaan Compare Match. Tämä arvo kertoo, milloin tehdä keskeytys. 15624 tuottaa yhden sekunnin keskeytyksiä kun käytetään prescaleria 1024
  TIFR1 |= (1 << OCF1A);   // Tyhjennetään vanha compare-match -lippu
  TCNT1 = 0; // nollataan laskuri
  TIMSK1 |= (1 << OCIE1A); // tämä ottaa käyttöön Output Compare A Match Interruptin Timer1:lle.
  TCCR1B |= (1 << CS12) | (1 << CS10); // Asettaa prescalerin 1024
}

ISR(TIMER1_COMPA_vect) // timer1 keskeytys
{
  if(randomAmount < 99)
  {
        newTimerInterrupt = true; // näytetään tällä boolilla että uusi timer1 keskeytys on tapahtunut
        if(randomAmount == 10) // jos numeroita on luotu 10
        {
          OCR1A = OCR1A - (OCR1A / 100 *20); // tämä lasku nopeuttaa aikaa 20%
         tempoChange = true;
         lastSpeedUp = randomAmount; // tallennetaan lastSpeedUp muuttujaan että viimeksi nopeutta on muutettu tässä kohtaa
        }
        if(randomAmount - lastSpeedUp >= 10) // jos satunnaisten numeroiden määrän ja viime nopeudenvaihtomäärän erotus on enemmän tai yhtäsuurikuin 10
        {                                    // tällä laskulla voidaan nopeuttaa timer1 keskeytystahtia jokaisen 10 satunnaisen numeron välein
         OCR1A = OCR1A - (OCR1A / 100 *20); // tämä lasku nopeuttaa aikaa 20%
         tempoChange = true;
         lastSpeedUp = randomAmount; // tallennetaan tähän muuttujaan että viime nopeuden vaihto tapahtui tässä satunnaismäärässä
        }
        
  }
 
 
  

}

void checkGame() // tällä tarkistetaan onko painettu nappi oikea
{
  
  if(randomNumbers[userPresses] == userNumbers[userPresses]) // tässä verrataan onko nappien painomäärän kohdalla, sekä randomnumero taulukossa että käyttäjän paino taulukossa sama numero, eli onko nappi ollut oikea
  {
    
    Serial.println("Oikein");
    userPresses++; // kasvatetaan käyttäjän painomäärää yhdellä
    result++; // kasvatetaan pistemäärää yhdellä
    showResult(result); // näytetään uusi pistemäärä näytöillä
   
    timeToCheckGameStatus = false; // asetetaan tämä bool falseksi. Tämä kertoo että ei ole tarvetta uudelle tarkistukselle
   if(result >= 99)  // jos pisteitä on 99, lopetetaan peli voittoon
   {
    stopTheGame();
    gameWon();
    
    
   }
  }
  
  else // tämä tapahtuu jos nappi on ollut väärä
  {
    
    Serial.println("Hävisit");
    stopTheGame(); // lopetetaan peli kutsumalla tätä funktiota
    gameLost();
    
    
  }
}

void initializeGame() // alustetaan peli
{
 clearAllLeds(); // sammutetaan kaikki ledit
  buttonNumber = 0; // nollataan nappinumero
  newTimerInterrupt = false; 
  randomAmount = 0; // nollataan satunnaisten numeroiden määrä
  userPresses = 0; // nollataan käyttäjän painomäärä
  result = 0; // nollataanpistemäärä
  timeToCheckGameStatus = false; 
  noteIndex = 0;
  tempo = 0.7;
  tempoChange = false;
  randomShow = 0;
  gameWin = false;
  for(int i = 0; i < 99; i++)
  {
     randomNumber = random(1, 5); // luodaan uusi satunnainen numero
     randomNumbers[i] = randomNumber; // tallennetaan tämä uusi satunnainen numero randomNumbers taulukkoon
     //Serial.print(randomNumber);
  }
  showResult(result); // nollataan näytöt
}


void startTheGame() // tämä aloittaa pelin
{
  initializeGame(); // kutsutaan pelin alustusta
  initializeTimer(); // kutsutaan timerin alustusta
startGame = true; // asetetaan tämä bool näyttämään että peli on alkanut
}
void stopTheGame() // tämä lopettaa pelin
{
  resetShow1();
 
  musicFinished = false;
  Serial.print("Pisteesi: ");
  Serial.println(result);
  startGame = false; // asetetaan pelin käynnissäolo falseksi
  buttonNumber = 0; // nollataan nappi numero
  TIMSK1 &= ~(1 << OCIE1A);  // lopetetaan timer1 keskeytykset
   TCCR1B = 0;                 // 
  TCNT1 = 0;        
  PCICR &= ~(1 << PCIE2);         
  clearAllLeds();
  
  randomShow = 0; 
   

}

void gameWon()
{
Serial.println("Voitit");
gameWin = true;
}
void gameLost()
{
  Serial.println("Hävisit");
 gameLose = true;
}
