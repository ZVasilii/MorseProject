#include <WString.h>
#include <LiquidCrystal.h>

/* This program is a Morse-code decoder

   Zaitsev Vasilii
   Elizaveta Kostenok
   Mary Mirzoian
   MIPT 2021
 */

/* NOMINAL CONSTANTS:
 *  
 * DOT_TIME = 100 (1 unit)
 * DAH_TIME = 300 (3 units)
 * SPACE_BETWEEN_SYMBOLS = 100 (1 unit)
 * SPACE_BETWEEN_LETTERS = 400 (4 units)
 * SPACE_BETWEEN_WORDS = 700   (7 unit)
 */

//Constants
enum dot {DOT = 0, DAH = 1};
const uint8_t  pinLaser = A0;          //Pin with photosensor on Arduino
const uint8_t  waitDelay = 20;         //Spending time on pause/signal
const uint16_t laserLimit = 200;       //Analog difference between high & low signal
const uint16_t dotUnit = 200;          //Duration of one DOT unit
const uint32_t spaceUnit = 600;        //Duration of space between words  
const uint32_t terminateUnit = 1000;   //Duration of pause "end of transmittion"

char      chrMorze[] = 
{
 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
 'U', 'V', 'W', 'X', 'Y', 'Z', 
 '1', '2', '3', '4', '5',  '6',  '7',  '8',  '9',  '0', 
  '.',  ';',  ',',  ':',  '?',  '!',  '-'
};

//Morse codes (1 is DAH, 0 is DIT)
uint8_t   varMorze[] = 
{
  0x1, 0x8, 0xA, 0x4, 0x0, 0x2, 0x6, 0x0, 0x0, 0x7,
  0x5, 0x4, 0x3, 0x2, 0x7, 0x6, 0xD, 0x2, 0x0, 0x1,
  0x1, 0x1, 0x3, 0x9, 0xB, 0xC, 0xF, 0x7, 0x3, 0x1,
  0x0, 0x10, 0x18, 0x1C, 0x1E, 0x1F, 0x0, 0x2A, 0x15,
  0x38, 0x0C, 0x33, 0x21
};

//Morse code lengthes
uint8_t   lenMorze[] = 
{  
  2,   4,   4,   3,   1,   4,   3,   4,   2,   4,
  3,   4,   2,   2,   3,   4,   4,   3,   3,   1,
  3,   4,   3,   4,   4,   4,   5,   5,   5,   5,
  5,    5,    5,    5,    5,    5,   6,    6,  6,
  6,    6,    6,    6
};

//Variables
bool flagSignal = false;      //Signal recieving now
bool flagLetter = false;      //Letter recieved now
bool flagSpace = false;       //Space between words recieved
uint8_t currSymbol = DOT;     //Current symbol
uint8_t currLetter = 0;       //Current letter
uint8_t lenLetter = 0;        //Length of the current letter
uint32_t tmp_terminate = 0;   //variables for counting time between events
uint32_t tmp_separator = 0;
uint32_t tmp_space = 0;


LiquidCrystal lcd ( 12 , 11 , 5 , 4 , 3 , 2 ); 


void setup () 
{
  //LCD setup
  lcd.begin ( 16 , 2 );        // specify the type of display LCD 16X2
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print (F( "Morse code"));
  lcd.setCursor(0,1);
  lcd.print (F( "Decoder"));
  delay(1000);
  lcd.clear();
  lcd.noAutoscroll();

  //Pin & serial port setup
  pinMode(pinLaser, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop () 
{
  //If signal detected:
  if (analogRead(pinLaser) < laserLimit)
  {
    flagSignal = true;
    tmp_separator = millis() + dotUnit;     //To distingush, if it's dot or dah
    while(analogRead(pinLaser) < laserLimit) {delay(waitDelay);};
    if (millis() > tmp_separator)
      currSymbol = DAH;
    else 
      currSymbol = DOT;
      
    currLetter <<= 1;
    currLetter |= currSymbol;
    lenLetter ++;
  } 
  //If pause detected
  else 
  {
    tmp_terminate = millis() + terminateUnit; //To distingush, if it's end of transmittion
    tmp_space = millis() + spaceUnit;         //To distingush, if it's space between words
    tmp_separator = millis() + dotUnit;       //To distingush, if it's space between letters
    while(analogRead(pinLaser) > laserLimit && (millis() < tmp_terminate)) { delay(waitDelay); }
    if (millis() > tmp_terminate)
    {
      flagSpace = true;
      flagLetter = flagSignal;
      flagSignal = false; 

    }
    else if (millis() > tmp_space)
    {
      flagSpace = true;
      flagLetter = flagSignal;
      flagSignal = false;
    }
      
    else if (millis() > tmp_separator)
    {
     flagSpace = false;
     flagLetter = flagSignal;
     flagSignal = false;
    }
  }  

  //If letter recieved
  if (flagLetter)
  {
      static uint8_t lettersPrinted = 0;
      for (uint8_t i = 0; i < 43; i++)
        if (currLetter == varMorze[i] && lenLetter == lenMorze[i])
        {+
          Serial.print(chrMorze[i]);
          lcd.print(chrMorze[i]);
          lettersPrinted++;
        }
      if(flagSpace)
      {
        Serial.print(F(" "));
        lcd.print(F(" "));
        lettersPrinted++;
      }
      if (lettersPrinted > 15)
      {
       lcd.scrollDisplayLeft();
      }
    flagLetter = (lenLetter = (currLetter = (flagSpace = 0)));
  }
}
