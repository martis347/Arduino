#include <QList.h>
#include "QList.cpp"

QList<unsigned long> bingoResetTimes;
bool secondClap = false;
int clapCounter = 0, bingoCounter = 0;
bool bingo;
unsigned long timeToWait;


int number1 = 30; //brightness or RED
char mode = 'w';
bool power = false;
int number2 = 0;
int number3; //rainbowSpeed or BLUE
String newSignal = "", oldSignal = "";

const int sound = 8;
const int r1 = 3; const int r2 = 9;
const int g1 = 5; const int g2 = 10;
const int b1 = 6; const int b2 = 11;

void setup()   
{
  Serial.begin(9600);//initialize Serial communication

  Serial.println("A");
  pinMode(r1, OUTPUT);pinMode(g1, OUTPUT);pinMode(b1, OUTPUT);
  pinMode(r2, OUTPUT);pinMode(g2, OUTPUT);pinMode(b2, OUTPUT);

  
  bingoResetTimes.push_back(millis() - 60000);
  bingoResetTimes.push_back(millis() - 60000);
  bingoResetTimes.push_back(millis() - 60000);
  bingoResetTimes.push_back(millis() - 60000);
  bingoResetTimes.push_back(millis() - 60000);
}

void loop() 
{
  if(Serial.available()) {
    delay(100);
    if((char)Serial.read() == '1')
    {
      power = true;
    }
    else
    {
      power = false;
    }
    Serial.read();
    mode = (char)Serial.read(); Serial.read();
    char a = Serial.read();
    String number;
    int i = 0;
    while(isDigit(a)){
      number += (char)a;
      a = Serial.read();
    }
    number1 = number.toInt();
    a = Serial.read();
    i = 0;
    number = "";
    while(isDigit(a)){
      number += (char)a;
      a = Serial.read();
    }
    number2 = number.toInt();

    a = Serial.read();
    i = 0;
    number = "";
    while(isDigit(a)){
      number += (char)a;
      a = Serial.read();
    }
    number3 = number.toInt();
    while(Serial.available())
      Serial.read();
  }
  SendSignal();
  ProcessLights();
  ProcessSound();
}

void SendSignal()
{
  newSignal = String();;
  if (power)
  {
    newSignal = newSignal + "1 ";
  }
  else
  {
    newSignal = newSignal + "0 ";
  }
  
  newSignal = newSignal + mode + " ";
  newSignal = newSignal + number1 + " ";
  newSignal = newSignal + number2 + " ";
  newSignal = newSignal + number3 + " ";

  if (oldSignal != newSignal)
  {
    Serial.println(newSignal);
    oldSignal = newSignal;
  }
}


void ProcessLights()
{
  if(power)
  {
    if(mode == 'w')
    {
      analogWrite(r1, number1); analogWrite(r2, number1);
      analogWrite(g1, number1); analogWrite(g2, number1);
      analogWrite(b1, number1); analogWrite(b2, number1);
    }
    else if (mode == 'r')
    {
      analogWrite(r1, number1); analogWrite(r2, number1);
      analogWrite(g1, 0); analogWrite(g2, 0);
      analogWrite(b1, 0); analogWrite(b2, 0);
    }
    else if (mode == 'g')
    {
      analogWrite(r1, 0); analogWrite(r2, 0);
      analogWrite(g1, number1); analogWrite(g2, number1);
      analogWrite(b1, 0); analogWrite(b2, 0);
    }
    else if (mode == 'b')
    {
      analogWrite(r1, 0); analogWrite(r2, 0);
      analogWrite(g1, 0); analogWrite(g2, 0);
      analogWrite(b1, number1); analogWrite(b2, number1);
    }
    else if (mode == '~')
    {
      unsigned int r, g, b;
      
      if(number2 == 255)
        number2 = 0;
      HSBToRGB(number2++, 255, number1, &r, &g, &b);
      analogWrite(r1, r); analogWrite(r2, r);
      analogWrite(g1, g); analogWrite(g2, g);
      analogWrite(b1, b); analogWrite(b2, b);
    }
    else if (mode == 'a')
    {
      analogWrite(r1, number1); analogWrite(r2, number1);
      analogWrite(g1, number2); analogWrite(g2, number2);
      analogWrite(b1, number3); analogWrite(b2, number3);
    }
    else if (mode == 'm')
    {
      bool val = digitalRead(sound);
      unsigned int r, g, b;
      if(val)
      {
        if(number2 == 255)
          number2 = 0;
        HSBToRGB(number2++, 255, 255, &r, &g, &b);
        analogWrite(r1, r); analogWrite(r2, r);
        analogWrite(g1, g); analogWrite(g2, g);
        analogWrite(b1, b); analogWrite(b2, b);
      }
      else 
      {
        analogWrite(r1, 0); analogWrite(r2, 0);
        analogWrite(g1, 0); analogWrite(g2, 0);
        analogWrite(b1, 0); analogWrite(b2, 0);
      }
    }
  }
  else
  {
    number1 = 0;
    analogWrite(r1, 0); analogWrite(r2, 0);
    analogWrite(g1, 0); analogWrite(g2, 0);
    analogWrite(b1, 0); analogWrite(b2, 0);
  }

  if (mode == '~')
  {
    delay(number3);
  }
}

