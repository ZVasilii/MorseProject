const uint32_t DOTTIME = 100;
const uint8_t SIGNALPIN = 4;

void LexFlash(const char* letter);
void Flash(int n);

//Morse codes (0 is DIT, 1 is DAH)
const char* letters[] =
{
  "01", "1000", "1010", "100", "0", "0010", "110", "0000", "00", //A to I
  "0111", "101", "0100", "11", "10", "111", "0110", "1101", "010", //J to R
  "000", "1", "001", "0001", "011", "1001", "1011", "1100"         //S to Z
};

const char* numbers[] =
{
  "11111", "01111", "00111", "00011", "00001", //0 to 4
  "00000", "10000", "11000", "11100", "11110"  //5 to 9
};

const char* spec_symb[] = 
{
  "000000", "101010", "010101", "111000",     //". ; , :"
  "001100", "110011", "100001"                //"? ! -"
};

void setup()
{
  pinMode(SIGNALPIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  static char lex = 0;  //1 letter from Serial
  if (Serial.available() > 0)
    lex = Serial.read();
    
  if ((lex >= 'a') && (lex <= 'z'))
    LexFlash(letters[lex - 'a']);
  if ((lex >= 'A') && (lex <= 'Z'))
    LexFlash(letters[lex - 'A']);
  if ((lex >= '0') && (lex <= '9'))
    LexFlash(numbers[lex - '0']);
    
  switch (lex)
  {
    case '.' : { LexFlash(spec_symb[0]); break; }
    case ';' : { LexFlash(spec_symb[1]); break; }
    case ',' : { LexFlash(spec_symb[2]); break; }
    case ':' : { LexFlash(spec_symb[3]); break; }
    case '?' : { LexFlash(spec_symb[4]); break; }
    case '!' : { LexFlash(spec_symb[5]); break; }
    case '-' : { LexFlash(spec_symb[6]); break; }
  }
  
  if (lex == ' ')
    delay(DOTTIME * 7);
}

void LexFlash(const char* letter)
{
  int i = 0;
  while (letter[i] != '\0')
  {
    if (letter[i] == '0') 
      Flash(1);
    if (letter[i] == '1') 
      Flash(3);
    delay(DOTTIME);   //Space between dits & dahs
    i++;
  }
  delay(DOTTIME * 3); //Space between letters
}

void Flash(int n)
{
  digitalWrite(SIGNALPIN, HIGH);
  delay(DOTTIME * n);
  digitalWrite(SIGNALPIN, LOW);
  
}
