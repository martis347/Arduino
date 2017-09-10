 #include <QList.h>
#include "QList.cpp"

QList<unsigned long> bingoResetTimes;
bool secondClap = false;
int clapCounter = 0, bingoCounter = 0;
bool bingo;
unsigned long timeToWait;

void setup () 
{
  Serial.begin (9600);
  pinMode(5, OUTPUT);
  bingoResetTimes.push_back(millis());
  bingoResetTimes.push_back(millis());
  bingoResetTimes.push_back(millis());
  bingoResetTimes.push_back(millis());
  bingoResetTimes.push_back(millis());
}

void loop () 
{
  int a = analogRead(A1);
  //Serial.print(a);
  //Serial.print("                       ");
  bool val = digitalRead(10);// digital interface will be assigned a value of pin 3 to read val
  if(val)
  {
    digitalWrite(5, HIGH);
  }
  else 
  {
    digitalWrite(5, LOW);
  }

  if(millis() >= timeToWait)
  {
    if(secondClap && val)
    {
      //Serial.println("Bingo0");
      bingo = true;
      secondClap = false;
      clapCounter = 0;
      bingoCounter = 0;
      val = false;
      timeToWait = millis() + 30;
    }
    else if(val && !secondClap)
    {
      secondClap = true;
      timeToWait = millis() + 40;
    }
    else if(!val && secondClap)
    {
      clapCounter++;
    }
      
    if(clapCounter > 2000)
    {
      secondClap = false;
      clapCounter = 0;
    }
    if(bingo && !val)
    {
      bingoCounter++;
    }
    if(bingo && bingoCounter < 10000 && val)
    {
      bingo = false;
      //Serial.print("Bingo reset ");
      timeToWait = millis() + 200;
      bingoResetTimes.push_back(millis());
      bingoResetTimes.pop_front();
      //Serial.println(bingoCounter);
      bingoCounter = 0;
    }
    else if (bingo && bingoCounter > 10000 && (millis() - bingoResetTimes.front()) < 30000)
    {
      Serial.println("Too many recent bingo resets!");
      bingo = false;
      bingoCounter = 0; 
    }  
    else if (bingo && bingoCounter > 10000)
    {
      Serial.print("                                                 BINGO! ");
      Serial.println(millis() - bingoResetTimes.front());
      timeToWait = millis() + 600;
      bingo = false;
      bingoCounter = 0;
    }  
  }
}

