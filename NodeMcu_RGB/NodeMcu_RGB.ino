#include <Arduino.h>
#include "IRremoteESP8266.h"//include IRremote library to decode the IR signals 
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include "FS.h"

const int sound = 8;
#define r1 5  //1
#define g1 4  //2
#define b1 15  //4

#define r2 14 //5
#define g2 12 //6
#define b2 13 //7

int number1 = 30; //brightness or RED
char mode = 'w';
bool power = false;
int number2 = 0;
int number3; //rainbowSpeed or BLUE
bool broadcast = false;
String newSignal = "", signal = "";

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String html;
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED:
          Serial.printf("[%u] Disconnected!\n", num);
          break;
        case WStype_CONNECTED: 
        {
          IPAddress ip = webSocket.remoteIP(num);
          Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
          webSocket.sendTXT(num, signal);
          break;
        }     
        case WStype_TEXT: 
        {
          //Serial.print((char *)payload);
          parseNetworkMessage((char *)payload);
          break;
        }
    }
}

void prepareFiles(){
  SPIFFS.begin();
  File file = SPIFFS.open("/index.html", "r");
  if (!file) 
  {
    Serial.println("file open failed");  
  } 
  else
  {
    html = "";
    while (file.available()) {
      //Serial.write(file.read());
      String line = file.readStringUntil('\n');
      Serial.println(line);
      html += line + "\n";
    }
    file.close();
  }

   Serial.print("Loaded file!");
}

void setup() {
    Serial.begin(115200);

    pinMode(r1, OUTPUT);pinMode(g1, OUTPUT);pinMode(b1, OUTPUT);
    pinMode(r2, OUTPUT);pinMode(g2, OUTPUT);pinMode(b2, OUTPUT);
  
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    prepareFiles();

    WiFi.begin("2 Guys 1 Router", "admin01ADF");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
   
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    
    server.onNotFound([]() {
        server.send(200, "text/html", "<!DOCTYPE html><html lang='en'><head><meta http-equiv='X-UA-Compatible' content='IE=edge'><meta charset='utf-8'/><script src='https://cdnjs.cloudflare.com/ajax/libs/bootswatch/3.3.7/fonts/glyphicons-halflings-regular.woff2'></script><meta media='all and (orientation:aa)' name='viewport' content='width=400,height=400,initial-scale=1'><title>Home Control</title><link href='https://martis347.github.io/Home-control/css.css' rel='stylesheet'></head><body><div id='app'></div><script type='text/javascript' src='https://martis347.github.io/Home-control/js.js'></script></body></html>");
    });
    
    server.begin();
    Serial.printf("Server Start\n");
}

void loop() {
  webSocket.loop();
  server.handleClient();

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
  
  signal = GenerateValuesString();
  ProcessLights();
  
  if(broadcast)
  {
    Serial.print("Broadcasting "); 
    Serial.println(signal);  
    webSocket.broadcastTXT(signal);
    broadcast = false;
  }
}

String GenerateValuesString()
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
  newSignal = newSignal + broadcast + " ";
  
  return newSignal;
}

String GenerateValuesString2()
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
  newSignal = newSignal + broadcast + " ";
  
  return newSignal;
}

long mils = millis();
unsigned int r, g, b;

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
        if(number2 == 1024)
          number2 = 0;
        HSBToRGB(number2++, 1024, 1024, &r, &g, &b);
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

  if (mode == '~' && millis() - mils >= number3 / 4)
  {
    number2++;
    if(number2 >= 1024)
      number2 = 0;
    HSBToRGB(number2, 1024, number1, &r, &g, &b);
    mils = millis();
  }
}

void parseNetworkMessage(char* message)
{
  char *token;
  int i = 0;
  while((token = strtok_r(message, " ", &message)))
  {
  	if(i == 0)
  	{
  		power = atoi(token) == 1 ? true : false;  
  	}
  	else if(i == 1)
  	{
  		mode = token[0];
  	}
  	else if(i == 2)
  	{
      number1 = atoi(token);
  	}
  	else if(i == 3 && mode == 'a')
  	{
  		number2 = atoi(token);
  	}
  	else if(i == 4)
  	{
  		number3 = atoi(token);   
  	}
    else if(i == 5)
    {
      broadcast = atoi(token) == 1 ? true : false;   
    }
  	i++;
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
        unsigned int sector = scaledHue >> 10; // sector 0 to 5 around the color wheel
        unsigned int offsetInSector = scaledHue - (sector << 10);  // position within the sector         
        unsigned int p = (inBrightness * ( 1024 - inSaturation )) >> 10;
        unsigned int q = (inBrightness * ( 1024 - ((inSaturation * offsetInSector) >> 10) )) >> 10;
        unsigned int t = (inBrightness * ( 1024 - ((inSaturation * ( 1024 - offsetInSector )) >> 10) )) >> 10;

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

