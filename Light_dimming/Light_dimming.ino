const int ledPinRed1 = 9;    // Red LED connected to analogue out pin
const int ledPinGrn1 = 10;    // Green LED connected to analogue out pin
const int ledPinBlu1 = 11;     // Blue LED connected to analogue out pin

const int ledPinRed2 = 3;    // Red LED connected to analogue out pin
const int ledPinGrn2 = 5;    // Green LED connected to analogue out pin
const int ledPinBlu2 = 6;     // Blue LED connected to analogue out pin

int analogPin = 0; // MSGEQ7 OUT
int strobePin = 3; // MSGEQ7 STROBE
int resetPin = 2; // MSGEQ7 RESET
int spectrumValue[7];
int filterValue = 80;


unsigned int r, g, b;

enum MODE {
  Static,
  RGB
};
enum RGBMode {
  Music,
  Rainbow,
  StaticColor
};

int stripNumber;
enum MODE colorMode;
int staticDimming;
int staticWarmness;
enum RGBMode rgbMode;
int musicColorShift;
int rainbowSpeed;
int rainbowBrightness;
int RGBColors[3];

int rainbowColor = 0;

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(analogPin, INPUT);
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  analogReference(DEFAULT); // 5V
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}

String incoming;
void loop()
{ 
  if (Serial.available() > 0) {
    incoming = Serial.readString();
    parseNetworkMessage(incoming.c_str());
  }

  switch(colorMode)
  {
      case Static:
        HSBToRGB(27, staticWarmness, staticDimming, &r, &g, &b);
        
        break;
      case RGB:
        HandleRGB();
        break;  
  }  

  analogWrite(ledPinRed1, r);
  analogWrite(ledPinGrn1, g);
  analogWrite(ledPinBlu1, b);
  if(rgbMode == Rainbow && colorMode == RGB)
  {
    delay(101-rainbowSpeed);
  }
  else
    delay(10);
}

void HandleRGB()
{
  switch(rgbMode)
  {
      case Music:
        DoMusic();
        break;
        
      case Rainbow:
        if(rainbowColor == 255)
          rainbowColor = 0;
        HSBToRGB(rainbowColor++, 255, rainbowBrightness, &r, &g, &b);
        break;
        
      case StaticColor:
        r = RGBColors[0];
        g = RGBColors[1];
        b = RGBColors[2];
        break;
  }
}

void DoMusic()
{
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
 
  // Get all 7 spectrum values from the MSGEQ7
  for (int i = 0; i < 5; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // Allow output to settle
 
    spectrumValue[i] = analogRead(analogPin);
 
    // Constrain any value above 1023 or below filterValue
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);

 
    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);

    // Remove serial stuff after debugging
    Serial.print(spectrumValue[i]);
    Serial.print(" ");
    digitalWrite(strobePin, HIGH);
  }
  r = spectrumValue[1];
  g = spectrumValue[2];
  b = spectrumValue[3];
}

void parseNetworkMessage(char* message)
{
  char *token;
  int i = 0;
  
  while((token = strtok_r(message, " ", &message)))
  {
      if(i == 0)
      {
        stripNumber = atoi(token);     
      }
      else if(i == 1)
      {
        if(strcmp(token, "S") == 0)
        {
          colorMode = Static;
        }
        else if(strcmp(token, "R") == 0)
        {
          colorMode = RGB;
        }
        Serial.println("CC");
      }
      else if(i == 2)
      {
        staticDimming = atoi(token);
      }
      else if(i == 3)
      {
         staticWarmness = atoi(token);
      }
      else if(i == 4)
      {
        if(strcmp(token, "M") == 0)
          rgbMode = Music;
        else if(strcmp(token, "R") == 0)
          rgbMode = Rainbow;
        else if(strcmp(token, "S") == 0)
          rgbMode = StaticColor;        
      }
      else if(i == 5)
      {
        musicColorShift = atoi(token);
      }
      else if(i == 6)
      {
        rainbowSpeed = atoi(token);
      }
      else if(i == 7)
      {
        rainbowBrightness = atoi(token);
      }
      else if(i == 8)
      {
        RGBColors[0] = atoi(token);
      }
      else if(i == 9)
      {
        RGBColors[1] = atoi(token);
      }
      else if(i == 10)
      {
        RGBColors[2] = atoi(token);
      }
      i++;
  }
    
  Serial.println(message);
  Serial.print(" Strip number: ");Serial.println(stripNumber);
  Serial.print(" Color mode: ");
  if(colorMode == RGB)
    Serial.println("RGB");
  else if(colorMode == Static)
    Serial.println("Static");

  Serial.print(" Static dimming: ");Serial.println(staticDimming);
  Serial.print(" Static warmness: ");Serial.println(staticWarmness);
  Serial.print(" RGB mode: ");
  if(rgbMode == Music)
    Serial.println("Music");
  else if(rgbMode == Rainbow)
    Serial.println("Rainbow");
  else if(rgbMode == StaticColor)
  Serial.println("StaticColor");
  Serial.print(" Music color shift ");Serial.println(musicColorShift);
  Serial.print(" Rainbow speed ");Serial.println(rainbowSpeed);
  Serial.print(" Rainbow brightness ");Serial.println(rainbowBrightness);
  Serial.print(" RGBColors[0] ");Serial.println(RGBColors[0]);
  Serial.print(" RGBColorsL[1] ");Serial.println(RGBColors[1]);
  Serial.print(" RGBColors[2] ");Serial.println(RGBColors[1]);
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
