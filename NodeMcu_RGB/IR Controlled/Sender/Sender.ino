#include "IRremote.h"//include IRremote library to decode the IR signals 


int IRreceiver = 2; //pin connected to IR receiver
IRrecv irrecv(IRreceiver);//create instance of 'IRrecv'
decode_results results;//create instance of 'decode_results'

decode_results prevResult;
int number1 = 30; //brightness or RED
String mode = "w";
bool power = false;
int number2 = 0; //GREEN
int number3 = 0; //rainbowSpeed or BLUE
String newSignal = "";

void setup()
{
  Serial.begin(9600);//initialize Serial communication
  irrecv.enableIRIn(); //start the IR receiver

  Serial.println("Starting...");
}

void loop()
{
  IRsignal();
}
void SendSignal()
{
  newSignal = String();
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

  Serial.println(newSignal);
}

void IRsignal() //takes action based on IR code received
{
  if (results.value != 0xFFFFFFFF)
    prevResult = results;
  if (irrecv.decode(&results)) //check if it is received an IR signal
  {
    
    
      Serial.println(results.value, HEX);//print on Serial monitor the value received in hexadecimal.Read the value of the button that you want use and put it into the switch case
  
      
    if (ProcessIR())
    {
       SendSignal();
    }
    irrecv.resume(); //receive the next value
   
    delay(150);//wait 200 millisecond
  }
}

bool ProcessIR() {
  switch (results.value) //check the IR signal received
  {
    case 0xFFA05F:
      if (number1 <100)
      {
        number1 += 20;
      }
      else
      {
        number1 += 50;
      }
      if (number1 > 255)
        number1 = 255;
      //Serial.print("Brighter - ");
      //Serial.println(brightness);
      break;
    case 0xFF20DF:
      if (number1 <101)
      {
        number1 -= 20;
      }
      else
      {
        number1 -= 50;
      }
      if (number1 < 0)
        number1 = 0;
      //Serial.print("Un-brighter - ");
      //Serial.println(brightness);
      break;
    case 0xFF906F:
      mode = "r";
      //Serial.println("Red");
      break;
    case 0xFF10EF:
      mode = "g";
      //Serial.println("Green");
      break;
    case 0xFF50AF:
      mode = "b";
      //Serial.println("Blue");
      break;
    case 0xFFD02F:
      mode = "w";
      //Serial.println("White");
      break;
    case 0xFFE01F:
      if (!power) {
        power = true;
        number1 = 30;
        mode = "w";
        //Serial.println("On");
      }
      break;
    case 0xFF609F:
      if (power)
      {
        power = false;
        number1 = 0;
        mode = "w";
        //Serial.println("Off");
      }
      break;
    case 0xFFC837:
      //Serial.println("Rainbow");
      mode = "~";
      break;
    case 0xFFF00F: //case for music
      mode = "m";
      break;
    case 0xFFE817: //case for rgb
      mode = "a";
      number1 = 30;
      number2 = 30;
      number3 = 30;
      break;
    case 0xFFB04F: //case for R+
      if(mode == "a")
      {
        number1 += 50;
        if (number1 > 255)
          number1 = 255;
      }
      break;
    case 0xFF30CF: //case for G+
      if(mode == "a")
      {
        number2 += 50;
        if (number2 > 255)
          number2 = 255;
      }
      break;
    case 0xFF708F: //case for B+
      if(mode == "a")
      {
        number3 += 50;
        if (number3 > 255)
          number3 = 255;
      }
      break;
    case 0xFFA857: //case for R-
      if(mode == "a")
      {
        number1 -= 50;
        if (number1 < 0)
          number1 = 0;
      }
      break;
    case 0xFF28D7: //case for G- 
      if(mode == "a")
      {
        number2 -= 50;
        if (number2 < 0)
          number2 = 0;
      }
      break;
    case 0xFF6897: //case for B- 
      if(mode == "a")
      {
        number3 -= 50;
        if (number3 < 0)
          number3 = 0;
      }
      break;
    case 0xFF48B7: //rainbow speed up
      if(mode == "~")
      {
         number3 += 50;
      }
      if (number3 > 200)
        number3 = 200;
        break;
    case 0xFF08F7:  //rainbow speed down
      if(mode == "~")
      {
         number3 -= 50;
      }
      if (number3 < 0)
        number3 = 0;
        break;
    case 0xFFFFFFFF:
      if (prevResult.value == 0xFFA05F && number1 < 250)
      {
        number1 += 6;
        //Serial.print("Little brighter - ");
        //Serial.println(brightness);
      }
      else if (prevResult.value == 0xFF20DF  && number1 > 5)
      {
        number1 -= 6;
        //Serial.print("Little un-brighter - ");
        //Serial.println(brightness);
      }
      else if (prevResult.value == 0xFFB04F  && number1 < 250 && mode == "a")
      {
         number1 += 6;
      }
      else if (prevResult.value == 0xFFA857  && number1 > 5 && mode == "a")
      {
         number1 -= 6;
      }
       else if (prevResult.value == 0xFF30CF  && number2 < 250 && mode == "a") 
      {
         number2 += 6;
      }
      else if (prevResult.value == 0xFF28D7  && number2 > 5 && mode == "a")
      {
         number2 -= 6;
      }
       else if (prevResult.value == 0xFF708F  && number3 < 250 && mode == "a")
      {
         number3 += 6;
      }
      else if (prevResult.value == 0xFF6897  && number3 > 5 && mode == "a")  
      {
         number3 -= 6;
      }
      else if (prevResult.value == 0xFF08F7  && number3 > 4 && mode == "~")  
      {
         number3 -= 5;
      }
      else if (prevResult.value == 0xFF48B7  && number3 < 196 && mode == "~")  
      {
         number3 += 5;
      }
      break;
    default:
      return false;
  }
  return true;
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

    switch ( sector ) {
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
