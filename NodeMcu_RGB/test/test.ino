#include <ESP8266WiFi.h>

#define r1 5  //1
#define g1 4  //2
#define b1 0  //3

#define r2 14 //5
#define g2 12 //6
#define b2 13 //7

void setup() {
    Serial.begin(115200);
//    for(uint8_t t = 4; t > 0; t--) {
//        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
//        Serial.flush();
//        delay(300);
//    }
    
    pinMode(r1, OUTPUT);pinMode(g1, OUTPUT);pinMode(b1, OUTPUT);
    pinMode(r2, OUTPUT);pinMode(g2, OUTPUT);pinMode(b2, OUTPUT);
    
//    WiFi.begin("2 Guys 1 Router", "admin01ADF");
//    while (WiFi.status() != WL_CONNECTED) {
//      delay(500);
//      Serial.print(".");
//    }
//   
//    Serial.println("");
//    Serial.println("WiFi connected");  
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
}
unsigned int r, g, b;
int number2 = 0;
void loop() {
//  analogWrite(r1, 250); analogWrite(r2, 250);
//  analogWrite(g1, 0); analogWrite(g2, 0);
//  analogWrite(b1, 0); analogWrite(b2, 0);
//
//   analogWrite(g1, 250); analogWrite(g2, 250);
//  analogWrite(r1, 0); analogWrite(r2, 0);
//  analogWrite(b1, 0); analogWrite(b2, 0);

   analogWrite(b1, 250); analogWrite(b2, 250);
  analogWrite(r1, 0); analogWrite(r2, 0);
  analogWrite(g1, 0); analogWrite(g2, 0);
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

