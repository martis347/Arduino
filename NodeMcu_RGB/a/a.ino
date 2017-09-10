
#include <IRremoteInt.h>



int IRreceiver = 2; //pin connected to IR receiver
//IRrecv irrecv(IRreceiver);//create instance of 'IRrecv'
//decode_results results;//create instance of 'decode_results'

//decode_results prevResult;
int number1 = 30; //brightness or RED
String mode = "w";
bool power = false;
int number2 = 0; //GREEN
int number3 = 0; //rainbowSpeed or BLUE
String newSignal = "";

void setup()
{


  Serial.println("Starting...");
}

void loop()
{
  
}