void ProcessSound()
{
  bool val = digitalRead(sound);
  if(val)
  {
    Serial.println(val);
  }
  if(millis() >= timeToWait)
  {
    if(secondClap && val)
    {
      Serial.println("Bingo0");
      bingo = true;
      secondClap = false;
      clapCounter = 0;
      bingoCounter = 0;
      val = false;
      timeToWait = millis() + 15;
    }
    else if(val && !secondClap)
    {
      secondClap = true;
      timeToWait = millis() + 20;
    }
    else if(!val && secondClap)
    {
      clapCounter++;
    }
      
    if(clapCounter > 700)
    {
      secondClap = false;
      clapCounter = 0;
      Serial.println("Reset ");
    }
    if(bingo && !val)
    {
      bingoCounter++;
    }
    if(bingo && bingoCounter < 3000 && val)
    {
      bingo = false;
      Serial.print("Bingo reset ");
      timeToWait = millis() + 200;
      bingoResetTimes.push_back(millis());
      bingoResetTimes.pop_front();
      Serial.println(bingoCounter);
      bingoCounter = 0;
    }
    else if (bingo && bingoCounter > 3000 && (millis() - bingoResetTimes.front()) < 30000)
    {
      Serial.println("Too many recent bingo resets!");
      bingo = false;
      bingoCounter = 0; 
    }  
    else if (bingo && bingoCounter > 3000)
    {
      Serial.print("                                                 BINGO! ");
      Serial.println(millis() - bingoResetTimes.front());
      timeToWait = millis() + 600;
      bingo = false;
      bingoCounter = 0;
      if(power)
      {
        power = false;
      }
      else if (!power)
      {
        power = true;
        number1 = 60;
        mode = 'w';
      }
    }  
  }
}

void HSBToRGB(
    unsigned int inHue, unsigned int inSaturation, unsigned int inBrightness,
    unsigned int *oR, unsigned int *oG, unsigned int *oB )
{
    if (inSaturation == 0)
    {
        // achromatic (grey)
        *oR = *oG = *oB = inBrightness;
    }
    else
    {
        unsigned int scaledHue = (inHue * 6);
        unsigned int sector = scaledHue >> 8; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 8);  // position within the sector         
        unsigned int p = (inBrightness * ( 255 - inSaturation )) >> 8;
        unsigned int q = (inBrightness * ( 255 - ((inSaturation * offsetInSector) >> 8) )) >> 8;
        unsigned int t = (inBrightness * ( 255 - ((inSaturation * ( 255 - offsetInSector )) >> 8) )) >> 8;

        switch( sector ) {
        case 0:
            *oR = inBrightness;
            *oG = t;
            *oB = p;
            break;
        case 1:
            *oR = q;
            *oG = inBrightness;
            *oB = p;
            break;
        case 2:
            *oR = p;
            *oG = inBrightness;
            *oB = t;
            break;
        case 3:
            *oR = p;
            *oG = q;
            *oB = inBrightness;
            break;
        case 4:
            *oR = t;
            *oG = p;
            *oB = inBrightness;
            break;
        default:    // case 5:
            *oR = inBrightness;
            *oG = p;
            *oB = q;
            break;
        }
    }
}
