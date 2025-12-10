#include "display.h"
#include "SpedenSpelit.h"
void initializeDisplay(void)
{
 
//laittaa nimetyt pinnit output muotoon
pinMode(DISP_RESETn, OUTPUT);
pinMode(DISP_SHIFT, OUTPUT);
pinMode(DISP_LATCH, OUTPUT);
pinMode(DISP_ENABLEn, OUTPUT);
pinMode(DISP_DATAIN, OUTPUT);
 
delay(20); //pieni delay jotta pinModet saadaan varmasti OUTPUT tilaan
 
//kaikki nollataan "clean slate" aloitukseen \/
 
digitalWrite(DISP_RESETn, 1); //1 = reset pois päältä, sen takia 1 koska shift rekisterissä 0 = aktiivinen eli aktiivinen matalalla jännitteellä
digitalWrite(DISP_SHIFT, 0); //0 = alhaalla, bitit ei siirry vahinggossa
digitalWrite(DISP_LATCH, 0); //0 = 7 segment näytöt ei päivity ennen pelin alkua
digitalWrite(DISP_ENABLEn, 0); //0 = näyttö kumminkin päällä, sama kun ekassa eli aktiivinen matalalla jännitteellä
digitalWrite(DISP_DATAIN, 0); //0 datalinja matala eli eka bitti on 0
 
//rekisteri nollataan \/
 
digitalWrite(DISP_RESETn, 0); //0 = reset päällä, kaikki piirin rekisterin bitit ja näyttöjen numerot tyhjennetään
digitalWrite(DISP_RESETn, 1); //1 = reset pois päältä,  rekisteri ja näytöt toimii normaalisti tyhjällä muistilla ja ilman edellisiä numeroita
 
//LATCH eli näyttöjen pulssi päivitetään rekisterin tyhjentämisen jälkeen \/
 
digitalWrite(DISP_LATCH, 1); //1 = LATCH auki, rekisterin tieto kopioidaan näytölle eli tyhjä = 0
digitalWrite(DISP_LATCH, 0); //0 = LATCH kiinni, kopiointi loppuu ja voit alkaa laittamaan omaa dataa näytölle
 
}
 
void writeByte(uint8_t bits,bool last) {
 
uint8_t dispBits = 0; //tehdään 8 bittinen muuttuja näytön numeroille käyttäen uint8_t muuttujatyyppiä
 
  switch(bits) {
    case 0:
    dispBits = 0b00111111; //numero 0
    break;
    case 1:
    dispBits = 0b00000110; //numero 1
    break;
    case 2:
    dispBits = 0b01011011; //numero 2
    break;
    case 3:
    dispBits = 0b01001111; //numero 3
    break;
    case 4:
    dispBits = 0b01100110; //numero 4
    break;
    case 5:
    dispBits = 0b01101101; //numero 5
    break;
    case 6:
    dispBits = 0b01111101; //numero 6
    break;
    case 7:
    dispBits = 0b00000111; //numero 7
    break;
    case 8:
    dispBits = 0b01111111; //numero 8
    break;
    case 9:
    dispBits = 0b01101111; //numero 9
    break;
    default:  
    dispBits = 0b01000000; //error, näyttää -
    break;
  }
 
  for(int i=0; i<8; i++) { //siirtää bitit rekisteriin
    digitalWrite(DISP_DATAIN, ((dispBits >> 7-i) & 0b00000001));
    digitalWrite(DISP_SHIFT, 1);
    digitalWrite(DISP_SHIFT, 0);
  }
 
  if(last == true) { // jos kaikki bitit menevät rekisteriin, rekisteri kopioidaan LATCHiin että numero voidaan tulostaa näytölle
    digitalWrite(DISP_LATCH, 1);
    digitalWrite(DISP_LATCH, 0);
  }
}
 
void writeHighAndLowNumber(uint8_t tens,uint8_t ones)
{
// See requirements for this function from display.h
}
 
void showResult(byte number) //funktio joka tulostaa luvun näytölle ja erottelee ne että ne tulostuu oikeassa järjestyksessä
 
{
  uint8_t tens = number / 10; //poimii kymmen luvut, jako jättää ne
  uint8_t ones = number % 10; //jakaa luvut että saadaan yhden luvut
 
  writeByte(tens, false); //siirtää numeron vasempaan, mutta sitä ei vielä näytetä
  writeByte(ones, true);  //siirtää numeron oikeaan näyttöön ja LATCH pulssi päivittää numerot kummallekkin näytölle näkyviin
}